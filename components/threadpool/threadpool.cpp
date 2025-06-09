#include "threadpool.h"

#include <iostream>

const size_t TASK_MAX_THRESHOLD = INT32_MAX;
const size_t THREAD_MAX_THRESHOLD = 100;
const size_t THREAD_MAX_IDLE_TIME = 60;// 单位是秒

Semaphore::Semaphore(int resource_limit) : resource_limit_(resource_limit) {}

void Semaphore::Wait() {
    std::unique_lock<std::mutex> lock(mtx_);
    // 等待信号量有资源,没有资源遍阻塞当前线程
    condition_.wait(lock, [&]() -> bool { return resource_limit_ > 0; });
    resource_limit_--;
}

void Semaphore::Post() {
    std::unique_lock<std::mutex> lock(mtx_);
    resource_limit_++;
    condition_.notify_all();
}

/************************* Thread类的实现 *************************/
size_t Thread::generate_id_ = 0;
Thread::Thread(ThreadFunc func) : func_(func), thread_id_(generate_id_++) {}

void Thread::Start() {
    // 创建一个真实的线程，来执行一个线程函数
    auto t = std::thread(func_, thread_id_);
    // 线程分离
    t.detach();
}

size_t Thread::GetID() const {
    return thread_id_;
}


/************************* ThreadPool类的实现 *************************/
ThreadPool::ThreadPool()
    : init_thread_size_(4),
      idle_thread_size_(0),
      thread_size_threshold_(THREAD_MAX_THRESHOLD),
      current_thread_size_(0),
      task_size_(0),
      task_queue_threshold_(TASK_MAX_THRESHOLD),
      pool_mode_(PoolMode::MODE_FIXED),
      is_pool_running_(false) {}

ThreadPool::~ThreadPool() {
    is_pool_running_ = false;
    // 唤醒所有处于等待状态的线程，回收线程资源
    not_empty_.notify_all();
    // 等待线程池中所有的线程返回，但此时线程有两种情况：1. 线程阻塞 2. 线程正在执行任务中
    std::unique_lock<std::mutex> lock(task_queue_mutex_);
    // 当线程列表为空时，表示线程资源回收完毕。不然一直等待线程执行完成
    exit_condition_.wait(lock, [&]() -> bool { return threads_.size() == 0; });
}

void ThreadPool::Start(size_t init_thread_size) {
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

void ThreadPool::ThreadEntry(size_t thread_id) {
    //    std::cout << "begin ThreadEntry..." << std::endl;
    //    std::cout << "thread id: " << std::this_thread::get_id() << std::endl;

    // 记录线程开始执行的时间
    auto last_time = std::chrono::high_resolution_clock::now();
    // 根据线程池状态，选择是否循环处理任务队列上的任务
    while (is_pool_running_) {
        std::shared_ptr<Task> task;
        {
            // 获取锁
            std::unique_lock<std::mutex> lock(task_queue_mutex_);
            std::cout << "tid: " << std::this_thread::get_id() << "尝试获取任务..." << std::endl;

            // cached模式下，可能已经创建了很多线程，如果空闲时间超过60s,应该把多余的线程结束回收掉
            // 超过init_thread_size_数量的空闲线程才需要进行回收
            // 锁 + 双重判断
            while (is_pool_running_ && task_queue_.empty()) {
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
            if (!is_pool_running_) {
                break;
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

    // 线程池要结束了，回收线程资源
    // 回收线程时的情况：任务到这执行完任务
    threads_.erase(thread_id);
    std::cout << "threadid: " << std::this_thread::get_id() << " exit..." << std::endl;
    // 唤醒ThreadPool析构函数上等待的条件变量
    exit_condition_.notify_all();
}

bool ThreadPool::CheckRunningState() const {
    return is_pool_running_;
}

void ThreadPool::SetInitThreadSize(size_t size) {
    init_thread_size_ = size;
}

void ThreadPool::SetMode(PoolMode mode) {
    // 如果线程池已经在运行，不允许进行设置
    if (CheckRunningState())
        return;
    pool_mode_ = mode;
}

void ThreadPool::SetTaskQueueThreshold(size_t threshold) {
    // 如果线程池已经在运行，不允许进行设置
    if (CheckRunningState())
        return;
    task_queue_threshold_ = threshold;
}

void ThreadPool::SetThreadSizeThreshold(size_t threshold) {
    if (CheckRunningState())
        return;
    // 在cached模式下，才可设置线程数量上限阈值
    if (pool_mode_ == PoolMode::MODE_CACHED) {
        thread_size_threshold_ = threshold;
    }
}

Result ThreadPool::SubmitTask(std::shared_ptr<Task> sp) {
    // 获取锁，搭配条件变量对任务列队进行操作
    std::unique_lock<std::mutex> lock(task_queue_mutex_);

    // 线程的通信，等待任务列队有空余
    //    while (task_queue_.size() == task_queue_threshold_) {
    //        not_full_.wait(lock);
    //    }

    // 用户提交任务最长不能阻塞超过1秒， 否则判断提交任务失败，直接返回
    // 如果不满足条件，则会一直等待，并且将锁释放
    if (!not_full_.wait_for(lock, std::chrono::seconds(1), [&]() -> bool {
            return task_queue_.size() < task_queue_threshold_;
        })) {
        // 表示not_full_等待了1s后，条件依旧没有满足
        std::cerr << "The task queue is full, submit task failed." << std::endl;
        return Result(sp, false);
    }

    // 如果有空余，将传入的任务插入到任务队列
    task_queue_.emplace(sp);
    task_size_++;

    // 插入后任务队列非空，在not_empty_上通知线程池分配线程来处理该任务
    not_empty_.notify_all();

    // cached模式，使用场景：适合任务处理比较紧急，小而快的任务。
    // 但不适合耗时多的任务，因为耗时任务会长时间占用线程，这种情况选择fixed模式
    // 需要根据任务数量和空闲线程的数量，判断是否需要创建新的线程出来
    if (pool_mode_ == PoolMode::MODE_CACHED && task_size_ > idle_thread_size_ && current_thread_size_ < thread_size_threshold_) {
        std::cout << ">>> create new thread..." << std::endl;

        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::ThreadEntry, this, std::placeholders::_1));
        //        threads_.emplace_back(std::move(ptr));
        size_t thread_id = ptr->GetID();
        threads_.emplace(thread_id, std::move(ptr));
        // 启动线程，内部创建新的线程对象
        threads_[thread_id]->Start();
        // 修改线程个数相关的变量
        current_thread_size_++;
        idle_thread_size_++;
    }

    // 返回任务的Result对象
    return Result(sp);
}


/************************* Result类的实现 *************************/
Result::Result(std::shared_ptr<Task> task, bool is_valid) : task_(task), is_valid_(is_valid) {
    task_->SetResult(this);
}

void Result::SetValue(Any any) {
    // 存储任务执行完的返回值
    any_ = std::move(any);
    // 获取到任务的返回值后，增加信号量资源
    semaphore_.Post();
}

Any Result::Get() {// 提供给用户调用
    if (!is_valid_) {
        return "";
    }
    // 如果task任务没有执行完，在这里阻塞用户的线程
    semaphore_.Wait();
    return std::move(any_);
}


/************************* Task类的实现 *************************/

Task::Task() : result_(nullptr) {}

void Task::Exec() {
    if (result_ != nullptr) {
        // 这里发生多态
        result_->SetValue(Run());
    }
}
void Task::SetResult(Result *res) {
    result_ = res;
}
