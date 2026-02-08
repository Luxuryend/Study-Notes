#include <iostream>
#include "StudentManager.h"

using namespace std;

void interface(){
    cout << "======= 学生管理系统 =======\n";
    cout << "1.添加学生信息\n";
    cout << "2.查询学生信息\n";
    cout << "3.显示所有学生\n";
    cout << "4.编辑学生信息\n";
    cout << "5.删除学生信息\n";
    cout << "6.统计学生信息\n";
    cout << "7.保存到文件\n";
    cout << "8.从文件读取\n";
    cout << "9.成绩排序\n";
    cout << "0.退出当前系统\n";
    cout << "===========================\n";
    cout << "请选择您的操作:";
}

int main() {
    int choice;
    while (true){
        interface();

        cin >> choice;

        switch (choice) {
            case 1:
                break;
            case 0:
                cout << "已退出系统";
                return 0;
        }
    }
}
