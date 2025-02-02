//
// Created by yhs on 2024-12-21.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <memory>
#include <thread>
#include <functional>

using std::cout;
using std::endl;

// 定义普通函数
int add(int a, int b) {
    return a + b;
}

struct Multiply {
    int operator()(int a, int b) {
        return a * b;
    }
};

// 定义函数指针类型的变量
int (*func_ptr)(int, int);

// 1. 函数指针
void test_func_ptr() {
    // 函数指针指向add函数并调用
#if 0
    // 方式一:
    // 函数名是函数的地址（类似数组名指的是数组的首地址）
    // 调用函数指针时可以直接调用传参，不需解引用，因为函数指针调用时，编译器会直接找到函数指针指向的函数，
    // 然后再调用该函数传参
    func_ptr = add;
    int result = func_ptr(1,2);
#else
    // 方式二：传统写法
    func_ptr = &add;
    int result = (*func_ptr)(1, 2);
#endif
    cout << "result: " << result << endl;
}


// 2. 仿函数/函数对象 - functor
// 概念：重载了operator()的类和结构体的实例

// 2.1 定义一个仿函数类
struct Adder {
    Adder(int value) : to_add(value) {}

    // 重载()运算符
    int operator()(int x) const {
        return x + to_add;
    }

    void add(int x) {
        to_add += x;
    }

    ~Adder() {
        cout << "Adder::~Adder()" << endl;
    }

    int to_add;
};

// 2.2 仿函数复杂操作： 实现一个可变的仿函数
struct Accumulator {
    Accumulator() : sum(0) {}

    void operator()(int x) {
        sum += x;
    }

    int sum;
};

// 2.3 仿函数：判断一个数是否大于某个阈值
struct IsGreaterThan {
    IsGreaterThan(int t) : threshold(t) {}

    bool operator()(int x) const {
        return threshold < x;
    }

    int threshold;
};

// 2.4 仿函数与模板结合
// 定义通用比较仿函数
template<typename T>
struct Compare {
    bool operator()(const T &a, const T &b) {
        return a < b;
    }
};

void test_functor() {
    // 2.1 定义与基本使用
    Adder add(1);
    cout << "1 + 2 = " << add(2) << endl;
    // 2.2 使用可变的仿函数
    Accumulator acc;
    acc(10);
    acc(20);
    acc(30);
    cout << "sum = " << acc.sum << endl;

    // 2.3 使用仿函数的标准库算法
    std::vector<int> numbers = {1, 4, 3, 10, 2};
    // 使用仿函数进行筛选
    IsGreaterThan g(5);
    auto it = std::find_if(numbers.begin(), numbers.end(), g);
    if (it != numbers.end()) {
        cout << "found :" << *it << endl;
    } else {
        cout << "not found!" << endl;
    }

    //2.4 使用通用比较的仿函数
    std::vector<double> numbers2 = {1.1, 4.1, 3.1, 10.1, 2.1};
    std::sort(numbers2.begin(), numbers2.end(), Compare<double>());
    cout << "after sorting: ";
    for (auto num: numbers2) {
        cout << num << " ";
    }
    cout << endl;
}

// 3. lambda表达式
class Processor {
public:
    Processor(int threshold) : _threshold(threshold) {}

    void process(std::vector<int> &data) {
        cout << "before process data: " << endl;
        for (auto num: data)
            cout << num << " ";
        cout << endl;

        auto new_end = std::remove_if(data.begin(), data.end(), [this](int n) -> bool {
            return n < _threshold;
        });
        data.erase(new_end, data.end());
        cout << "after process data: " << endl;
        for (auto num: data)
            cout << num << " ";
        cout << endl;
    }

private:
    int _threshold;
};


void test_lambda() {
    int threshold = 5;
    // 6  10 8  1 3 2
    std::vector<int> numbers = {1, 6, 3, 10, 8, 2};
    auto new_end = remove_if(numbers.begin(), numbers.end(), [threshold](int x) {
        return x < threshold;
    });


    numbers.erase(new_end, numbers.end());
    for (auto num: numbers) {
        cout << num << " ";
    }
    cout << endl;

    // !!! 使用值捕获方式，改变外部变量内容
    int temp = 10;
    int *ptemp = &temp;
    // 通过传入指针，解引用后间接改变外部变量
    auto lambda = [ptemp](int x) {
        *ptemp *= x;
    };


    // 特殊场景：lambda表达式捕获了普通指针时，如果在其他线程也使用该指针并且回收了内存，
    //         可能会导致非法访问内存，导致程序崩溃或者产生不可预测的结果。这是因为lambda表达式在执行时可能会访问已经被回收的内存。
    // 最佳操作：异步操作下，lambda表达式配合智能指针
    std::thread t1;
    {
        // 在创建lambda表达式捕获智能指针，智能指针的引用计数加一。在调用lambda时不在再增加引用计数。
        auto add_ptr = std::make_shared<Adder>(10);
        auto lambda2 = [add_ptr](int x) {
            cout << "begin lambda2 use count: " << add_ptr.use_count() << endl;
            std::this_thread::sleep_for(std::chrono::seconds(5));
            add_ptr->add(x);
            cout << "end lambda use_count: " << add_ptr.use_count() << endl;
        };
        // 将lambda表达式传给线程，线程内部会依次将lambda表达式所捕获的变量，以值的方式拷贝给在线程内部的成员（即将值拷贝到线程）
        t1 = std::thread(lambda2, 5);
        cout << "before } use_count: " << add_ptr.use_count() << endl;
    }
    t1.join();

    // lambda表达式捕获成员变量，搭配标准库算法
    Processor p(5);
    std::vector<int> data = {1, 6, 3, 10, 8, 2};
    p.process(data);
}

