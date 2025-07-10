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

void InventoryWidget::showWarning() {
    QSqlQuery query;
    // 以设备型号为单位，统计总库存小于等于2的型号
    query.exec("SELECT device_model, MIN(name), SUM(quantity) FROM equipment GROUP BY device_model HAVING SUM(quantity) <= 2");
    QVector<QVector<QString>> data;
    while (query.next()) {
        QVector<QString> row;
        row.append(query.value(1).toString()); // 设备名称
        row.append(query.value(0).toString()); // 设备型号
        row.append(query.value(2).toString()); // 总库存
        row.append("2"); // 预警阈值
        data.append(row);
    }
    if (data.isEmpty()) {
        QMessageBox::information(this, "库存预警", "当前没有低于阈值的设备型号");
        return;
    }
    // 用表格窗口显示
    QDialog dlg(this);
    dlg.setWindowTitle("库存预警设备");
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QTableWidget *table = new QTableWidget(data.size(), 4, &dlg);
    table->setHorizontalHeaderLabels(QStringList() << "设备名称" << "设备型号" << "总库存" << "预警阈值");
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    for (int i = 0; i < data.size(); ++i) {
        for (int j = 0; j < 4; ++j) {
            table->setItem(i, j, new QTableWidgetItem(data[i][j]));
        }
    }
    table->resizeColumnsToContents();
    layout->addWidget(table);
    QPushButton *okBtn = new QPushButton("确定", &dlg);
    connect(okBtn, &QPushButton::clicked, &dlg, &QDialog::accept);
    layout->addWidget(okBtn);
    dlg.exec();
}
