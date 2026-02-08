#pragma once

#include <string>
using namespace std;


class Student{
public:
    Student();      //构造函数
    ~Student();     //析构函数

    string studentID;
    string name;
    char gender;
    int age;
    string className;

    virtual void input() = 0;
    virtual void display() = 0;
    virtual void load(ifstream & in) = 0;
    virtual void save(ofstream & out) const = 0;
    virtual double calculateTotalScore() = 0;
    string getStudentID() const;
    string getName() const {return name;}
};