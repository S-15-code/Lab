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
        db.setDatabaseName("lab_equipment.db");

        if (!db.open()) {
            QMessageBox::critical(nullptr, "Database Error", db.lastError().text());
            return false;
        }

        // 创建设备表
        QSqlQuery query;
        if (!query.exec("CREATE TABLE IF NOT EXISTS equipment ("
                        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                        "name TEXT NOT NULL,"
                        "device_model TEXT,"
                        "serial_number TEXT UNIQUE,"
                        "purchase_date TEXT,"
                        "price REAL,"
                        "quantity INTEGER,"
                        "location TEXT,"
                        "status TEXT,"
                        "threshold INTEGER)")) {
            qDebug() << "Create equipment table error:" << query.lastError();
            return false;
        }

        // 创建借还记录表
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
                        "FOREIGN KEY(equipment_id) REFERENCES equipment(id))")) {
            qDebug() << "Create maintenance table error:" << query.lastError();
            return false;
        }

        // 创建维修与设备联查视图，调整字段顺序
        query.exec("DROP VIEW IF EXISTS v_maintenance_full");
        query.exec(
            "CREATE VIEW IF NOT EXISTS v_maintenance_full AS "
            "SELECT m.id, e.name AS equipment_name, e.serial_number, m.maintenance_date, m.issue_description, m.repair_description, m.cost, m.technician, m.completion_date, m.status "
            "FROM maintenance m "
            "LEFT JOIN equipment e ON m.equipment_id = e.id"
        );

        return true;
    }
};

#endif // DATABASE_H

