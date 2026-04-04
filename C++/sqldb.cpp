#include "widget.h"
#include "./ui_widget.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    m_db = QSqlDatabase::addDatabase("QSQLITE");    // 使用 SQLite
    m_db.setDatabaseName(m_db_name);                // 设置数据库名

    if (m_db.open()){
        QString sql = QString(R"(
            CREATE TABLE IF NOT EXISTS %1 (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT,
            age INTEGER,
            email TEXT,
            latest_time DATETIME DEFAULT CURRENT_TIMESTAMP)
        )").arg(m_table_name);

        QSqlQuery query;

        if (!query.exec(sql)) {
            qDebug() << "建表失败:" << query.lastError().text();
        } else {
            qDebug() << "数据库初始化成功或表已存在";
        }
    }
    else{
        qDebug() << "Error: Failed to connect database." << m_db.lastError().text();
    }

    m_model = new QSqlTableModel(this, m_db);   // 1. 创建模型，绑定当前的数据库
    m_model->setTable(m_table_name);            // 2. 设置要显示的表名

    // 3. 设置策略：手动提交（修改界面后不立刻写回数据库，直到点保存）
    // 或者用 OnFieldChange（改完即存）
    m_model->setEditStrategy(QSqlTableModel::OnFieldChange);

    // 4. 设置表头文字（可选）
    m_model->setHeaderData(0, Qt::Horizontal, "编号");
    m_model->setHeaderData(1, Qt::Horizontal, "姓名");
    m_model->setHeaderData(2, Qt::Horizontal, "年龄");

    // m_model->removeColumn(4);

    // 5. 非常重要：执行查询，拉取数据
    m_model->select();

    // 6. 把模型绑定到界面上的 TableView
    ui->tableView->setModel(m_model);
    ui->tableView->setSortingEnabled(true);     // 强大的排序功能
}

Widget::~Widget()
{
    m_db.close();   // 释放连接 释放文件句柄
    delete ui;
}

// 数据库操作
bool Widget::insert_data(const QString &name, int age, const QString &email)
{
    QSqlQuery query;
    query.prepare("INSERT INTO " + m_table_name + " (name, age, email) VALUES ( :name, :age, :email)");
    query.bindValue(":name", name);
    query.bindValue(":age", age);
    query.bindValue(":email", email);

    if (query.exec()){
        qDebug() << "insert success" << name;
        return true;
    }
    else {
        qDebug() << "insert failed";
        return false;
    }
}


bool Widget::update_data(int id, const QString &name, int age, const QString &email)
{
    QSqlQuery query;
    query.prepare("UPDATE " + m_table_name + " SET name = :name, age = :age, email = :email WHERE id = :id");
    query.bindValue(":name", name);
    query.bindValue(":age", age);
    query.bindValue(":email", email);
    query.bindValue(":id", id);

    if (query.exec()){
        qDebug() << "update success";
        return true;
    }
    else {
        qDebug() << "update failed";
        return false;
    }
}


bool Widget::delete_data(int id)
{
    QSqlQuery query;
    query.prepare("DELETE FROM " + m_table_name + " WHERE id = :id");
    query.bindValue(":id", id);

    if (query.exec()){
        qDebug() << "delete success";
        return true;
    }
    else {
        qDebug() << "delete failed";
        return false;
    }
}


QVariantList Widget::select_all_data()
{
    QVariantList allData;
    QSqlQuery query;
    QString sql = "SELECT * FROM " + m_table_name;

    if (!query.exec(sql)) {
        qDebug() << "Select error:" << query.lastError().text();
        return allData; // 失败则返回空列表
    }

    // 循环提取每一行
    while (query.next()) {
        QVariantList row; // 小列表：存放当前这一行的各列数据

        // 注意：value() 里的参数是数据库的字段名
        row << query.value("id");
        row << query.value("name");
        row << query.value("age");
        row << query.value("email");
        row << query.value("latest_time");

        allData << QVariant(row); // 将这一行包装成 QVariant 塞进大列表
    }
    return allData;
}


QVariantList Widget::select_sorted_data(const QString &column_name, bool is_ascending)
{
    QVariantList allData;
    QSqlQuery query;

    // 1. 确定升降序关键字
    QString order = is_ascending ? "ASC" : "DESC";

    // 2. 拼接 SQL 语句
    // 注意：表名、列名、排序关键字都必须用 + 拼接，因为它们不是“数据值”
    QString sql = "SELECT * FROM " + m_table_name
                  + " ORDER BY " + column_name + " " + order;

    if (!query.exec(sql)) {
        qDebug() << "Sort Select Error:" << query.lastError().text();
        return allData;
    }

    // 3. 循环解析结果集
    while (query.next()) {
        QVariantList row;
        row << query.value("id");
        row << query.value("name");
        row << query.value("age");
        row << query.value("email");
        row << query.value("latest_time");

        allData << QVariant(row);
    }

    return allData;
}


// ui操作
void Widget::on_pushButton_insert_clicked()
{
    QString name = ui->lineEdit_name->text();
    int age = ui->lineEdit_age->text().toInt();
    QString email = ui->lineEdit_email->text();

    insert_data(name,age,email);
    m_model->select();
}


void Widget::on_pushButton_delete_clicked()
{
    QModelIndex currentIndex = ui->tableView->currentIndex();

    if (!currentIndex.isValid()){
        qDebug() << "没选择删除数据";
    }

    int row = currentIndex.row();
    int id = m_model->index(row, 0).data().toInt();
    delete_data(id);
    m_model->select();
}

