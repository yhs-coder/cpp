#include <iostream>
#include <memory>
/*
 * 装饰器模式 Decorator pattern
 * 在类原有功能的基础上，为了增强现有类的功能，通过实现子类的方式，重写接口，
 * 虽然可以完成功能扩展，但是需要实现太多的子类.比如三种汽车，需要实现同样功能，就嘚实现三个子类！如果实现的功能越多，实现的子类的个数更大！
 * 遇到上述情况需要使用装饰器模式来解决！
 * */

// 抽象基类
class Car {
public:
    virtual void show() = 0;
    virtual ~Car() { std::cout << "~Car()" << std::endl; }
};

// 三个实体的汽车类
class Bmw : public Car {
public:
    ~Bmw() {
        std::cout << "~Bmw()" << std::endl;
    }
    void show() override {
        std::cout << "这是一辆宝马汽车，配置有：基类配置 ";
    }
};
class Audi : public Car {
public:
    void show() override {
        std::cout << "这是一辆奥迪汽车，配置有：基类配置 ";
    }
};
class Benz : public Car {
public:
    void show() override {
        std::cout << "这是一辆奔驰汽车，配置有：基类配置 ";
    }
};

// 装饰器抽象类
class Decorator : public Car {
public:
    Decorator(std::unique_ptr<Car> car) : car_(std::move(car)) {}

    void show() override {
        // 原有功能
        car_->show();
    }
protected:
    std::unique_ptr<Car> car_;
};

//  具体装饰器类：自动驾驶装饰器
class AutoPilotDecorator : public Decorator {
public:
    AutoPilotDecorator(std::unique_ptr<Car> car) : Decorator(std::move(car)) {}

    void show() override {
        // 原有功能
        Decorator::show();
        // 新功能
        std::cout << "自动驾驶 ";
    }
};

class AutoBrakeDecorator : public Decorator {
public:
    AutoBrakeDecorator(std::unique_ptr<Car> car) : Decorator(std::move(car)) {}

    void show() override {
        // 原有功能
        Decorator::show();
        // 新功能
        std::cout << "自动刹车 " << std::endl;
        std::cout << "----------------" << std::endl;
    }
};


int main() {
    auto car = std::make_unique<Bmw>();
    car->show();
    std::cout << std::endl;

    // 添加新功能
    auto auto_pilot_car = std::make_unique<AutoPilotDecorator>(std::move(car));
    auto_pilot_car->show();
    std::cout << std::endl;

    // 添加新功能
    auto auto_brake_car = std::make_unique<AutoBrakeDecorator>(std::move(auto_pilot_car));
    auto_brake_car->show();
    return 0;
}

#if 0
// 如果想要添加新功能，一般方法是重写子类，但是同一功能得实现三个子类！
class NewMethod : public Benz {
public:
    void show() {
        Benz::show();
        std::cout << "增加新功能1" << std::endl;;
    }
};

// 如果想要添加新功能，就要重写子类，如：
class NewMethod2 : public Bmw {
public:
    void show() {
        Bmw::show();
        std::cout << "增加新功能1" << std::endl;;
    }
};
class NewMethod3 : public Audi {
public:
    void show() {
        Audi::show();
        std::cout << "增加新功能1 " << std::endl;
    }
};


int main() {
    NewMethod* p1 = new NewMethod();
    NewMethod2* p2 = new NewMethod2();
    NewMethod3* p3 = new NewMethod3();
    p1->show();
    p2->show();
    p3->show();
    delete p2;
    return 0;
}
#endif