#pragma once
#include "Student.h"
#include <iostream>
#include <fstream>

using namespace std;

class PrimaryStudent:public Student{
public:
    PrimaryStudent();
    ~PrimaryStudent();
    int mathScore;
    int chineseScore;
    int englishScore;

    void input() override;
    void display() override;
    void load(ifstream & in) override;
    void save(ofstream & out) const override;
    double calculateTotalScore() override;
};