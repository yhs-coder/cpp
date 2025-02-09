#include "MemoryPool.h"

MemoryPool::MemoryPool(size_t obj_size, size_t total_size) : _obj_size(obj_size), _total_size(total_size) {
    _pool = ((char *) malloc(_obj_size * total_size));
    if (_pool == nullptr) {
        throw std::bad_alloc{};
    }

    // 初始化_free_list,将内存池的每个内存块的指针，放入栈中进行管理
    for (size_t i = 0; i < total_size; ++i) {
        _free_list.push(_pool + i * _obj_size);
    }
}

MemoryPool::~MemoryPool() {
    std::cout << "MemoryPool::~MemoryPool()" << std::endl;
    free(_pool);
}

void *MemoryPool::allocate() {
    // 栈为空,表示此时为没有可用内存块
    if (_free_list.empty()) {
        throw std::bad_alloc{};
    }
    void *obj = _free_list.top();
    _free_list.pop();
    return obj;
}

void MemoryPool::deallocate(void *ptr) {
    // 将内存块指针重新压入栈中
    _free_list.push(ptr);
}

size_t MemoryPool::size() const {
    return _free_list.size();
}
