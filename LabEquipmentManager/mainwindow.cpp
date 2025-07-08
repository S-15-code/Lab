#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("实验室设备管理系统");
    resize(1000, 600);

    createMenu();
    createToolBar();
    createStatusBar();
    createTabs();
}

MainWindow::~MainWindow() {
    // 确保所有未提交的更改被保存
    if (equipmentWidget->model->isDirty()) {
        equipmentWidget->model->submitAll();
    }
}

void MainWindow::createMenu() {
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    fileMenu->addAction("退出", this, &QMainWindow::close);

    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    helpMenu->addAction("关于", []() {
        QMessageBox::about(nullptr, "关于", "实验室设备管理系统 v1.0");
    });
}

void MainWindow::createToolBar() {
    QToolBar *toolBar = addToolBar("工具");
    toolBar->addAction("刷新", this, [this]() {
        equipmentWidget->refresh();
        borrowReturnWidget->refresh();
        maintenanceWidget->refresh();
        inventoryWidget->refresh();
    });
}

void MainWindow::createStatusBar() {
    statusBar()->showMessage("就绪");
}

void MainWindow::createTabs() {
    tabWidget = new QTabWidget(this);
    setCentralWidget(tabWidget);

    equipmentWidget = new EquipmentWidget(this);
    borrowReturnWidget = new BorrowReturnWidget(this);
    maintenanceWidget = new MaintenanceWidget(this);
    inventoryWidget = new InventoryWidget(this);

    tabWidget->addTab(equipmentWidget, "设备管理");
    tabWidget->addTab(borrowReturnWidget, "借还记录");
    tabWidget->addTab(maintenanceWidget, "维修跟踪");
    tabWidget->addTab(inventoryWidget, "库存预警");
}
