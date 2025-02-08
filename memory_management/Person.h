//
// Created by yhs on 2025-02-08.
//

#ifndef PERSON_H
#define PERSON_H
#include <string>

class Person {
public:
    Person() : _name("anyone"), _age(1) {}
    Person(std::string name, int age) : _name(name), _age(age) {}
    std::string _name;
    int _age;
};


#endif //PERSON_H
