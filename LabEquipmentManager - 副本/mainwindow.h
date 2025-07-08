#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QMenuBar>
#include <QMenu>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox> // 用于 about 对话框
#include "equipmentwidget.h"
#include "borrowreturnwidget.h"
#include "maintenancewidget.h"
#include "inventorywidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void createMenu();
    void createToolBar();
    void createStatusBar();
    void createTabs();

    QTabWidget *tabWidget;
    EquipmentWidget *equipmentWidget;
    BorrowReturnWidget *borrowReturnWidget;
    MaintenanceWidget *maintenanceWidget;
    InventoryWidget *inventoryWidget;
};

#endif // MAINWINDOW_H
