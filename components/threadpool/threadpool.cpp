#include "threadpool.h"

#include <iostream>

const size_t TASK_MAX_THRESHOLD = 4;

Semaphore::Semaphore(int resource_limit) : resource_limit_(resource_limit) {}

void Semaphore::wait() {
    std::unique_lock<std::mutex> lock(mtx_);
    // 等待信号量有资源,没有资源遍阻塞当前线程
    condition_.wait(lock, [&]() -> bool { return resource_limit_ > 0; });
    resource_limit_--;
}

void Semaphore::post() {
    std::unique_lock<std::mutex> lock(mtx_);
    resource_limit_++;
    condition_.notify_all();
}

Thread::Thread(ThreadFunc func) : func_(func) {}

void Thread::Start() {
    // 创建一个真实的线程，来执行一个线程函数
    auto t = std::thread(func_);
    t.detach();
}

ThreadPool::ThreadPool()
    : init_thread_size_(4),
      task_size_(0),
      task_queue_threshold_(TASK_MAX_THRESHOLD),
      pool_mode_(PoolMode::MODE_FIXED) {}

ThreadPool::~ThreadPool() {}

void ThreadPool::Start(size_t init_thread_size) {
    // 记录初始线程个数
    init_thread_size_ = init_thread_size;

    // 创建线程对象
    for (size_t i = 0; i < init_thread_size_; ++i) {
        // 创建线程对象的时候，把线程函数给到thread线程对象
        // 使用std::bind(&ThreadPool::ThreadFunc, this) 生成一个可调用对象
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::ThreadEntry, this));
        //        threads_.emplace_back(new
        //        Thread(std::bind(&ThreadPool::ThreadEntry, this)));
        threads_.emplace_back(std::move(ptr));
    }

    // 启动所有线程
    for (size_t i = 0; i < init_thread_size_; ++i) {
        // 执行线程函数
        threads_[i]->Start();
    }
}

void ThreadPool::ThreadEntry() {
    //    std::cout << "begin ThreadEntry..." << std::endl;
    //    std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
    // 一直循环处理任务队列上的任务
    for (;;) {
        std::shared_ptr<Task> task;
        {
            // 获取锁
            std::unique_lock<std::mutex> lock(task_queue_mutex_);
            std::cout << "tid: " << std::this_thread::get_id() << "尝试获取任务..." << std::endl;

            // 等待not_empty_上的条件满足
            not_empty_.wait(lock, [&]() -> bool { return task_queue_.size() > 0; });
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
            task->Run();
        }
    }
}

void ThreadPool::SetInitThreadSize(size_t size) {
    init_thread_size_ = size;
}

void ThreadPool::SetTaskQueueThreshold(size_t threshold) {
    task_queue_threshold_ = threshold;
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
        return Result(sp,false);
    }

    // 如果有空余，将传入的任务插入到任务队列
    task_queue_.emplace(sp);
    task_size_++;

    // 插入后任务队列非空，在not_empty_上通知线程池分配线程来处理该任务
    not_empty_.notify_all();
    return Result(sp);
}


/************************* Result类的实现 *************************/
Result::Result(std::shared_ptr<Task> task, bool is_valid) : task_(task), is_valid_(is_valid) {}

/************************* Task类的实现 *************************/

Task::Task() : result_(nullptr) {}

void Task::Exec() {
    if (result_ != nullptr) {

    }
}
