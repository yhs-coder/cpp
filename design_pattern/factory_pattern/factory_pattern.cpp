#include <iostream>
#include <memory>

/*
 * 工厂模式
 * 1. 简单工厂 Simple Factory
 *      把对象的创建封装在一个接口函数里面，通过传入不同的标识，返回创建的对象。
 *      客户不用自己负责new对象，不需要了解对象创建的详细过程。
 *      但是提供创建对象实例的接口函数不闭合，不能对修改关闭。
 * 2. 工厂方法 Factory Method
 *      Factory基类，提供一个纯虚函数（用于创建产品），定义派生类（生产具体产品的工厂），负责创建对应的产品。
 *      可以做到不同的产品，在不同的工厂里面创建，能够对线程工厂以及产品的修改关闭。
 *      但实际上，很多产品是有关联关系的，属于一个产品簇，不应该放在不同的工厂里面去创建，这样一是不符合实际
 *      的产品对象创建逻辑，二是需要创建的工厂类太多，不好维护。
 * 3. 抽象工厂 Abstract Factory
 *      抽象工厂，就是将有关联关系的，属于一个产品簇的所有产品创建的接口函数，放在一个抽象工厂AbstractFactory里面。
 *      在派生类（具体产品的工厂）中，负责创建该产品簇里面的所有产品。
 *
 * 使用工厂模式，主要是封装了对象的创建
 * 比如买车，用户向工厂申请买一辆车，工厂返回相应的对象，无需关注创建的过程。
 * */

// 系列产品1
class Car {
public:
    Car(std::string name) : name_(name) {}
    virtual void show() = 0;

protected:
    std::string name_;
};

class Tesla : public Car {
public:
    Tesla(std::string name) : Car(name) {}
    void show() override {
        std::cout << "购买一辆特斯拉汽车 " << name_ << std::endl;
    }
};

class Audi : public Car {
public:
    Audi(std::string name) : Car(name) {}
    void show() override {
        std::cout << "购买一辆奥迪汽车 " << name_ << std::endl;
    }
};

// 系列产品2
class Light {
public:
    virtual void show() = 0;
};
class TeslaLight : public Light {
public:
    void show() override {
        std::cout << "Tesla Light..." << std::endl;
    }
};

class AudiLight : public Light {
public:
    void show() override {
        std::cout << "Audi Light..." << std::endl;
    }
};


/*******************************************************************************************
工厂方法： 定义一个抽象基类，提供一个纯虚函数（工厂方法），用于创建产品。定义派生类（生产具体产品的工厂），让每个工厂创建相对应的产品
当需要创建新的产品时，只需要从Factory类派生新的工厂类，重写接口返回新创建的产品。
达到对扩展开放，对修改封闭的设计原则。

工厂方法 -> 抽象工厂 （将有关联关系的产品簇，提供产品对象的统一创建）
*******************************************************************************************/
class AbstractFactory {
public:
    // 工厂方法，创建汽车
    virtual std::unique_ptr<Car> create_car(std::string name) = 0;
    // 工厂方法，创建汽车关联产品，车灯
    virtual std::unique_ptr<Light> create_light() = 0;
};

// 具体的工厂类创建具体的产品

class TeslaFactory : public AbstractFactory {
public:
    std::unique_ptr<Car> create_car(std::string name) override {
        return std::make_unique<Tesla>(name);
    }

    std::unique_ptr<Light>create_light() override {
        return std::make_unique<TeslaLight>();
    }
};

class AudiFactory : public AbstractFactory {
public:
    std::unique_ptr<Car>create_car(std::string name) override {
        return std::make_unique<Audi>(name);
    }
    std::unique_ptr<Light>create_light() override {
        return std::make_unique<AudiLight>();
    }
};

int main() {
    std::unique_ptr<AbstractFactory> tesla_factory(new TeslaFactory());
    std::unique_ptr<AbstractFactory> audi_factory(new AudiFactory());
    std::unique_ptr<Car> c1(tesla_factory->create_car("tesla"));
    std::unique_ptr<Car> c2(audi_factory->create_car("A8"));
    std::unique_ptr<Light> l1(tesla_factory->create_light());
    std::unique_ptr<Light> l2(tesla_factory->create_light());
    c1->show();
    l1->show();
    c2->show();
    l2->show();
    return 0;
}

#if 0
/*******************************************************************************************
简单工厂
简单工厂的缺点：将所有产品的创建都放在同一个工厂上，当需要删除或者新增某个对象时，需要对接口进行频繁修改
而接口的设计并不封闭，不符合软件设计的“开-闭”原则（对扩展开放，对修改关闭）
*******************************************************************************************/
enum CarType {
    TESLA,
    AUDI
};
class SimpleFactory{
public:
    std::unique_ptr<Car> create_car(CarType ct) {
        switch (ct) {
            case TESLA:
                return std::make_unique<Tesla>("T");
            case AUDI:
                return std::make_unique<Audi>("A6");
            default:
                std::cerr << "传入工厂的参数不正确" << std::endl;
                return nullptr;
        }
    }
};

int main()
{
    // 测试简单工厂
    std::unique_ptr<SimpleFactory> factory(new SimpleFactory());
    std::unique_ptr<Car> c1(factory->create_car(TESLA));
    std::unique_ptr<Car> c2(factory->create_car(AUDI));
    c1->show();
    c2->show();
    return 0;
}
#endif