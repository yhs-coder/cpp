#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <thread>
#include <unordered_map>

/*
 * 行为型模式：主要关注的是对象之间的通信
 * 观察者模式又叫观察者-监听者模式（也叫发布-订阅模式）：该设计模式主要关注的是对象的一对多的关系，也就是多个对象都依赖一个对象，
 * 当该对象的状态发生改变时，其他对象都能接收到对应的通知
 *
 * 比如，基于同一组数据（数据对象），会生成不同的界面显示，有曲线图（对象1），柱状图（对象2），图饼图（对象3）。
 * 当这组数据发生改变时，所有对象（1，2，3）都应该及时收到相应的通知，从而修改图像！
 *
 * Observer1 Observer2 Observer3
 *      data(数据) 数据有更改，应该及时通知相应的观察者，去处理相应的事件！
 * */

// 定义监听者基类
class Listener {
public:
    Listener(std::string name) : name_(name) {}

    // 监听者处理消息事件纯虚函数接口
    virtual void handle_message(int msgid) = 0;

protected:
    std::string name_;
};

// 一个具体的监听者类Listener1
class Listener1 : public Listener {
public:
    Listener1(std::string name) : Listener(name) {}
    // Listener1处理自己感兴趣的事件
    void handle_message(int msgid) override {
        std::cout << "listener: " << name_ << " recv:" << msgid
                  << " msg, handle it now!" << std::endl;
    }
};
// 一个具体的监听者类Listener2
class Listener2 : public Listener {
public:
    Listener2(std::string name) : Listener(name) {}
    ~Listener2() { std::cout << "~Listener2..." << std::endl; }
    // Listener2处理自己感兴趣的事件
    void handle_message(int msgid) override {
        std::cout << "listener: " << name_ << " recv:" << msgid
                  << " msg, handle it now!" << std::endl;
    }
};

// 实现观察者
class Observer {
public:
    /*
	params:
	1. Listener *listener: 具体的监听者
	2. int msgid： 监听者感兴趣的事件
	该函数接口主要用于 监听者向观察者注册感兴趣的事件
	*/
    void register_listener(std::weak_ptr<Listener> listener, int msgid) {
        listener_map_[msgid].push_back(listener);
        /* 同等写法，但上述写法更简洁高效
        auto it = listener_map_.find(msgid);
        if (it != listener_map_.end()) {
            it->second.push_back(listener);
        } else {
            std::list<Listener*> tmp;
            tmp.push_back(listener);
            listener_map_.insert({msgid, std::move(tmp)});
        }*/
    }

    /*
	params:
	1. int msgid：观察到发生的事件id
	该函数接口主要用于观察者观察到事件发生，并转发到对该事件感兴趣
	的监听者
	*/
    void dispatch_message(int msgid) {
        auto it = listener_map_.find(msgid);
        // 发现该事件有监听者注册
        if (it != listener_map_.end()) {
            // 遍历对该事件感兴趣的监听者的链表
            for (auto it1 = it->second.begin(); it1 != it->second.end();) {
                // 智能指针的提升操作，用来判断监听者对象是否存活
                std::shared_ptr<Listener> ps = it1->lock();
                if (ps != nullptr) {
                    ps->handle_message(msgid);
                    it1++;
                } else {
                    std::cout << "删除已经失效的监听者..." << std::endl;
                    // 监听者对象已经析构，从map中删除这样的监听者对象
                    it1 = it->second.erase(it1);
                }
            }
            //            for (auto listener: it->second) {
            //                // 通知所有对该事件感兴趣的监听者区去处理
            //                listener->handle_message(msgid);
            //            }
        }
    }

private:
    // 存储监听者注册的感兴趣的事件
    std::unordered_map<int, std::list<std::weak_ptr<Listener>>> listener_map_;
};

int main() {
    std::shared_ptr<Listener> p1(new Listener1("流量分析模块"));
    std::shared_ptr<Listener> p2(new Listener2("流量统计模块"));
    Observer obser;
    // 监听者p1注册1，2，3事件
    obser.register_listener(p1, 1);
    obser.register_listener(p1, 2);
    obser.register_listener(p1, 3);
    // 监听者p2注册1，3事件
    obser.register_listener(p2, 1);
    obser.register_listener(p2, 2);


    // 模拟事件的发生
    int msgid = -1;
    std::thread t1([&obser]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(3));
            obser.dispatch_message(1);
        }
    });

    std::thread t2([&obser]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(4));
            obser.dispatch_message(2);
        }
    });

    std::thread t3([&obser]() {
        while (true) {
            std::this_thread::sleep_for(std::chrono::seconds(5));
            obser.dispatch_message(3);
        }
    });
    {
        // 监听者出作用域后自动销毁
        std::shared_ptr<Listener> p3(new Listener2("流量捕获模块"));
        obser.register_listener(p3, 3);
        obser.register_listener(p2, 3);
        std::this_thread::sleep_for(std::chrono::seconds(15));
    }

    t1.join();
    t2.join();
    t3.join(); /*
    for (;;) {
        std::cout << "输入事件id: ";
        std::cin >> msgid;
        if (msgid == -1) {
            break;
        }
        obser.dispatch_message(msgid);
    }*/
    return 0;
}