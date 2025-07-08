#include "mainwindow.h"
#include <QApplication>
#include "databaseconnection.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    // 在主窗口构造函数或应用程序初始化时
    DatabaseConnection dbConn;
    if(dbConn.openDatabase()) {
        if(dbConn.createTables()) {
            qDebug() << "Database initialized successfully";
        } else {
            qDebug() << "Failed to create tables";
        }
    } else {
        qDebug() << "Failed to open database";
    }
    w.show();

    return a.exec();
}
