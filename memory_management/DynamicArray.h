#ifndef DYNAMICARRAY_H
#define DYNAMICARRAY_H
#include <iostream>
#include <cstring>
#include <cstdlib>

// 动态数组管理
/*
 * 使用malloc和 realloc 来手动管理动态数组, 实现可变大小的数组
 * 需要处理内存分配、释放和数据复制
 */

class DynamicArray {
public:
    DynamicArray();
    ~DynamicArray();
    void push_back(int value);
    int get(size_t index);
    size_t size() const;
    int operator[](size_t index);
private:
    void resize(size_t new_capacity);
    size_t _size;     // 数组元素个数
    size_t _capacity; // 数组容量
    int *_data;       // 数组数据指针
};


#endif //DYNAMICARRAY_H
