#include <iostream>
#include "threadpool.h"

const size_t TASK_MAX_THRESHOLD = 1024;

ThreadPool::ThreadPool()
    : init_thread_size_(4)
    , task_size_(0)
    , task_queue_threshold_(TASK_MAX_THRESHOLD)
    , pool_mode_(PoolMode::MODE_FIXED)
{}

ThreadPool::~ThreadPool() {

}

void ThreadPool::Start(size_t init_thread_size) {
    // 记录初始线程个数
    init_thread_size_ = init_thread_size;

    // 创建线程对象
    for (size_t i = 0; i < init_thread_size_; ++i) {
        // 创建线程对象的时候，把线程函数给到thread线程对象
        // 使用std::bind(&ThreadPool::ThreadFunc, this) 生成一个可调用对象
        threads_.emplace_back(new Thread(std::bind(&ThreadPool::ThreadEntry, this)));
    }

    // 启动所有线程
    for (size_t i = 0; i < init_thread_size_; ++i) {
        // 执行线程函数
        threads_[i]->Start();
    }
}

void ThreadPool::ThreadEntry() {
    std::cout << "begin ThreadEntry..." << std::endl;
    std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
    std::cout << "end ThreadEntry..." << std::endl;
}

Thread::Thread(ThreadFunc func)
    : func_(func)
{}

void Thread::Start() {
    // 创建一个真实的线程，来执行一个线程函数
    std::thread t(func_);
    // 分离线程
    t.detach();
}

