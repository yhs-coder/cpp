#include <functional>
#include <iostream>
#include <thread>
#include <future>
#include "threadpool.hpp"


int sum1(int a, int b) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return a + b;
}
int sum2(int a, int b, int c) {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return a + b + c;
}

void test() {
    std::thread t1(sum1, 1, 1);
    std::thread t2(sum2, 2, 2, 2);
    t1.join();
    t2.join();
}
void test2() {
    // 使用  std::packaged_task 将函数打包
   std::packaged_task<int(int, int)> task(sum1);
   // 使用get_future() 函数返回future对象
   // std::future类和先前线程池中Result类相似，保存任务执行完成的结果d
   std::future<int> res = task.get_future();
//   task(100, 100);
   std::thread t(std::move(task), 200,200);
   t.detach();
   // 通过future对象中的get() 获取任务（函数）的返回值
   // get()会阻塞等待std::packaged_task的任务完成，而不是等待等待线程，所以上述需要执行线程分离，或者join等待线程
   std::cout << "task result: " << res.get() << std::endl;
   std::cout << "over..." << std::endl;
}


int main() {
//    test();
//    test2();

    // 测试线程池
    ThreadPool pool;
//    pool.SetMode(PoolMode::MODE_CACHED);
    pool.Start(2);

    std::future<int> r1 = pool.SubmitTask(sum1, 10, 10);
    std::future<int> r2 = pool.SubmitTask(sum2, 10, 10, 10);
    std::future<int> r3 = pool.SubmitTask([](int a, int b) ->int{
        int sum = 0;
        for (int i = a; i <= b; i++) {
            sum += i;
        }
        return sum;
    }, 1, 100);
    std::future<int> r4 = pool.SubmitTask(sum2, 10, 20, 30);
    std::future<int> r5 = pool.SubmitTask(sum2, 30, 30, 30);
    std::cout  << r1.get() << std::endl;
    std::cout  << r2.get() << std::endl;
    std::cout  << r3.get() << std::endl;
    std::cout  << r4.get() << std::endl;
    std::cout  << r5.get() << std::endl;
    return 0;
}