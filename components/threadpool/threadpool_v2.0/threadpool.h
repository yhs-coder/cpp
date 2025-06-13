
#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

const size_t TASK_MAX_THRESHOLD = INT32_MAX;
const size_t THREAD_MAX_THRESHOLD = 100;
const size_t THREAD_MAX_IDLE_TIME = 60;// 单位是秒

// 线程类型
class Thread {
public:
    // 线程函数对象类型
    using ThreadFunc = std::function<void(size_t)>;
    Thread(ThreadFunc func) : func_(func), thread_id_(generate_id_++) {}
    ~Thread() = default;

    // 启动线程
    void Start() {
        // 创建一个真实的线程，来执行一个线程函数
        auto t = std::thread(func_, thread_id_);
        // 线程分离
        t.detach();
    }

    // 获取Thread对象的id
    size_t GetID() const {
        return thread_id_;
    }

private:
    ThreadFunc func_;
    static size_t generate_id_;// 用于给每个id赋值
    size_t thread_id_;         // 每个Thread对象唯一标识的id,用于寻找Thread对象
};
size_t Thread::generate_id_ = 0;

// 线程池支持的模式
enum class PoolMode {
    MODE_FIXED, // 固定线程数
    MODE_CACHED,// 线程数量可动态增长
};

/*
 * example:
 * ThreadPool pool;
 * pool.start(4);
 * class MyTask : public Task {
 *     void Run() { // 任务代码}
 * };
 * pool.SubmitTask(new MyTask());
 * // 或者pool.SubmitTask(std::make_shared<MyTask>());
 */
class ThreadPool {
public:
    ThreadPool()
        : init_thread_size_(4),
          idle_thread_size_(0),
          thread_size_threshold_(THREAD_MAX_THRESHOLD),
          current_thread_size_(0),
          task_size_(0),
          task_queue_threshold_(TASK_MAX_THRESHOLD),
          pool_mode_(PoolMode::MODE_FIXED),
          is_pool_running_(false) {}

    ~ThreadPool() {
        is_pool_running_ = false;
        // 唤醒所有处于等待状态的线程，回收线程资源
        not_empty_.notify_all();
        // 等待线程池中所有的线程返回，但此时线程有两种情况：1. 线程阻塞 2. 线程正在执行任务中
        std::unique_lock<std::mutex> lock(task_queue_mutex_);
        // 当线程列表为空时，表示线程资源回收完毕。不然一直等待线程执行完成
        exit_condition_.wait(lock, [&]() -> bool { return threads_.size() == 0; });
    }
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool) = delete;

    // 设置线程池的工作模式
    void SetMode(PoolMode mode) {
        // 如果线程池已经在运行，不允许进行设置
        if (CheckRunningState())
            return;
        pool_mode_ = mode;
    }

    // 设置初始的线程数量
    void SetInitThreadSize(size_t size) {
        init_thread_size_ = size;
    }


    //  开启线程池, 获取计算机CPU数量
    void Start(size_t init_thread_size = std::thread::hardware_concurrency()) {
        // 设置线程池为运行状态
        is_pool_running_ = true;

        // 记录初始线程个数
        init_thread_size_ = init_thread_size;
        // 记录当前线程个数
        current_thread_size_ = init_thread_size;

        // 创建线程对象
        for (size_t i = 0; i < init_thread_size_; ++i) {
            // 创建线程对象的时候，把线程函数给到thread线程对象
            // 使用std::bind(&ThreadPool::ThreadFunc, this) 生成一个可调用对象
            auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::ThreadEntry, this, std::placeholders::_1));
            //        threads_.emplace_back(new
            //        Thread(std::bind(&ThreadPool::ThreadEntry, this)));
            //        threads_.emplace_back(std::move(ptr));
            threads_.emplace(ptr->GetID(), std::move(ptr));
        }

        // 启动所有线程
        for (size_t i = 0; i < init_thread_size_; ++i) {
            // 在这里真正创建线程对象，内部分离线程
            threads_[i]->Start();
            // 记录初始空闲线程的数量
            idle_thread_size_++;
        }
    }


    // 设置task任务队列上限阈值
    void SetTaskQueueThreshold(size_t threshold) {
        // 如果线程池已经在运行，不允许进行设置
        if (CheckRunningState())
            return;
        task_queue_threshold_ = threshold;
    }

    // 设置线程池cached模式下上限阈值
    void SetThreadSizeThreshold(size_t threshold) {
        if (CheckRunningState())
            return;
        // 在cached模式下，才可设置线程数量上限阈值
        if (pool_mode_ == PoolMode::MODE_CACHED) {
            thread_size_threshold_ = threshold;
        }
    }

    // 往线程池中提交任务
    template<typename Func, typename... Args>
    auto SubmitTask(Func&& func, Args&&... args) -> std::future<decltype(func(args...))> {

    }

