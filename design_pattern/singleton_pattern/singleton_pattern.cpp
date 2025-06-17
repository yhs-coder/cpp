#include <iostream>
#include <mutex>
/*
 * 单例模式: 一个类不管创建多少次对象，永远只能得到该类型一个对象的实例
 * 常用的场景：日志模块、数据库模块
 *
 * 如何设计一个单例类：
 *  1.构造函数、拷贝构造、赋值重载私有化，限制对象创建个数
 *  2.定义一个唯一的类的实例对象
 *  3. 提供获取类的唯一实例对象的接口方法
 *
 * 全局静态对象（定义在所有函数之外的静态对象）在程序启动时被构造，在程序结束时被析构。
 * 局部静态对象（定义在函数内部的静态对象）在第一次被访问时被构造，在程序结束时被析构。
 *
 * 单例模式又分：
 *  - 懒汉式单例模式：唯一的实例对象，直到第一次获取它的时候才产生
 *    饿汉单例模式中，单例对象定义成了一个static静态对象，它是在程序启动时，main函数运行之前就初始化好了，不存在线程安全问题，可以放心的在多线程环境中使用。
 *  - 饿汉式单例模式：程序启动时就实例化了该对象，并没有推迟到第一次使用该对象时再进行实例化
 * */

// 精简的线程安全的懒汉单例模式
/*
 * 对于static静态局部变量的初始化，编译器会自动对它的初始化进行加锁和解锁控制，使静态局部变量的初始化成为线程安全的操作，
 * 不用担心多个线程都会初始化静态局部变量，因此下面的懒汉单例模式是线程安全的单例模式！
 * */
class Singleton
{
public:
    static Singleton* get_instance()
    {
        static Singleton single; // 懒汉式单例模式，定义唯一的对象实例
        return &single;
    }
private:
    Singleton() { std::cout << "Singleton()" << std::endl; }
    ~Singleton() { std::cout << "~Singleton()" << std::endl;}
    Singleton(const Singleton&);
};

int main()
{
    Singleton *p1 = Singleton::get_instance();
    Singleton *p2 = Singleton::get_instance();
    Singleton *p3 = Singleton::get_instance();
    std::cout << "p1: " << p1 << std::endl;
    std::cout << "p2: " << p2 << std::endl;
    std::cout << "p3: " << p3 << std::endl;
    return 0;
}

#if 0
std::mutex mtx;
// 线程安全的懒汉式单例模式
class Singleton {
public:
    // 获取类的唯一实例对象的方法
    // 注意：该函数是不可重入函数
    static Singleton *GetInstance() {
        // 第一次获取实例对象时，才创建实例对象！
        // 在构造函数中执行一些初始化操作
        // std::lock_guard<std::mutex> guard(mtx); 锁的颗粒度太大，单线程下不适用
        if (instance_ == nullptr) {
            // 锁 + 双重判断 （双重检验锁）
            std::lock_guard<std::mutex> guard(mtx);
            // 需要再次判断，可能别的线程已经创建实例对象了！
            if (instance_ == nullptr) {
                instance_ = new Singleton();
            }
        }
        return instance_;
    }

private:
    // 定义一个唯一的类的实例对象
    // 使用volatile关键字防止指令重排序，保证内存可见性。
    static Singleton *volatile instance_;
    // 构造函数私有化
    Singleton() {
        // 加载配置信息、读取磁盘文件、访问数据库，这些操作应该是第一次获取对象时才执行
        // 使用饿汉式单例模式，执行这些操作会导致软件启动时初始化时间加长
    }
    ~Singleton() { std::cout << "~Singleton()..." << std::endl; }

    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    // 定义一个嵌套类，在该类的析构函数中，自动释放外层类的资源
    class Release {
    public:
        ~Release() { delete instance_; }
    };
    // 利用该静态对象在程序结束时自动析构的特点，来释放外层类的对象资源
    static Release release_;
};
Singleton::Release Singleton::release_;
Singleton *volatile Singleton::instance_ = nullptr;

int main() {
    Singleton *p1 = Singleton::GetInstance();
    Singleton *p2 = Singleton::GetInstance();
    Singleton *p3 = Singleton::GetInstance();
    std::cout << "p1: " << p1 << std::endl;
    std::cout << "p2: " << p2 << std::endl;
    std::cout << "p3: " << p3 << std::endl;
    return 0;
}
#endif

#if 0
// 懒汉式单例模式
// 自动释放资源
class Singleton {
public:
    // 获取类的唯一实例对象的方法
    // 注意：该函数是不可重入函数
    static Singleton *GetInstance() {
        // 第一次获取实例对象时，才创建实例对象！
        // 需要再次判断，可能别的线程已经创建实例对象了！
        if (instance_ == nullptr) {
            instance_ = new Singleton();
        }
        return instance_;
    }

private:
    // 定义一个唯一的类的实例对象
    static Singleton *instance_;
    // 构造函数私有化
    Singleton() {
        std::cout << "Singleton()..." << std::endl;
        // 加载配置信息、读取磁盘文件、访问数据库，这些操作应该是第一次获取对象时才执行
        // 使用饿汉式单例模式，执行这些操作会导致软件启动时初始化时间加长
    }
    ~Singleton() { std::cout << "~Singleton()..." << std::endl; }
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;

    // 定义一个嵌套类，在该类的析构函数中，自动释放外层类的资源
    class Release {
    public:
        ~Release() { delete instance_; }
    };
    // 利用该静态对象在程序结束时自动析构的特点，来释放外层类的对象资源
    static Release release_;
};

Singleton *Singleton::instance_ = nullptr;
Singleton::Release Singleton::release_;

int main() {
    Singleton *p1 = Singleton::GetInstance();
    Singleton *p2 = Singleton::GetInstance();
    Singleton *p3 = Singleton::GetInstance();
    std::cout << "p1: " << p1 << std::endl;
    std::cout << "p2: " << p2 << std::endl;
    std::cout << "p3: " << p3 << std::endl;
    return 0;
}
#endif



#if 0
// 饿汉式单例模式，是线程安全的
// 静态的成员变量在数据段，数据段在程序启动后，在main函数之前就初始化好了。线程函数启动
class Singleton {
public:
    // 获取类的唯一实例对象的方法
    static Singleton *GetInstance() {
        return &instance_;
    }

private:
    // 定义一个唯一的类的实例对象
    static Singleton instance_;
    // 构造函数私有化
    Singleton() {
        // 加载配置信息、读取磁盘文件、访问数据库，这些操作应该是第一次获取对象时才执行
        // 使用饿汉式单例模式，执行这些操作会导致软件启动时初始化时间加长
    }
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
};
Singleton Singleton::instance_;

int main() {

    // 测试单例类
    Singleton *p1 = Singleton::GetInstance();
    Singleton *p2 = Singleton::GetInstance();
    Singleton *p3 = Singleton::GetInstance();
    std::cout << "p1: " << p1 << std::endl;
    std::cout << "p2: " << p2 << std::endl;
    std::cout << "p3: " << p3 << std::endl;
    return 0;
}
#endif
