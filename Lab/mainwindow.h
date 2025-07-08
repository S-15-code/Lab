#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFile>
#include <QDebug>

#include "databaseconnection.h"
#include "borrowmanager.h"
#include "equipmentmanager.h"
#include "maintenancemanager.h"
#include "usermanager.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    // 所有槽函数都在这里声明
    void on_addEquipmentButton_clicked();
    void on_borrowButton_clicked();
    void on_addMaintenanceButton_clicked();

    void loadEquipmentData();
    void loadUserData();
    void checkInventoryThresholds();
    void addTestData();

private:
    Ui::MainWindow *ui;
    DatabaseConnection m_dbConn;
    EquipmentManager* m_equipmentManager;
    UserManager* m_userManager;
    BorrowManager* m_borrowManager;
    MaintenanceManager* m_maintenanceManager;
};

#endif // MAINWINDOW_H