private:
    // 定义线程函数
    void ThreadEntry(size_t thread_id) {
        //    std::cout << "begin ThreadEntry..." << std::endl;
        //    std::cout << "thread id: " << std::this_thread::get_id() << std::endl;

        // 记录线程开始执行的时间
        auto last_time = std::chrono::high_resolution_clock::now();
        // 所有任务必须执行完成，线程池才可以回收所有线程资源
        for (;;) {
            std::shared_ptr<Task> task;
            {
                // 获取锁
                std::unique_lock<std::mutex> lock(task_queue_mutex_);
                std::cout << "tid: " << std::this_thread::get_id() << "尝试获取任务..." << std::endl;

                // cached模式下，可能已经创建了很多线程，如果空闲时间超过60s,应该把多余的线程结束回收掉
                // 超过init_thread_size_数量的空闲线程才需要进行回收
                // 锁 + 双重判断
                while (task_queue_.empty()) {
                    // 线程池要结束，回收线程资源
                    if (!is_pool_running_) {
                        // 线程池要结束了，回收线程资源
                        threads_.erase(thread_id);
                        std::cout << "threadid: " << std::this_thread::get_id() << " exit..." << std::endl;
                        // 唤醒ThreadPool析构函数上等待的条件变量
                        exit_condition_.notify_all();
                    }
                    // cached模式下，可能已经创建了很多线程，如果空闲时间超过60s,应该把多余的线程结束回收掉
                    // 超过init_thread_size_数量的空闲线程才需要进行回收
                    if (pool_mode_ == PoolMode::MODE_CACHED) {
                        // 每隔一秒返回一次
                        // 通过wait_for的返回值，区分超时返回和任务待执行返回
                        if (std::cv_status::timeout == not_empty_.wait_for(lock, std::chrono::seconds(1))) {
                            auto now_time = std::chrono::high_resolution_clock::now();
                            auto dur = std::chrono::duration_cast<std::chrono::seconds>(now_time - last_time);
                            if (dur.count() >= THREAD_MAX_IDLE_TIME && current_thread_size_ > init_thread_size_) {
                                // 开始回收多余的空闲线程
                                // 把Thread对象从线程列表中删除
                                threads_.erase(thread_id);
                                // 修改线程数量相关变量的值
                                current_thread_size_--;
                                idle_thread_size_--;
                                std::cout << "threadid: " << std::this_thread::get_id() << " exit..." << std::endl;
                                // 删除线程后，无须在向下执行，直接返回
                                return;
                            }
                        }
                    } else {
                        // 等待not_empty_上的条件满足
                        not_empty_.wait(lock);
                    }
                    // 线程池要结束了，回收线程资源
                    // 回收线程时的情况：线程处于等待状态被唤醒
                    /*if (!is_pool_running_) {
                    threads_.erase(thread_id);
                    std::cout << "threadid: " << std::this_thread::get_id() << " exit..." << std::endl;
                    // 唤醒ThreadPool析构函数上等待的条件变量
                    exit_condition_.notify_all();
                    // 删除线程后，无须在向下执行，直接返回
                    return;
                }*/
                }
                // 需要线程处理，空闲线程数量--
                idle_thread_size_--;
                std::cout << "tid: " << std::this_thread::get_id() << "获取任务成功..." << std::endl;

                // 取出任务队列的元素，并且任务数量--
                task = task_queue_.front();
                task_queue_.pop();
                task_size_--;

                // 如果任务队列上还有剩余的任务，就继续通知其它在not_empty_上等待线程去执行任务
                if (!task_queue_.empty()) {
                    not_empty_.notify_all();
                }

                // 取出任务后，任务队列必不为空。
                // 通知所有在not_full_上等待的线程，通知可以继续提交生产任务
                not_full_.notify_all();
            }// 出了局部作用域，锁自动释放，让别的线程获取锁执行任务

            // 任务非空时，当前线程才执行这个任务
            if (task != nullptr) {
                // task->Run();
                task->Exec();
            }
            // 当线程处理完任务，空闲线程数量++
            idle_thread_size_++;

            // 更新线程执行完任务的时间
            last_time = std::chrono::high_resolution_clock::now();
        }
    }


    // 检查线程池的运行状态
    bool CheckRunningState() const {
        return is_pool_running_;
    }

private:
    //    std::vector<std::unique_ptr<Thread>> threads_;                // 线程池中的线程列表
    std::unordered_map<size_t, std::unique_ptr<Thread>> threads_;// 线程池中的线程列表
    size_t init_thread_size_;                                    // 初始的线程数量
    std::atomic_uint idle_thread_size_;                          // 记录空闲线程的数量
    size_t thread_size_threshold_;                               // 线程数量上限阈值
    std::atomic_uint current_thread_size_;                       // 记录当前线程池中线程的总数量

    using Task = std::function<void()>;
    std::queue<std::shared_ptr<Task>> task_queue_;// 任务队列,使用智能指针管理传入的对象，自动释放资源
    std::atomic_uint task_size_;                  // 任务数量
    size_t task_queue_threshold_;                 // 任务队列的阈值

    std::mutex task_queue_mutex_;           // 任务队列的互斥锁，保证任务列队的线程安全
    std::condition_variable not_full_;      // 任务队列非满条件变量
    std::condition_variable not_empty_;     // 任务队列非空条件变量
    std::condition_variable exit_condition_;// 线程池析构时，等待线程资源全部回收

    PoolMode pool_mode_;              // 线程池的工作模式
    std::atomic_bool is_pool_running_;// 表示当前线程池的启动状态，用于状态管理
};


#endif//THREADPOOL_H
