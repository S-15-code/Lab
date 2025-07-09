#include "inventorywidget.h"
#include "ui_inventorywidget.h"

InventoryWidget::InventoryWidget(QWidget *parent) : 
    QWidget(parent),
    ui(new Ui::InventoryWidget)
{
    ui->setupUi(this);

    // 创建表格视图
    model = new QSqlQueryModel(this);
    ui->tableView->setModel(model);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // 连接信号和槽
    connect(ui->checkButton, &QPushButton::clicked, this, &InventoryWidget::checkInventory);
    connect(ui->refreshButton, &QPushButton::clicked, this, &InventoryWidget::refresh);
}

InventoryWidget::~InventoryWidget()
{
    delete ui;
}

void InventoryWidget::refresh() {
    model->setQuery("SELECT device_model AS '设备型号', MIN(name) AS '设备名称', SUM(quantity) AS '总库存', 2 AS '预警阈值' FROM equipment GROUP BY device_model");
    ui->tableView->setModel(model);
    ui->statusLabel->setText("库存状态: 未检查");
}

void InventoryWidget::checkInventory() {
    QSqlQueryModel *warningModel = new QSqlQueryModel(this);
    QString sql = "SELECT device_model AS '设备型号', MIN(name) AS '设备名称', SUM(quantity) AS '总库存', 2 AS '预警阈值' "
                  "FROM equipment GROUP BY device_model HAVING SUM(quantity) <= 2";
    warningModel->setQuery(sql);

    if (warningModel->rowCount() == 0) {
        ui->statusLabel->setText("库存状态: 所有设备型号库存充足");
        QMessageBox::information(this, "库存预警", "所有设备型号库存充足");
        // 显示全部型号库存
        model->setQuery("SELECT device_model AS '设备型号', MIN(name) AS '设备名称', SUM(quantity) AS '总库存', 2 AS '预警阈值' FROM equipment GROUP BY device_model");
        ui->tableView->setModel(model);
    } else {
        ui->statusLabel->setText("库存状态: 以下设备型号库存预警");
        ui->tableView->setModel(warningModel);
        model = warningModel; // 让model指向当前预警model，便于后续刷新
        QMessageBox::warning(this, "库存预警", "以下设备型号库存不足，请注意表格显示！");
    }
}