void test_function() {
    // 1. 封装普通函数
    std::function<int(int, int)> func1 = add;
    cout << "Add : " << func1(1, 1) << endl;

    // 2. 封装函数对象/仿函数
    Multiply multiply;
    std::function<int(int, int)> func2 = multiply;
    cout << "Multiply : " << func2(1, 1) << endl;

    // 3. 封装lambda表达式
    std::function<int(int, int)> func3 = [](int a, int b) {
        return a - b;
    };
    cout << "subtract : " << func3(2, 1) << endl;
}

// function对象的用法场景：回调机制（回调函数的传递）
// 定义回调类型
using Callback = std::function<void(int)>;

// 触发事件的函数
void trigger_event(Callback cb, int value) {
    // 事件发生， 调用回调
    cb(value);
}

void test_function_usecase() {

    // 使用lambda作为回调
    trigger_event([](int value){
        std::cout << "事件触发，value: " << value << std::endl;
    }, 24);

    // 使用仿函数作为回调
    struct Printer {
        void operator() (int value) {
            std:cout << "Printer打印value: " << value << std::endl;
        }
    } printer;
    trigger_event(printer, 100);
}

// std::function应用场景2: 存储和调用不同类型的可调用对象
void test_function_usecase2() {
    std::vector<std::function<int(int, int)>> operations;
    operations.emplace_back(add);
    operations.emplace_back(Multiply());
    operations.emplace_back([](int a, int b) {
        return a -b;
    });
    for (auto &op : operations) {
        std::cout << op(5, 10) << std::endl;
    }
}

void display(const std::string& msg, int count) {
    for (int i = 0; i < count; ++i) {
        std::cout << msg << std::endl;
    }
}

void print(int a, int b, int c) {
    std::cout << "print: a: " << a << ", b: " << b << ", c: " << c << std::endl;
}

class Calculator {
public:
    Calculator() : result(0) {}
    int result;
    int multiply(int a, int b) {
        result = a * b;
        return result;
    }
};

class Greeter {
public:
    void greet(const std::string& name) const {
        std::cout << "hello, " << name  << "!" << std::endl;
    }
};

class Logger {
public:
    static void log(const std::string& message) {
        std::cout << "log: " << message << std::endl;
    }
};

void test_bind() {
    // 1. std::bind绑定add的第一个参数，生成新的函数对象
    auto new_add = std::bind(add, 10, std::placeholders::_1);
    cout << "new_add(5) = " << new_add(5) << endl; // 5传给add的第二个参数

    // 2. std::placeholders占位符
    // 绑定msg为"hello", 使用std::placeholders进行占位
    auto say_hello= std::bind(display, "hello", std::placeholders::_1);
    say_hello(2); // 2传给display的第二个参数

    // 绑定count次数为2，生成新的函数对象
    auto say_twice = std::bind(display, std::placeholders::_1, 2);
    say_twice("hi"); // "hello"传给display的未绑定的第一个参数

    // 新生成函数的第三个参数,绑定到原来函数的第一个位置，同理将新生成函数第一个参数，绑定到原来函数的第三个位置
    auto new_func = std::bind(print, std::placeholders::_3, std::placeholders::_2, std::placeholders::_1);
    new_func(1, 2, 3);

    // 使用lamdba表达式绑定参数 (更为直观和高效，比较常用)
    // 绑定第一个参数为2
    auto new_func3 = [] (int b) {
        return add(2, b);
    };
    std::cout << "new_func3(5) = " << new_func3(5) << std::endl;

    // 绑定类的成员函数
    // 方式一: 使用std::bind绑定成员函数
    Calculator calc;
    // 注意：需要在成员函数和对象前加&, 并且需要绑定到一个对象实例上来调用
    auto new_func4 = std::bind(&Calculator::multiply, &calc, 2, std::placeholders::_1);
    std::cout << "new_func4(5) = " << new_func4(5) << std::endl;

    // 使用lambda表达式绑定成员函数
    // 绑定对象实例
    auto new_func5 = [&calc](int a){
        return calc.multiply(2, a);
    };
    std::cout << "new_func5(5) = " << new_func5(5) << std::endl;


    Greeter greeter;
    auto new_func6 = [&greeter](const std::string& name){
        greeter.greet(name);
    };
    new_func6("world");

    // 绑定静态成员函数
    auto log_func = std::bind(&Logger::log, std::placeholders::_1);
    log_func("This is a static log message.");
    auto log_func2 = [](const std::string& message) {
        Logger::log(message);
    };
    log_func2("This is a static log message.");
}


int main() {
//    test_func_ptr();
//    test_functor();
//    test_lambda();
//    test_function();

//    test_function_usecase();
//    test_function_usecase2();
    test_bind();

    return 0;
}