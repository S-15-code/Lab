#ifndef DATABASECONNECTION_H
#define DATABASECONNECTION_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QDir>

class DatabaseConnection : public QObject
{
    Q_OBJECT
public:
    explicit DatabaseConnection(QObject *parent = nullptr);
    ~DatabaseConnection();

    bool openDatabase();
    void closeDatabase();
    bool createTables();
    bool isOpen() const;

    QSqlDatabase& database() { return m_db; }

    bool backupDatabase(const QString& backupPath);
    bool restoreDatabase(const QString& backupPath);

private:
    QSqlDatabase m_db;
};

#endif // DATABASECONNECTION_H
