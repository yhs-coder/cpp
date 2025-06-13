#include <functional>
#include <iostream>
#include <thread>
#include <future>

/*
 * 如何让线程池提交任务更加方便?
 * 1. 能够将提交任务设置成类似线程对象创建那般的做法，每个函数表示一个任务
 * 2. 为了能够接收任意个数的参数 -> 使用可变参数模板编程
 * pool.SubmitTask(sum1, 1, 1)
 *
 * 3.使用 packaged_task(function函数对象) 打包任务
 *   使用std::future来代替原先的Result，用它来保存任务执行完的结果
 * */

int sum1(int a, int b) {
    std::this_thread::sleep_for(std::chrono::seconds(5));
    return a + b;
}
int sum2(int a, int b, int c) {
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
    test2();
    return 0;
}