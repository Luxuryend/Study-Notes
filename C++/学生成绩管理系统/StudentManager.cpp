#include "StudentManager.h"

StudentManager::StudentManager() {

}

StudentManager::~StudentManager(){
    if (students.size() == 0){
        return;
    }
    for (Student* student : students) {
        if (student != nullptr){
            delete student;
        }
    }
}

void StudentManager::addStudent() {
    cout << "=====添加学生信息=====\n";
}
