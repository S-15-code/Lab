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
    if (equipmentWidget->model->isDirty()) {//检查模型自上次提交后是（1）否（0）有未保存的修改
        equipmentWidget->model->submitAll();
    }
}

void MainWindow::createMenu() {
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");//获取主窗口的菜单栏对象-》在菜单栏中添加一个新菜单
    fileMenu->addAction("退出", this, &QMainWindow::close);

    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    helpMenu->addAction("关于", []() {
        QMessageBox::about(nullptr, "关于", "实验室设备管理系统 v1.0");
    });
    //Lambda表达式 []() { ... }定义该动作的响应逻辑
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
    tabWidget = new QTabWidget(this);//标签页容器
    setCentralWidget(tabWidget);

    equipmentWidget = new EquipmentWidget(this);
    borrowReturnWidget = new BorrowReturnWidget(this);
    maintenanceWidget = new MaintenanceWidget(this);
    inventoryWidget = new InventoryWidget(this);

    tabWidget->addTab(equipmentWidget, "设备管理");
    tabWidget->addTab(borrowReturnWidget, "借还记录");//添加借还记录组件到标签页
    tabWidget->addTab(maintenanceWidget, "维修跟踪");
    tabWidget->addTab(inventoryWidget, "库存预警");
}
