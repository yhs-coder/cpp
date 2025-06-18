#include <iostream>
#include <list>
#include <string>
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
    Listener1(std::string name) : Listener(name){}
    // Listener1处理自己感兴趣的事件
    void handle_message(int msgid) override {
        std::cout << "listener: " << name_ << " recv:" << msgid
                  << " msg, handle it now!" << std::endl;
    }
};
// 一个具体的监听者类Listener2
class Listener2 : public Listener {
public:
    Listener2(std::string name) : Listener(name){}

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
    void register_listener(Listener *listener, int msgid) {
        listener_map_[msgid].push_back(listener);
    }

    /*
	params:
	1. int msgid：观察到发生的事件id
	该函数接口主要用于观察者观察到事件发生，并转发到对该事件感兴趣
	的监听者
	*/
    void dispatch_message(int msgid) {
        auto it = listener_map_.find(msgid);
        if (it != listener_map_.end()) {
            for (auto listener : it->second) {
                listener->handle_message(msgid);
            }
        }
    }


private:
    // 存储监听者注册的感兴趣的事件
    std::unordered_map<int, std::list<Listener *>> listener_map_;
};

int main() {
    Listener *p1 = new Listener1("流量分析模块");
    Listener *p2 = new Listener2("流量统计模块");

    Observer obser;

    return 0;
}