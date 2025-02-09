#ifndef MEMORYPOOL_H
#define MEMORYPOOL_H

#include <iostream>
#include <stack>

class MemoryPool {
public:
    MemoryPool(size_t obj_size, size_t total_size);
    ~MemoryPool();
    void* allocate();
    void deallocate(void* ptr);
    size_t size() const;
private:
    size_t _obj_size;               // 每个对象的大小
    size_t _total_size;             // 内存池中对象的个数
    char *_pool;                    // 内存池的起始地址
    std::stack<void *> _free_list;  // 空闲对象栈
};


#endif //MEMORYPOOL_H
