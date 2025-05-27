#include <chrono>
#include <iostream>

#include "threadpool.h"

class MyTask : public Task {
public:
    void Run() override {
        std::cout << "begin..." << std::endl;
        std::cout << "thread id: " << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(5));
        std::cout << "end..." << std::endl;
    }
};

int main() {
    ThreadPool pool;
    pool.Start(4);
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    pool.SubmitTask(std::make_shared<MyTask>());
    //    std::this_thread::sleep_for(std::chrono::seconds(5));
    getchar();
    return 0;
}
