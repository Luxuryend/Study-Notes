#pragma once
#include <vector>
#include <iostream>
#include "Student.h"

class StudentManager {
public:
    StudentManager();
    ~StudentManager();

    void addStudent();
private:
    vector<Student*> students;
};
