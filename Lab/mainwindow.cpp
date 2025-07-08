#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_equipmentManager(nullptr)
    , m_userManager(nullptr)
    , m_borrowManager(nullptr)
    , m_maintenanceManager(nullptr)
{
    ui->setupUi(this);

    // 初始化数据库连接
    if(!m_dbConn.openDatabase()) {
        qDebug() << "Failed to open database";
        return;
    }

    if(!m_dbConn.createTables()) {
        qDebug() << "Failed to create tables";
        return;
    }

    // 初始化管理器
    m_equipmentManager = new EquipmentManager(m_dbConn.database(), this);
    m_userManager = new UserManager(m_dbConn.database(), this);
    m_borrowManager = new BorrowManager(m_dbConn.database(), this);
    m_maintenanceManager = new MaintenanceManager(m_dbConn.database(), this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_equipmentManager;
    delete m_userManager;
    delete m_borrowManager;
    delete m_maintenanceManager;
}

void MainWindow::loadEquipmentData()
{
    if(!m_equipmentManager) return;

    // 这里添加加载设备数据的实现
    // 例如: auto equipment = m_equipmentManager->getAllEquipment();
}

void MainWindow::loadUserData()
{
    if(!m_userManager) return;

    // 这里添加加载用户数据的实现
}

void MainWindow::on_addEquipmentButton_clicked()
{
    if(!m_equipmentManager) return;

    // 这里添加设备添加功能的实现
}

void MainWindow::on_borrowButton_clicked()
{
    if(!m_borrowManager) return;

    // 这里添加借出设备功能的实现
}

void MainWindow::on_addMaintenanceButton_clicked()
{
    if(!m_maintenanceManager) return;

    // 这里添加维修记录功能的实现
}

void MainWindow::checkInventoryThresholds()
{
    if(!m_equipmentManager) return;

    // 这里添加库存检查功能的实现
}

void MainWindow::addTestData()
{
    // 这里添加测试数据的功能实现
}
