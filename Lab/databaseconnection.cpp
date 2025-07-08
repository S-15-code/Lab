#include "databaseconnection.h"

DatabaseConnection::DatabaseConnection(QObject *parent) : QObject(parent)
{
    // 设置SQLite数据库
    m_db = QSqlDatabase::addDatabase("QSQLITE");

    // 确定数据库文件路径
    QString dbPath = QDir::currentPath() + "/lab_equipment.db";
    qDebug() << "Database path:" << dbPath;

    m_db.setDatabaseName(dbPath);
}

DatabaseConnection::~DatabaseConnection()
{
    closeDatabase();
}

bool DatabaseConnection::openDatabase()
{
    if(!m_db.open()) {
        qDebug() << "Error opening database:" << m_db.lastError().text();
        return false;
    }
    qDebug() << "Database opened successfully";
    return true;
}

void DatabaseConnection::closeDatabase()
{
    if(m_db.isOpen()) {
        m_db.close();
        qDebug() << "Database closed";
    }
}

bool DatabaseConnection::isOpen() const
{
    return m_db.isOpen();
}

bool DatabaseConnection::createTables()
{
    if(!m_db.isOpen()) {
        qDebug() << "Database is not open";
        return false;
    }

    QSqlQuery query;

    // 创建设备表
    if(!query.exec("CREATE TABLE IF NOT EXISTS Equipment ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "name TEXT NOT NULL,"
                   "model TEXT,"
                   "serial_number TEXT UNIQUE,"
                   "purchase_date DATE,"
                   "price REAL,"
                   "status INTEGER,"
                   "location TEXT,"
                   "category TEXT,"
                   "warranty_period INTEGER,"
                   "last_maintenance_date DATE,"
                   "next_maintenance_date DATE,"
                   "threshold INTEGER)")) {
        qDebug() << "Error creating Equipment table:" << query.lastError().text();
        return false;
    }

    // 创建用户表
    if(!query.exec("CREATE TABLE IF NOT EXISTS User ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "name TEXT NOT NULL,"
                   "role TEXT,"
                   "department TEXT,"
                   "contact TEXT,"
                   "email TEXT)")) {
        qDebug() << "Error creating User table:" << query.lastError().text();
        return false;
    }

    // 创建借还记录表
    if(!query.exec("CREATE TABLE IF NOT EXISTS BorrowRecord ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "equipment_id INTEGER NOT NULL,"
                   "user_id INTEGER NOT NULL,"
                   "borrow_date DATE NOT NULL,"
                   "expected_return_date DATE,"
                   "actual_return_date DATE,"
                   "borrow_purpose TEXT,"
                   "FOREIGN KEY (equipment_id) REFERENCES Equipment(id),"
                   "FOREIGN KEY (user_id) REFERENCES User(id))")) {
        qDebug() << "Error creating BorrowRecord table:" << query.lastError().text();
        return false;
    }

    // 创建维修记录表
    if(!query.exec("CREATE TABLE IF NOT EXISTS Maintenance ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "equipment_id INTEGER NOT NULL,"
                   "maintenance_date DATE NOT NULL,"
                   "maintenance_type TEXT,"
                   "description TEXT,"
                   "cost REAL,"
                   "technician TEXT,"
                   "completion_status INTEGER,"
                   "next_maintenance_date DATE,"
                   "FOREIGN KEY (equipment_id) REFERENCES Equipment(id))")) {
        qDebug() << "Error creating Maintenance table:" << query.lastError().text();
        return false;
    }

    qDebug() << "All tables created successfully";
    return true;
}


