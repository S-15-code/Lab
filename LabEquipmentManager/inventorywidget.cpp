#include "inventorywidget.h"
#include "ui_inventorywidget.h"
#include <QMessageBox>

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

    // 初始刷新数据
    refresh();
}

InventoryWidget::~InventoryWidget()
{
    delete ui;
}

void InventoryWidget::refresh() {
    // 显示所有设备型号的库存统计，维修和报废算作正常库存为0，已借出台数为未归还数量
    QString queryStr =
        "SELECT e.model AS '设备型号', "
        "MIN(e.name) AS '设备名称', "
        "SUM(CASE WHEN e.status = '正常' THEN e.quantity ELSE 0 END) AS '正常库存', "
        "SUM(CASE WHEN e.status = '维修中' OR e.status = '报废' THEN e.quantity ELSE 0 END) AS '维修/报废台数', "
        "IFNULL((SELECT COUNT(1) FROM borrow_return br JOIN equipment eq2 ON br.equipment_id = eq2.id WHERE eq2.model = e.model AND br.return_date IS NULL), 0) AS '已借出台数', "
        "2 AS '预警阈值' "
        "FROM equipment e "
        "GROUP BY e.model "
        "ORDER BY SUM(CASE WHEN e.status = '正常' THEN e.quantity ELSE 0 END) ASC";

    model->setQuery(queryStr);
    if (model->lastError().isValid()) {
        QMessageBox::critical(this, "数据库错误", "查询失败: " + model->lastError().text());
    }
    ui->tableView->resizeColumnsToContents();
    ui->statusLabel->setText("库存状态: 未检查");
}

void InventoryWidget::checkInventory() {
    // 所有设备型号都要考虑库存预警，只有正常状态台数>2的才不预警，维修/报废算作正常台数为0，已借出台数为未归还数量
    QString warningQuery =
        "SELECT e.model AS '设备型号', "
        "MIN(e.name) AS '设备名称', "
        "SUM(CASE WHEN e.status = '正常' THEN e.quantity ELSE 0 END) AS '正常库存', "
        "SUM(CASE WHEN e.status = '维修中' OR e.status = '报废' THEN e.quantity ELSE 0 END) AS '维修/报废台数', "
        "IFNULL((SELECT COUNT(1) FROM borrow_return br JOIN equipment eq2 ON br.equipment_id = eq2.id WHERE eq2.model = e.model AND br.return_date IS NULL), 0) AS '已借出台数', "
        "2 AS '预警阈值' "
        "FROM equipment e "
        "GROUP BY e.model "
        "HAVING SUM(CASE WHEN e.status = '正常' THEN e.quantity ELSE 0 END) <= 2 "
        "ORDER BY SUM(CASE WHEN e.status = '正常' THEN e.quantity ELSE 0 END) ASC";

    QSqlQueryModel *warningModel = new QSqlQueryModel(this);
    warningModel->setQuery(warningQuery);
    if (warningModel->lastError().isValid()) {
        QMessageBox::critical(this, "数据库错误", "查询失败: " + warningModel->lastError().text());
        delete warningModel;
        return;
    }

    if (warningModel->rowCount() == 0) {
        ui->statusLabel->setText("库存状态: 所有设备型号库存充足");
        QMessageBox::information(this, "库存检查", "所有设备型号库存充足，没有低于预警阈值的型号。");
        refresh();  // 刷新显示全部数据
        delete warningModel;
    } else {
        ui->statusLabel->setText(QString("库存状态: 发现 %1 个型号库存不足").arg(warningModel->rowCount()));
        ui->tableView->setModel(warningModel);
        ui->tableView->resizeColumnsToContents();
        // 不要delete warningModel，因为tableView还在用
        QMessageBox::warning(this, "库存预警",
                            QString("发现 %1 个设备型号库存不足！\n请及时补充库存或联系供应商。")
                                .arg(warningModel->rowCount()));
    }
}
