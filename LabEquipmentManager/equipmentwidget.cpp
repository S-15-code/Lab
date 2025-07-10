#include "equipmentwidget.h"
#include "ui_equipmentwidget.h"
#include "statusitemdelegate.h"
#include <QSqlQuery>
#include <QDebug>
#include "mainwindow.h"
#include <QSqlTableModel>
#include <QInputDialog>
#include <QMessageBox>
#include <QDate>
#include <QTime>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QTimer>

EquipmentWidget::EquipmentWidget(QWidget *parent) : 
    QWidget(parent),
    ui(new Ui::EquipmentWidget)
{
    ui->setupUi(this);
    
    // 创建表格视图
    model = new QSqlTableModel(this);
    model->setTable("equipment");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    ui->tableView->setModel(model);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->resizeColumnsToContents();

    // 连接信号和槽
    connect(ui->addButton, &QPushButton::clicked, this, &EquipmentWidget::addEquipment);
    connect(ui->editButton, &QPushButton::clicked, this, &EquipmentWidget::editEquipment);
    connect(ui->deleteButton, &QPushButton::clicked, this, &EquipmentWidget::deleteEquipment);
    connect(ui->searchButton, &QPushButton::clicked, this, &EquipmentWidget::searchEquipment);
    connect(ui->refreshButton, &QPushButton::clicked, this, &EquipmentWidget::refresh);
    connect(ui->scrapInfoButton, &QPushButton::clicked, this, &EquipmentWidget::showScrapInfo);
    connect(ui->statisticsButton, &QPushButton::clicked, this, &EquipmentWidget::showStatistics);
    // 删除与维修、借还相关的按钮连接
    // connect(ui->maintenanceBtn, &QPushButton::clicked, this, &EquipmentWidget::markAsMaintenance);
    // connect(ui->scrapBtn, &QPushButton::clicked, this, &EquipmentWidget::markAsScrapped);
    // connect(ui->borrowBtn, &QPushButton::clicked, this, &EquipmentWidget::markAsBorrowed);
    // connect(ui->returnBtn, &QPushButton::clicked, this, &EquipmentWidget::markAsReturned);

    ui->tableView->setItemDelegate(new StatusItemDelegate(this));
}

EquipmentWidget::~EquipmentWidget()
{
    delete ui;
}

void EquipmentWidget::refresh() {
    model->setFilter(""); // 清除过滤条件
    model->select();      // 刷新所有数据
}


void EquipmentWidget::addEquipment() {
    // 1. 让用户输入设备名称
    bool nameOk;
    QString name = QInputDialog::getText(this, "输入设备名称",
                                        "请输入设备名称:",
                                        QLineEdit::Normal,
                                        "电脑", &nameOk);

    if (!nameOk) return;

    // 2. 让用户输入数量
    bool quantityOk;
    int quantity = QInputDialog::getInt(this, "输入数量",
                                      "请输入要添加的设备数量:",
                                      1, 1, 100, 1, &quantityOk);

    if (!quantityOk) return;

    // 3. 让用户输入设备型号
    bool modelOk;
    QString deviceModel = QInputDialog::getText(this, "输入型号",
                                              "请输入设备型号:",
                                              QLineEdit::Normal,
                                              "X-1000", &modelOk);

    if (!modelOk) return;

    // 4. 让用户选择购入年份（默认当前年份）
    bool yearOk;
    int currentYear = QDate::currentDate().year();
    int purchaseYear = QInputDialog::getInt(this, "购入年份",
                                          "请输入设备购入年份:",
                                          currentYear,
                                          2000,  // 最小年份
                                          currentYear,  // 最大年份
                                          1, &yearOk);

    if (!yearOk) return;

    // 初始化随机数种子（只需执行一次，建议放在构造函数中）
    qsrand(QTime::currentTime().msec());

    // 5. 开始事务处理
    QSqlDatabase::database().transaction();

    for (int i = 0; i < quantity; ++i) {
        int row = model->rowCount();
        model->insertRow(row);

        // 生成1000-9999的随机数
        int randomNumber = 1000 + (qrand() % 9000); // 1000 ≤ randomNumber ≤ 9999

        // 生成序列号：yyyy-xxxx格式
        QString serialNumber = QString("%1-%2")
            .arg(purchaseYear)
            .arg(randomNumber, 4, 10, QLatin1Char('0')); // 确保4位，不足补零

        // 設定各字段數據
        model->setData(model->index(row, 1),name );          // name
        model->setData(model->index(row, 2), deviceModel);   // device_model
        model->setData(model->index(row, 3), serialNumber);  // serial_number
        model->setData(model->index(row, 4), QDate::currentDate().toString("yyyy-MM-dd")); // 登记日期
        model->setData(model->index(row, 5), 1000.00);       // price
        model->setData(model->index(row, 6), 1);             // quantity (单台)
        model->setData(model->index(row, 7), "西南交大");     // location
        model->setData(model->index(row, 8), "正常");         // status
        model->setData(model->index(row, 9), 2);             // threshold
    }

    // 6. 提交事务
    if (model->submitAll()) {
        QSqlDatabase::database().commit();
        QMessageBox::information(this, "成功", QString("成功添加 %1 台 %2 设备\n序列号格式: %3-xxxx").arg(quantity).arg(name).arg(purchaseYear));
    } else {
        QSqlDatabase::database().rollback();
        model->revertAll();
        QMessageBox::warning(this, "错误", "添加失败: " + model->lastError().text());
    }
}
void EquipmentWidget::addMultipleEquipment(const QString& name,const QString& deviceModel,int quantity) {
    QSqlDatabase::database().transaction();
    int i=0;
    for (i = 0; i < quantity; ++i) {
        int row = model->rowCount();
        model->insertRow(row);

        QString serialNumber = QString("SN-%1-%2-%3")
            .arg(name.left(2).toUpper())
            .arg(deviceModel)
            .arg(i+1, 4, 10, QLatin1Char('0'));

        model->setData(model->index(row, 1), name);
        model->setData(model->index(row, 2), deviceModel);
        model->setData(model->index(row, 3), serialNumber);
        // 设置其他字段...
    }

    if (!model->submitAll()) {
        QSqlDatabase::database().rollback();
        model->revertAll();
        qDebug() << "批量添加失败:" << model->lastError().text();
    } else {
        QSqlDatabase::database().commit();
    }
}
void EquipmentWidget::editEquipment() {
    QModelIndex index = ui->tableView->currentIndex();
        if (!index.isValid()) {
            QMessageBox::warning(this, "警告", "请选择要编辑的设备");
            return;
        }

        ui->tableView->edit(index);

        // 确保编辑后提交
        if (!model->submitAll()) {
            QMessageBox::warning(this, "错误", "保存失败: " + model->lastError().text());
        }
}
QString EquipmentWidget::generateSerialNumber(const QString& name, const QString& deviceModel, int index) {
    return QString("%1-%2-%3-%4")
        .arg(QDate::currentDate().toString("yyyyMMdd"))
        .arg(name.left(2).toUpper())
        .arg(deviceModel)
        .arg(index, 4, 10, QLatin1Char('0'));
}
void EquipmentWidget::deleteEquipment() {
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) return;

    QString serialNumber = model->data(model->index(index.row(), 3)).toString();
    QString deviceModel = model->data(model->index(index.row(), 2)).toString();  // 获取型号

    if (QMessageBox::question(this, "确认",
        QString("确定要删除设备 %1 (型号:%2, SN:%3)吗?")
            .arg(model->data(model->index(index.row(), 1)).toString())
            .arg(deviceModel)
            .arg(serialNumber)) == QMessageBox::Yes) {

        model->removeRow(index.row());
        if (!model->submitAll()) {
            QMessageBox::warning(this, "错误", "删除失败: " + model->lastError().text());
        }
    }
}

