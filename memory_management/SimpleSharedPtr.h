#ifndef SimpleSharedPtr_H
#define SimpleSharedPtr_H


// 模拟实现一个简易版本的share_ptr,不考虑线程安全

// 控制块，里面维护着管理资源的引用计数
struct ControlBlock {
    ControlBlock() : ref_count(1) {}
    int ref_count;  // 引用计数
};

template <class T>
class SimpleSharedPtr {
public:
    SimpleSharedPtr() : _ptr(nullptr), _control(nullptr){}
    explicit SimpleSharedPtr(T *ptr) : _ptr(ptr) {
        if (ptr) {
            _control = new ControlBlock();
        } else {
            _control = nullptr;
        }
    }

    ~SimpleSharedPtr() {
        if (_ptr)
            release();

    }
    // 拷贝构造函数,SimpleSharedPtr s2(s1)
    SimpleSharedPtr(const SimpleSharedPtr &s) : _ptr(s._ptr), _control(s._control) {
        // s的控制块不为空增加引用计数
        if (_control) {
            _control->ref_count++;
        }
    }

    // 拷贝赋值函数，s2 = s1
    SimpleSharedPtr& operator = (const SimpleSharedPtr &s) {
        // 检测自赋值
        if (this != &s) {
            // 释放自身的资源
            release();
            // 拷贝s的指针和控制块
            _ptr = s._ptr;
            _control = s._control;
            if (_control) {
                // 增加引用计数
                _control->ref_count++;
            }
        }
        return *this;
    }

    // 移动构造函数 SimpleSharedPtr s2(std::move(s1))
    SimpleSharedPtr(SimpleSharedPtr &&other) noexcept : _ptr(other._ptr), _control(other._control) {
        // 将other的指针和控制块置空
        other._ptr = nullptr;
        other._control = nullptr;
    }

    // 移动赋值操作符 s2 = std::move(s1)
    SimpleSharedPtr& operator = (SimpleSharedPtr &&other) noexcept {
        // 检测自赋值
        if (this != &other) {
            // 释放当前资源
            release();
            // 拷贝other的指针和控制块
            _ptr = other._ptr;
            _control = other._control;
            // 将other的指针和控制块置空
            other._ptr = nullptr;
            other._control = nullptr;
        }
        return *this;
    }

    // 模拟原生指针的行为

    // 解引用*操作符
    T& operator * () const {
        return *_ptr;
    }
    // ->箭头操作符
    T* operator -> () const{
        return _ptr;
    }

    // 获取引用计数
    int use_count() const {
        return _control ? _control->ref_count : 0;
    }

    // 获取原生指针
    T* get() const {
        return _ptr;
    }

    // 重置指针
    void reset(T *ptr=nullptr) {
        release();
        _ptr = ptr;
        if (_ptr) {
            _control = new ControlBlock();
        } else {
            _control = nullptr;
        }
    }
private:
    // 辅助函数，释放资源
    void  release() {
//        if (_control) {
//            --_control->ref_count;
//            if (_control->ref_count == 0) {
//                delete _ptr;
//                delete _control;
//            }
//        }
        // 减少管理资源的引用计数
        if (_control && --_control->ref_count == 0) {
            delete _ptr;
            delete _control;
            _ptr = nullptr;
            _control = nullptr;
        }
    }

    T *_ptr;                 // 指向资源/实际管理对象的指针
    ControlBlock *_control;  // 指向控制块的指针

};


#endif //SimpleSharedPtr_H
