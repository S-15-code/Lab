#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

class Database {
public:
    static bool initialize() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        //"QSQLITE"： Qt 内置的 SQLite 数据库驱动的名称。
        //向 Qt 的数据库管理器注册一个新的数据库连接，并指定该连接使用的数据库驱动为 "QSQLITE"
        db.setDatabaseName("lab_equipment.db");

        if (!db.open()) {
            QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
            return false;
        }

        // 创建设备表
        QSqlQuery query;//用于执行和操作 SQL 语句的一个类
        if (!query.exec("CREATE TABLE IF NOT EXISTS equipment ("//创建设备表，以下为字段说明
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "name TEXT NOT NULL,"
                        "device_model TEXT,"
                        "serial_number TEXT UNIQUE,"
                        "purchase_date TEXT,"
                        "price REAL,"
                        "quantity INTEGER,"
                        "location TEXT,"
                        "status TEXT,"
                        "threshold INTEGER,"
                        "scrap_time TEXT,"
                        "scrap_reason TEXT)")) {
            qDebug() << "Create equipment table error:" << query.lastError();
            //调试输出流，输出到 Qt 的调试控制台
            return false;
        }

        // 创建借还记录表（借还记录的外联图在cpp中以另一种方式实现）
        if (!query.exec("CREATE TABLE IF NOT EXISTS borrow_return ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "equipment_id INTEGER,"
                        "borrower_name TEXT,"
                        "borrow_date TEXT,"
                        "return_date TEXT,"
                        "purpose TEXT,"
                        "FOREIGN KEY(equipment_id) REFERENCES equipment(id))")) {
            qDebug() << "Create borrow_return table error:" << query.lastError();
            return false;
        }

        // 创建维修记录表
        if (!query.exec("CREATE TABLE IF NOT EXISTS maintenance ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "equipment_id INTEGER,"
                        "maintenance_date TEXT,"
                        "issue_description TEXT,"
                        "repair_description TEXT,"
                        "cost REAL,"
                        "technician TEXT,"
                        "completion_date TEXT,"
                        "status TEXT,"
                        "FOREIGN KEY(equipment_id) REFERENCES equipment(id))")) {//只有 equipment 表中存在的 id，才能作为 equipment_id （外键）存在于子表中。
            qDebug() << "Create maintenance table error:" << query.lastError();
            return false;
        }

        // 创建维修与设备联查视图，调整字段顺序
        //视图可以理解为一个“虚拟表”，它本身不存储数据，而是保存了一个查询结果，方便后续像操作表一样操作它。
        query.exec("DROP VIEW IF EXISTS v_maintenance_full");//query.exec（）执行sql语句
        query.exec(
            "CREATE VIEW IF NOT EXISTS v_maintenance_full AS "
            "SELECT m.id, e.name AS equipment_name, e.serial_number, m.maintenance_date, m.issue_description, m.repair_description, m.cost, m.technician, m.completion_date, m.status "
            "FROM maintenance m "//指定主表（左表）为 maintenance，并给它起别名 m
            "LEFT JOIN equipment e ON m.equipment_id = e.id"


                    //通过 LEFT JOIN 关联 equipment（右表），别名为 e
                    //关联条件是：维修记录表中的 equipment_id 字段等于设备表的 id 字段。
        );

        return true;
    }
};

#endif // DATABASE_H

