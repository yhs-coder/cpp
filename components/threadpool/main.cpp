#include <chrono>
#include <iostream>

#include "threadpool.h"

using ulong = unsigned long long;

class MyTask : public Task {
public:
    MyTask(int begin, int end) : begin_(begin), end_(end) {}
    Any Run() override {
//        std::cout << "begin..." << std::endl;
//        std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
//        std::this_thread::sleep_for(std::chrono::seconds(5));
//        std::cout << "end..." << std::endl;
        ulong sum = 0;
        for (int i = begin_; i < end_; i++) {
            sum += i;
        }
        std::cout << "tid: " << std::this_thread::get_id() << "...." << std::endl;
        std::cout << "sum = " << sum << std::endl;
        return sum;
    }
private:
    int begin_;
    int end_;
};

class Test {

};

int main() {
#if 1
    ThreadPool pool;
    // 用户自己设置线程池的工作模式
    pool.SetMode(PoolMode::MODE_CACHED);
    // 开始启动线程池
    pool.Start(4);
    Result res1 = pool.SubmitTask(std::make_shared<MyTask>(1, 1000000));
    Result res2 = pool.SubmitTask(std::make_shared<MyTask>(1000001, 2000000));
    Result res3 = pool.SubmitTask(std::make_shared<MyTask>(2000001, 3000000));

    ulong sum1 = res1.Get().Cast_<ulong>();
    ulong sum2 = res2.Get().Cast_<ulong>();
    ulong sum3 = res3.Get().Cast_<ulong>();
    std::cout << "执行完成！" << std::endl;
    // Master - Slave线程模型
    // Master线程用来分解任务，然后给各个Slave线程分配任务
    // 等待各个Slave线程执行完任务，返回结果
    // Master线程合并各个任务结果，输出
    std::cout << (sum1 + sum2 + sum3) << std::endl;

//    pool.SubmitTask(std::make_shared<MyTask>());
//    pool.SubmitTask(std::make_shared<MyTask>());
//    pool.SubmitTask(std::make_shared<MyTask>());
//    pool.SubmitTask(std::make_shared<MyTask>());
//    pool.SubmitTask(std::make_shared<MyTask>());
//    pool.SubmitTask(std::make_shared<MyTask>());
//    pool.SubmitTask(std::make_shared<MyTask>());
//    pool.SubmitTask(std::make_shared<MyTask>());
//    pool.SubmitTask(std::make_shared<MyTask>());
//    pool.SubmitTask(std::make_shared<MyTask>());
    //    std::this_thread::sleep_for(std::chrono::seconds(5));
    getchar();
#else
    // 随着task被执行完，task对象没了，依赖于task对象的Result对象也销毁了
    std::cout << "Any类的大小: " << sizeof(Any) << std::endl;

#endif
    return 0;
}