void EquipmentWidget::searchEquipment() {
    QString keyword = QInputDialog::getText(this, "搜索设备", "请输入设备名称、型号或序列号:").trimmed();
    if (keyword.isEmpty()) {
        model->setFilter("");
        model->select();
        return;
    }
    // 搜索設備時修正 device_model
    QSqlQuery query;
    QString sql = "SELECT id FROM equipment WHERE name LIKE ? OR device_model LIKE ? OR serial_number LIKE ?";
    query.prepare(sql);
    QString likeStr = "%" + keyword + "%";
    query.addBindValue(likeStr);
    query.addBindValue(likeStr);
    query.addBindValue(likeStr);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "搜索失败: " + query.lastError().text() + "\nSQL: " + sql);
        return;
    }
    QStringList idList;
    while (query.next()) {
        idList << query.value(0).toString();
    }
    if (idList.isEmpty()) {
        QMessageBox::information(this, "搜索结果", "未找到匹配的设备记录");
        model->setFilter("");
        model->select();
        return;
    }
    QString filter = QString("id IN (%1)").arg(idList.join(","));
    model->setFilter(filter);
    model->select();
    QMessageBox::information(this, "搜索结果", QString("找到 %1 条匹配的设备记录").arg(model->rowCount()));
}

bool EquipmentWidget::changeStatus(int equipmentId, const QString& newStatus) {
    QSqlQuery query;
    query.prepare("UPDATE equipment SET status = ? WHERE id = ?");
    query.addBindValue(newStatus);
    query.addBindValue(equipmentId);

    if (!query.exec()) {
        qDebug() << "状态更新错误:" << query.lastError();
        return false;
    }
    return true;
}

void EquipmentWidget::showScrapInfo() {
    QSqlDatabase::database().commit();
    // 只显示报废设备
    model->setFilter("status = '报废'");
    model->select();
    int count = model->rowCount();
    if (count == 0) {
        QMessageBox::information(this, "报废信息", "当前没有报废的设备");
        model->setFilter("");
        model->select();
        return;
    }
    QMessageBox::information(this, "报废信息", QString("共有 %1 台报废设备，点击刷新可恢复全部列表").arg(count));
}

void EquipmentWidget::showStatistics() {
    QSqlQuery query;
    // 统计设备总数
    int total = 0;
    if (query.exec("SELECT COUNT(*) FROM equipment")) {
        if (query.next()) total = query.value(0).toInt();
    }
    // 统计各状态数量
    QMap<QString, int> statusMap;
    if (query.exec("SELECT status, COUNT(*) FROM equipment GROUP BY status")) {
        while (query.next()) {
            statusMap[query.value(0).toString()] = query.value(1).toInt();
        }
    }
    QString statText = QString("设备总数：%1\n").arg(total);
    for (auto it = statusMap.begin(); it != statusMap.end(); ++it) {
        statText += QString("%1：%2\n").arg(it.key()).arg(it.value());
    }
    QMessageBox::information(this, "设备统计", statText);
}
