//
// Created by yhs on 2024-12-08.
//

#include <iostream>
#include <cstdlib>
#include "Person.h"
#include "DynamicArray.h"
#include "MemoryPool.h"

// C风格内存管理: 使用malloc和free进行内存管理
void test_malloc_free() {
    // 1. 使用malloc分配内存
    int *p = (int *) malloc(sizeof(int));
    if (p == nullptr) {
        std::cerr << "Memory allocation failed" << std::endl;
        return;
    }

    *p = 12;
    std::cout << "Value at p: " << *p << std::endl;
    free(p);

    // 2. malloc分配的内存并未初始化
    Person *person = (Person *) malloc(sizeof(Person));
    if (person == nullptr) {
        std::cerr << "Memory allocation failed" << std::endl;
        return;
    }

    // 输出结果是随机值
//    std::cout << "Person's age:: " << person->_age << " , name: " << person->_name << std::endl;
    free(person);

    // 3. 使用malloc分配一个数组大小的内存
    int *arr = (int *) malloc(5 * sizeof(int));
    if (arr == nullptr) {
        std::cerr << "Memory allocation failed" << std::endl;
        return;
    }

    for (int i = 0; i < 5; ++i) {
//        arr[i] = i *2;
        *(arr + i) = i * 2;
    }

    for (int i = 0; i < 5; ++i) {
        std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
    }

    free(arr);
    arr = nullptr;
}

// 使用realloc进行内存重分配
void test_realloc() {
    int *arr = (int *)malloc(3 * sizeof(int));
    if (arr == nullptr) {
        std::cerr << "Initial malloc failed" << std::endl;
        return;
    }
    for (int i = 0; i < 3; i++) {
        arr[i] = i + 1;
    }

    std::cout << "Initial array: ";
    for (int i = 0; i < 3; i++) {
        std::cout << arr[i] << " ";
    }
    std::cout << std::endl;

    // 重新分配5个整数
    int *temp = (int *)realloc(arr, 5 * sizeof(int));
    if (temp == nullptr) {
        std::cerr << "Realloc failed" << std::endl;
        // 释放原有的内存
        free(arr);
        return;
    }
    arr = temp;
    for (int i = 3; i < 5; i++) {
        arr[i] = (i + 1) * 100;
    }
    for (int i = 0; i < 5; i++) {
        std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
    }
    free(arr);
}


// C++内存管理: 使用new和delete操作符
void test_new_delete() {
    Person *p = new Person("tom", 100);
    std::cout << "Person age:" << p->_age << " , name: " << p->_name << std::endl;
    delete p;

    // 使用new分配数组内存
    auto p_ptr = new Person[2]{
            Person("tom", 100),
            Person("jack", 200)
    };
    for (int i = 0; i < 2; ++i) {
        std::cout << "Person" << " age:" << p_ptr[i]._age << " , name: " << p_ptr[i]._name << std::endl;
    }
    // 使用[]释放new分配的数组
    delete[] p_ptr;

}

// 动态数组

void test_dynamic_array() {
    try {
        DynamicArray arr;
        arr.push_back(10);
        arr.push_back(20);
        arr.push_back(30);
        arr.push_back(40);
        for (size_t i = 0; i < arr.size(); i++) {
//        std::cout << "arr[" << i << "] = " << arr[i] << std::endl;
            std::cout << "arr[" << i << "] = " << arr.get(i) << std::endl;
        }
        std::cout << std::endl;
    } catch(const std::bad_alloc &e) {
        std::cerr << "Memory allocation failed: " << e.what() << std::endl;
    } catch(const std::out_of_range &e) {
        std::cerr << "Index out of range: " << e.what() << std::endl;
    } catch(const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

}

// 简易内存池
void test_memory_pool() {
    try {
        MemoryPool pool(sizeof(Person), 3);
        std::cout << "内存池上可用的内存块: " << pool.size() << std::endl;
        void *mem1 = pool.allocate();
        std::cout << "内存池上可用的内存块: " << pool.size() << std::endl;
        void *mem2 = pool.allocate();
        void *mem3 = pool.allocate();
//        void *mem4 = pool.allocate(); // 内存池无可用内存块，抛出异常

        // 使用 "定位new" 在预分配的内存上构造对象
        auto obj1 = new(mem1) Person("tom", 10);
        auto obj2 = new(mem2) Person("jack", 20);
        auto obj3 = new(mem3) Person("alice", 30);
//        auto obj4 = new(mem4) Person("bob", 40);
        std::cout << "obj1: " << obj1->_name << " " << obj1->_age << std::endl;
        std::cout << "obj2: " << obj2->_name << " " << obj2->_age << std::endl;
        std::cout << "obj3: " << obj3->_name << " " << obj3->_age << std::endl;
//        std::cout << "obj4: " << obj4->_name << " " << obj4->_age << std::endl;

        // 显式调用析构函数，清理内存池上对应的对象
        obj1->~Person();
        obj2->~Person();
        obj3->~Person();
//        std::cout << "内存池上可用的内存块: " << pool.size() << std::endl;
        // 回收内存块
        pool.deallocate(mem1);
        std::cout << "deallocate回收内存，此时内存池上可用的内存块: " << pool.size() << std::endl;

        pool.deallocate(mem2);
        pool.deallocate(mem3);

    } catch(const std::bad_alloc &e){
        std::cout << "Memory allocation failed: " << e.what() << std::endl;
    }
}

int main() {
//    test_malloc_free();
//    test_realloc();
//    test_new_delete();
//    test_dynamic_array();
    test_memory_pool();
    return 0;
}
