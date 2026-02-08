#include "PrimaryStudent.h"

void PrimaryStudent::input() {
    cout << "请输入学号:";
    cin >> studentID;
    cout << "请输入姓名:";
    cin >> name;
    cout << "请输入性别:";
    cin >> gender;
    cout << "请输入班级: ";
    cin >> className;
    cout << "请输入英语成绩: ";
    cin >> englishScore;
    cout << "请输入数学成绩: ";
    cin >> mathScore;
    cout << "请输入语文成绩: ";
    cin >> chineseScore;
}

void PrimaryStudent::display(){
    cout << "学号:" << studentID << endl;
    cout << "姓名:" << name << endl;
    cout << "性别:" << gender << endl;
    cout << "年龄:" << age << endl;
    cout << "班级:" << className << endl;
    cout << "语文成绩:" << chineseScore << endl;
    cout << "数学成绩:" << mathScore << endl;
    cout << "英语成绩:" << englishScore << endl;
    cout << "总分:" << calculateTotalScore() << endl;
}

void PrimaryStudent::load(ifstream &in) {
    // string类型使用 getline函数，数值类型使用read函数
    char buffer[256];
    in.getline(buffer, 256, '\0');

    in.getline(buffer, 256, '\0');
    studentID = buffer;

    in.getline(buffer, 256, '\0');
    name = buffer;

    in.read(&gender, sizeof(gender));

    in.read(reinterpret_cast<char *>(&age), sizeof(age));

    in.getline(buffer, 256, '\0');
    className = buffer;

    in.read(reinterpret_cast<char *>(&mathScore), sizeof(mathScore));
    in.read(reinterpret_cast<char *>(&chineseScore), sizeof(chineseScore));
    in.read(reinterpret_cast<char *>(&englishScore), sizeof(englishScore));
}

void PrimaryStudent::save(ofstream & out) const {
    // 二进制存储
    string type = "PrimaryStudent";
    out.write(type.c_str(), type.size() + 1);

    // 写入基本信息
    out.write(studentID.c_str(), studentID.size() + 1);
    out.write(name.c_str(), name.size() + 1);
    out.write(& gender, sizeof(char));
    out.write(reinterpret_cast<const char *>(&age), sizeof(age));
    out.write(className.c_str(), className.size() + 1);

    // 写入成绩
    out.write(reinterpret_cast<const char *>(&mathScore), sizeof(mathScore));
    out.write(reinterpret_cast<const char *>(&chineseScore), sizeof(chineseScore));
    out.write(reinterpret_cast<const char *>(&englishScore), sizeof(englishScore));
}

double PrimaryStudent::calculateTotalScore() {
    return mathScore + chineseScore + englishScore;
}