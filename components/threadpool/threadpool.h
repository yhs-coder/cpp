//
// Created by yhs on 2025-05-19.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// Any类型: 可以接收任意数据的类型
class Any {
public:
    // 构造函数可以让Any类型接收任意其它的数据
    template<typename DataType>
    Any(DataType data) : base_(std::make_unique<Derive<DataType>>(data)) {}

    Any() = default;
    ~Any() = default;
    Any(const Any&) = delete;
    Any& operator=(const Any&) = delete;
    Any(Any&&) = default;
    Any& operator=(Any&&) = default;

    // 将Any对象里面存储的data数据提取出来
    template<typename DataType>
    DataType Cast_() {
        auto pd = dynamic_cast<Derive<DataType>*>(base_.get());
        if (pd == nullptr) {
            throw "type is unmatch!";
        }
        return pd->data_;
    }

private:
    // 基类类型
    class Base {
    public:
        // 基类和派生类的析构函数经过编译器处理后统一命名为destructor
        // 如果不声明为虚函数,那么delete了Base时,不会调用Derive的析构函数
        virtual ~Base() = default;
    };

    // 派生类类型
    template<typename T>
    class Derive : public Base {
    public:
        Derive(T data) : data_(data) {}
        ~Derive() = default;
        // 接收任意类型的数据
        T data_;
    };

private:
    // 基类指针,指向Derive派生类,从而可以获取Derive类中的data数据
    std::unique_ptr<Base> base_;
};

class Result;// Result类的前置声明

// 任务抽象基类
class Task {
public:
    Task();
    ~Task() = default;
    void Exec();
    void SetResult(Result* res);

    // 用户可以自定义任意任务类型，从Task抽象基类继承，重写Run方法，实现自定义任务处理
    virtual Any Run() = 0;

private:
    Result* result_;    // 使用裸指针指向Result对象， Result对象的生命周期要 大于 Task
};

// 实现一个信号量类
class Semaphore {
public:
    Semaphore(int resource_limit = 0);
    ~Semaphore() = default;

    // 获取一个信号量资源
    void Wait();

    // 增加一个信号量资源
    void Post();

private:
    int resource_limit_;    // 信号量资源
    std::mutex mtx_;
    std::condition_variable condition_;
};

// Result类, 用于接收提交到线程池的task任务执行完成后的返回值类型
class Result {
public:
    Result(std::shared_ptr<Task> task, bool is_valid = true);
    ~Result() = default;

    // 获取任务执行完的返回值
    void SetValue(Any any);

    // 返回任务执行完的结果
    Any Get();

private:
    Any any_;                   // 存储任务返回值
    Semaphore semaphore_;       // 线程通信 - 信号量
    std::shared_ptr<Task> task_;// 指向对应获取返回值的任务对象
    std::atomic_bool is_valid_; // 判断返回值是否有效
};

// 线程类型
class Thread {
public:
    // 线程函数对象类型
    using ThreadFunc = std::function<void(size_t)>;
    Thread(ThreadFunc func);
    ~Thread() = default;

    // 启动线程
    void Start();

    // 获取Thread对象的id
    size_t GetID() const;

private:
    ThreadFunc func_;
    static size_t generate_id_;   // 用于给每个id赋值
    size_t thread_id_;            // 每个Thread对象唯一标识的id,用于寻找Thread对象
};

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
    ThreadPool();
    ~ThreadPool();
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool) = delete;

    // 设置线程池的工作模式
    void SetMode(PoolMode mode);

    // 设置初始的线程数量
    void SetInitThreadSize(size_t size);

    //  开启线程池
    void Start(size_t init_thread_size = 4);

    // 设置task任务队列上限阈值
    void SetTaskQueueThreshold(size_t threshold);

    // 设置线程池cached模式下上限阈值
    void SetThreadSizeThreshold(size_t threshold);

    // 往线程池中提交任务
    Result SubmitTask(std::shared_ptr<Task> sp);

private:
    // 定义线程函数
    void ThreadEntry(size_t thread_id);

    // 检查线程池的运行状态
    bool CheckRunningState() const;

private:
//    std::vector<std::unique_ptr<Thread>> threads_;                // 线程池中的线程列表
    std::unordered_map<size_t, std::unique_ptr<Thread>>threads_;    // 线程池中的线程列表
    size_t init_thread_size_;                                       // 初始的线程数量
    std::atomic_uint idle_thread_size_;                             // 记录空闲线程的数量
    size_t thread_size_threshold_;                                  // 线程数量上限阈值
    std::atomic_uint current_thread_size_;                          // 记录当前线程池中线程的总数量

    std::queue<std::shared_ptr<Task>> task_queue_;                  // 任务队列,使用智能指针管理传入的对象，自动释放资源
    std::atomic_uint task_size_;                                    // 任务数量
    size_t task_queue_threshold_;                                   // 任务队列的阈值

    std::mutex task_queue_mutex_;                                   // 任务队列的互斥锁，保证任务列队的线程安全
    std::condition_variable not_full_;                              // 任务队列非满条件变量
    std::condition_variable not_empty_;                             // 任务队列非空条件变量

    PoolMode pool_mode_;                                            // 线程池的工作模式
    std::atomic_bool is_pool_running_;                              // 表示当前线程池的启动状态，用于状态管理
};


#endif//THREADPOOL_H
