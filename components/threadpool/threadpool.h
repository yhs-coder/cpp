//
// Created by yhs on 2025-05-19.
//

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

// 任务抽象基类
class Task {
public:
    // 用户可以自定义任意任务类型，从Task抽象基类继承，重写Run方法，实现自定义任务处理
    virtual void Run() = 0;
};

// 线程对象
class Thread {
public:
private:
};

// 线程池支持的模式
enum class PoolMode {
    MODE_FIXED,     // 固定线程数
    MODE_CACHED,    // 线程数量可动态增长
};

class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();

    // 设置线程池的工作模式
    void SetMode(PoolMode mode);

    //  开启线程池
    void Start();

private:
    std::vector<Thread*> threads_;                  // 线程池中的线程列表
    size_t init_thread_size_{};                       // 初始的线程数量

    std::queue<std::shared_ptr<Task>> task_queue_;  // 任务队列,使用智能指针管理传入的对象，自动释放资源
    std::atomic_uint task_size_{};                    // 任务数量
    size_t task_queue_threshhold_{};                  // 任务队列的阈值

    std::mutex task_queue_mutex_;                   // 任务队列的互斥锁，保证任务列队的线程安全
    std::condition_variable not_full_;              // 任务队列非满条件变量
    std::condition_variable not_empty_;             // 任务队列非空条件变量

};


#endif //THREADPOOL_H
