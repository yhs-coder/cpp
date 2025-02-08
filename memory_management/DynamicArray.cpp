//
// Created by yhs on 2025-02-08.
//

#include "DynamicArray.h"

DynamicArray::DynamicArray()
        : _capacity(2)
        , _size(0)
        , _data((int*)malloc(_capacity * sizeof(int)))
{
    if (_data == nullptr) {
        std::cerr << "malloc failed" << std::endl;
        throw std::bad_alloc{};
    }
}

DynamicArray::~DynamicArray() {
    free(_data);
    _data = nullptr;
}

void DynamicArray::push_back(int value) {
    if (_size == _capacity) {
        // 扩容
        resize(_capacity * 2);
    }
    _data[_size++] = value;
}

int DynamicArray::get(size_t index) {
    if (index >= _size) {
        throw std::out_of_range{"index out of range"};
    }
    return _data[index];
}

size_t DynamicArray::size() const {
    return _size;
}

int DynamicArray::operator[](size_t index) {
    return get(index);

    /*if (index >= _size) {
        throw std::out_of_range{"index out of range"};
    }
    return _data[index];*/
}


void DynamicArray::resize(size_t new_capacity) {
    int *temp = (int*)realloc(_data, new_capacity * sizeof(int));
    if (temp == nullptr) {
        std::cerr << "realloc failed" << std::endl;
        throw std::bad_alloc{};
    }
    _data = temp;
    _capacity = new_capacity;
    std::cout << "Expand capacity : " << _capacity  << std::endl;
}
