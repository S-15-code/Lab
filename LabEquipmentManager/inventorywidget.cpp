#include "inventorywidget.h"

InventoryWidget::InventoryWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 状态标签
    statusLabel = new QLabel("库存状态: 未检查");
    layout->addWidget(statusLabel);

    // 创建表格视图
    model = new QSqlQueryModel(this);
    view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(view);

    // 创建按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *checkButton = new QPushButton("检查库存");
    connect(checkButton, &QPushButton::clicked, this, &InventoryWidget::checkInventory);

    QPushButton *refreshButton = new QPushButton("刷新");
    connect(refreshButton, &QPushButton::clicked, this, &InventoryWidget::refresh);

    buttonLayout->addWidget(checkButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);
}

void InventoryWidget::refresh() {
    model->setQuery("SELECT model AS '设备型号', MIN(name) AS '设备名称', SUM(quantity) AS '总库存', 2 AS '预警阈值' FROM equipment GROUP BY model");
    view->setModel(model);
    statusLabel->setText("库存状态: 未检查");
}

void InventoryWidget::checkInventory() {
    QSqlQueryModel *warningModel = new QSqlQueryModel(this);
    QString sql = "SELECT model AS '设备型号', MIN(name) AS '设备名称', SUM(quantity) AS '总库存', 2 AS '预警阈值' "
                  "FROM equipment GROUP BY model HAVING SUM(quantity) <= 2";
    warningModel->setQuery(sql);

    if (warningModel->rowCount() == 0) {
        statusLabel->setText("库存状态: 所有设备型号库存充足");
        QMessageBox::information(this, "库存预警", "所有设备型号库存充足");
        // 显示全部型号库存
        model->setQuery("SELECT model AS '设备型号', MIN(name) AS '设备名称', SUM(quantity) AS '总库存', 2 AS '预警阈值' FROM equipment GROUP BY model");
        view->setModel(model);
    } else {
        statusLabel->setText("库存状态: 以下设备型号库存预警");
        view->setModel(warningModel);
        model = warningModel; // 让model指向当前预警model，便于后续刷新
        QMessageBox::warning(this, "库存预警", "以下设备型号库存不足，请注意表格显示！");
    }
}
