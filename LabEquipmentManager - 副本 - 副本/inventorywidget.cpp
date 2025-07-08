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
    model->setQuery("SELECT name, quantity, threshold FROM equipment");
    model->setHeaderData(0, Qt::Horizontal, "设备名称");
    model->setHeaderData(1, Qt::Horizontal, "库存数量");
    model->setHeaderData(2, Qt::Horizontal, "预警阈值");

    statusLabel->setText("库存状态: 未检查");
}

void InventoryWidget::checkInventory() {
    QSqlQuery query;
    query.exec("SELECT name, quantity, threshold FROM equipment WHERE quantity <= threshold");

    if (query.size() == 0) {
        statusLabel->setText("库存状态: 所有设备库存充足");
        return;
    }

    QStringList warningItems;
    while (query.next()) {
        warningItems << QString("%1 (库存: %2, 阈值: %3)")
                        .arg(query.value(0).toString())
                        .arg(query.value(1).toInt())
                        .arg(query.value(2).toInt());
    }

    statusLabel->setText("库存状态: 以下设备库存不足");
    model->setQuery(query.lastQuery());
    model->setHeaderData(0, Qt::Horizontal, "设备名称");
    model->setHeaderData(1, Qt::Horizontal, "库存数量");
    model->setHeaderData(2, Qt::Horizontal, "预警阈值");

    QMessageBox::warning(this, "库存警告",
                        "以下设备库存不足:\n" + warningItems.join("\n"));
}
