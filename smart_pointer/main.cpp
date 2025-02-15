#include <iostream>
#include <memory>

class Test {
public:
    Test(int val = 0) : _value(val) {
        std::cout << "Test Constructor: " << _value << std::endl;
    }
    ~Test() {
        std::cout << "Test Destructor: " << _value << std::endl;
    }
    void show() const {
        std::cout << "Value: " << _value << std::endl;
    }

private:
    int _value;
};

struct FileDeleter {
    void operator()(FILE* fp) {
        if (fp) {
            std::cout << "Close file successfully." << std::endl;
            fclose(fp);
        }
    }
};


// shared_ptr用法
void test_shared_ptr() {
    // 创建shared_ptr对象
    auto test = new Test(1);
    // 1. 传入指针，构造shared_ptr对象
    std::shared_ptr<Test> ptr1(test);
    ptr1->show();

    // 直接在构造中传入new创建的对象
    std::shared_ptr<Test> ptr2(new Test(2));
    ptr2->show();

    // 2. 使用移动构造函数，创建shared_ptr对象
    std::shared_ptr<Test> ptr3 = std::shared_ptr<Test>(test);
    ptr3->show();

    // 3. 使用std::make_shared函数(C++14引入)，创建shared_ptr对象
    auto ptr4 = std::make_shared<Test>(4);
    ptr4->show();

    // 4. 通过拷贝构造，创建shared_ptr对象
    auto ptr5 = ptr4;
    ptr5->show();
    std::cout << "------------------------------------------------------" << std::endl;
    std::cout << "After copy, ptr4 use count: " << ptr4.use_count() << std::endl; // 2
    std::cout << "After copy, ptr5 use count: " << ptr5.use_count() << std::endl; // 2

    // 5. 通过拷贝赋值，共享所有权
    std::shared_ptr<Test> ptr6;
    ptr6 = ptr4;
    std::cout << "After assign, ptr4 use count: " << ptr4.use_count() << std::endl; // 3
    std::cout << "After assign, ptr5 use count: " << ptr5.use_count() << std::endl; // 3
    std::cout << "After assign, ptr6 use count: " << ptr6.use_count() << std::endl; // 3
    std::cout << "------------------------------------------------------" << std::endl;

    // 6. 使用reset函数，释放资源
    ptr6.reset();
    std::cout << "After reset, ptr4 use count: " << ptr4.use_count() << std::endl; // 2
    std::cout << "After reset, ptr5 use count: " << ptr5.use_count() << std::endl; // 2
    ptr6->show();

    delete test;
}

// unique_ptr用法
void test_unique_ptr() {
    //  创建一个 unique_ptr
    std::unique_ptr<Test> ptr1(new Test(1));
    ptr1->show();

    // 移动 unique_ptr
    std::unique_ptr<Test> ptr2 = std::move(ptr1);
    ptr2->show();
//    ptr1->show();   // ptr1已经失效，不能使用
    if (!ptr1) {
        std::cout << "ptr1 is now nullptr after move." << std::endl;
    }

    ptr1 = std::unique_ptr<Test>(new Test(2));
    ptr1->show();

    // // 使用 make_unique（C++14 引入）
    std::unique_ptr<Test> ptr3 = std::make_unique<Test>(3);
    ptr3->show();

    // 重置 unique_ptr
    ptr2.reset(new Test(300));
    ptr2->show();

// unique_ptr 自动释放资源
}

// 自定义删除器
void test_deleter() {
    {
        std::shared_ptr<FILE> file_ptr(fopen("test.txt", "w"), FileDeleter());
        if (file_ptr) {
            std::cout << "File opened successfully." << std::endl;
            fprintf(file_ptr.get(), "hello, world!\n");
        }
    }

    {
        auto file_deleter = [](FILE *fp) {
            if (fp) {
                std::cout << "Close file successfully." << std::endl;
                fclose(fp);
            }
        };

        std::unique_ptr<FILE,decltype(file_deleter)> file_ptr(fopen("test.txt", "w"), file_deleter);
        if (file_ptr) {
            std::cout << "File opened successfully." << std::endl;
            fprintf(file_ptr.get(), "try everything!\n");
        }
    }
}

int main() {
//    test_shared_ptr();
//    test_unique_ptr();
    test_deleter();
    return 0;
}
