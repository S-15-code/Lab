#include "maintenancewidget.h"
#include "ui_maintenancewidget.h"
#include <QDate>
#include <QDateTime>
#include <QTimer>

MaintenanceWidget::MaintenanceWidget(QWidget *parent) : 
    QWidget(parent),
    ui(new Ui::MaintenanceWidget)
{
    ui->setupUi(this);

    // 创建表格视图
    model = new QSqlTableModel(this);
    model->setTable("v_maintenance_full");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setHeaderData(0, Qt::Horizontal, "id");
    model->setHeaderData(1, Qt::Horizontal, "设备名称");
    model->setHeaderData(2, Qt::Horizontal, "设备序列号");
    model->setHeaderData(3, Qt::Horizontal, "报修日期");
    model->setHeaderData(4, Qt::Horizontal, "问题描述");
    model->setHeaderData(5, Qt::Horizontal, "维修描述");
    model->setHeaderData(6, Qt::Horizontal, "费用");
    model->setHeaderData(7, Qt::Horizontal, "维修人员");
    model->setHeaderData(8, Qt::Horizontal, "完成日期");
    model->setHeaderData(9, Qt::Horizontal, "状态");
    model->select();

    ui->tableView->setModel(model);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->resizeColumnsToContents();

    // 连接信号和槽
    connect(ui->addButton, &QPushButton::clicked, this, &MaintenanceWidget::addMaintenance);
    connect(ui->updateButton, &QPushButton::clicked, this, &MaintenanceWidget::updateMaintenance);
    connect(ui->completeButton, &QPushButton::clicked, this, &MaintenanceWidget::completeMaintenance);
    connect(ui->scrapButton, &QPushButton::clicked, this, &MaintenanceWidget::scrapEquipment);
    connect(ui->searchButton, &QPushButton::clicked, this, &MaintenanceWidget::searchMaintenance);
    connect(ui->refreshButton, &QPushButton::clicked, this, &MaintenanceWidget::refresh);
}

MaintenanceWidget::~MaintenanceWidget()
{
    delete ui;
}

void MaintenanceWidget::refresh() {
    model->setFilter(""); // 清除过滤条件
    model->select();      // 刷新所有数据
}

void MaintenanceWidget::addMaintenance() {
    bool ok;
    // 设备名称
    QString equipmentName = QInputDialog::getText(this, "添加维修", "请输入设备名称:", QLineEdit::Normal, "", &ok);
    if (!ok || equipmentName.isEmpty()) return;
    // 序列号
    QString serialNumber = QInputDialog::getText(this, "添加维修", "请输入设备序列号:", QLineEdit::Normal, "", &ok);
    if (!ok || serialNumber.isEmpty()) return;
    // 查找设备ID
    QSqlQuery query;
    query.prepare("SELECT id FROM equipment WHERE name = ? AND serial_number = ?");
    query.addBindValue(equipmentName);
    query.addBindValue(serialNumber);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "错误", "未找到对应设备");
        return;
    }
    int equipmentId = query.value(0).toInt();
    
    // 检查设备状态，确保不是报废设备
    query.prepare("SELECT status FROM equipment WHERE id = ?");
    query.addBindValue(equipmentId);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "错误", "查询设备状态失败");
        return;
    }
    QString equipmentStatus = query.value(0).toString();
    
    if (equipmentStatus == "报废") {
        QMessageBox::warning(this, "错误", "该设备已报废，无法进行维修");
        return;
    }
    
    // 报修日期（精确到分钟）
    QString defaultDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
    QString maintenanceDate = QInputDialog::getText(
        this, "添加维修", "请输入报修日期(格式: yyyy-MM-dd HH:mm):",
        QLineEdit::Normal, defaultDateTime, &ok);
    if (!ok || maintenanceDate.isEmpty()) return;
    // 损坏原因（问题描述）
    QString issue = QInputDialog::getText(this, "添加维修", "请输入损坏原因:", QLineEdit::Normal, "", &ok);
    if (!ok || issue.isEmpty()) return;
    // 插入记录，其它字段留空
    query.prepare("INSERT INTO maintenance (equipment_id, maintenance_date, issue_description, status) VALUES (?, ?, ?, '待维修')");
    query.addBindValue(equipmentId);
    query.addBindValue(maintenanceDate);
    query.addBindValue(issue);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "添加维修记录失败: " + query.lastError().text());
        return;
    }
    // 同步设备状态为“维修中”
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE equipment SET status = '维修中' WHERE id = ?");
    updateQuery.addBindValue(equipmentId);
    updateQuery.exec();
    refresh();
    QMessageBox::information(this, "成功", "维修记录添加成功，设备状态已同步为维修中");
}

void MaintenanceWidget::updateMaintenance() {
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择要更新的记录");
        return;
    }

    int recordId = model->data(model->index(index.row(), 0)).toInt();

    // 检查设备是否已报废
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT e.status FROM equipment e JOIN maintenance m ON e.id = m.equipment_id WHERE m.id = ?");
    checkQuery.addBindValue(recordId);
    if (checkQuery.exec() && checkQuery.next()) {
        QString equipmentStatus = checkQuery.value(0).toString();
        if (equipmentStatus == "报废") {
            QMessageBox::warning(this, "警告", "该设备已报废，无法更新维修状态");
            return;
        }
    }

    bool ok;
    QString repairDesc = QInputDialog::getText(this, "更新维修", "请输入维修描述:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QString technician = QInputDialog::getText(this, "更新维修", "请输入维修人员:", QLineEdit::Normal, "", &ok);
    if (!ok || technician.isEmpty()) return;

    double cost = QInputDialog::getDouble(this, "更新维修", "请输入维修费用:", 0, 0, 100000, 2, &ok);
    if (!ok) return;

    QSqlQuery query;
    query.prepare("UPDATE maintenance SET repair_description = ?, technician = ?, cost = ?, status = '维修中' "
                  "WHERE id = ?");
    query.addBindValue(repairDesc);
    query.addBindValue(technician);
    query.addBindValue(cost);
    query.addBindValue(recordId);

    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "更新维修记录失败");
        return;
    }

    // 同步设备表状态为“维修中”
    QSqlQuery eqQuery;
    eqQuery.prepare("SELECT equipment_id FROM maintenance WHERE id = ?");
    eqQuery.addBindValue(recordId);
    if (eqQuery.exec() && eqQuery.next()) {
        int equipmentId = eqQuery.value(0).toInt();
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE equipment SET status = '维修中' WHERE id = ?");
        updateQuery.addBindValue(equipmentId);
        updateQuery.exec();
    }

    refresh();
    QMessageBox::information(this, "成功", "维修记录更新成功");
}

void MaintenanceWidget::completeMaintenance() {
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择要完成的记录");
        return;
    }

    int recordId = model->data(model->index(index.row(), 0)).toInt();

    // 检查设备是否已报废
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT e.status FROM equipment e JOIN maintenance m ON e.id = m.equipment_id WHERE m.id = ?");
    checkQuery.addBindValue(recordId);
    if (checkQuery.exec() && checkQuery.next()) {
        QString equipmentStatus = checkQuery.value(0).toString();
        if (equipmentStatus == "报废") {
            QMessageBox::warning(this, "警告", "该设备已报废，无法完成维修");
            return;
        }
    }

    QSqlQuery query;
    query.prepare("UPDATE maintenance SET completion_date = datetime('now'), status = '已完成' "
                  "WHERE id = ?");
    query.addBindValue(recordId);

    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "完成维修记录失败");
        return;
    }

    // 获取该维修记录对应的设备ID
    query.prepare("SELECT equipment_id FROM maintenance WHERE id = ?");
    query.addBindValue(recordId);
    if (query.exec() && query.next()) {
        int equipmentId = query.value(0).toInt();
        QSqlQuery updateQuery;
        updateQuery.prepare("UPDATE equipment SET status = '正常' WHERE id = ?");
        updateQuery.addBindValue(equipmentId);
        updateQuery.exec();
    }

    refresh();
    QMessageBox::information(this, "成功", "维修记录已完成，设备状态已同步为正常");
}

void MaintenanceWidget::searchMaintenance() {
    QString keyword = QInputDialog::getText(this, "搜索维修", "请输入设备名称、序列号、损坏原因或报修日期:");
    if (keyword.isEmpty()) {
        model->setFilter("");
        model->select();
        return;
    }
    // 修正SQL语法，使用正确的参数化查询
    QString filter = QString(
        "equipment_name LIKE '%%1%' OR "
        "serial_number LIKE '%%1%' OR "
        "issue_description LIKE '%%1%' OR "
        "maintenance_date LIKE '%%1%'"
    ).arg(keyword);

    model->setFilter(filter);
    model->select();

    if (model->rowCount() == 0) {
        QMessageBox::information(this, "搜索结果", "未找到匹配的维修记录");
    } else {
        QMessageBox::information(this, "搜索结果", QString("找到 %1 条匹配的维修记录").arg(model->rowCount()));
    }
}

void MaintenanceWidget::scrapEquipment() {
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择要报废的设备记录");
        return;
    }

    int recordId = model->data(model->index(index.row(), 0)).toInt();
    
    // 获取设备ID和状态
    QSqlQuery query;
    query.prepare("SELECT equipment_id FROM maintenance WHERE id = ?");
    query.addBindValue(recordId);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "错误", "查询设备ID失败");
        return;
    }
    int equipmentId = query.value(0).toInt();

    // 检查设备当前状态
    query.prepare("SELECT name, serial_number, status FROM equipment WHERE id = ?");
    query.addBindValue(equipmentId);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "错误", "查询设备信息失败");
        return;
    }
    QString equipmentName = query.value(0).toString();
    QString serialNumber = query.value(1).toString();
    QString equipmentStatus = query.value(2).toString();

    // 【调试弹窗】显示设备ID、名称、序列号、设备表状态
    QMessageBox::information(this, "调试", QString("设备ID: %1\n名称: %2\n序列号: %3\n设备表状态: %4")
        .arg(equipmentId).arg(equipmentName).arg(serialNumber).arg(equipmentStatus));

    // 检查设备是否已经报废
    if (equipmentStatus == "报废") {
        QMessageBox::warning(this, "警告", "该设备已经报废，无需重复处理");
        return;
    }

    // 检查设备是否在维修中
    if (equipmentStatus != "维修中") {
        QMessageBox::warning(this, "警告", "只有维修中的设备才能进行报废处理");
        return;
    }

    // 确认报废
    if (QMessageBox::question(this, "确认报废", 
        QString("确定要报废设备 %1 (序列号: %2) 吗？\n此操作不可撤销！")
        .arg(equipmentName).arg(serialNumber)) != QMessageBox::Yes) {
        return;
    }

    // 输入报废原因
    bool ok;
    QString scrapReason = QInputDialog::getText(this, "报废原因", 
        "请输入报废原因:", QLineEdit::Normal, "", &ok);
    if (!ok || scrapReason.isEmpty()) {
        QMessageBox::warning(this, "警告", "必须输入报废原因");
        return;
    }

    // 更新设备状态为报废，并记录报废时间和原因
    query.prepare("UPDATE equipment SET status = '报废', scrap_time = datetime('now'), scrap_reason = ? WHERE id = ?");
    query.addBindValue(scrapReason);
    query.addBindValue(equipmentId);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "报废处理失败: " + query.lastError().text());
        return;
    }

    // 更新维修记录状态为报废
    query.prepare("UPDATE maintenance SET status = '报废' WHERE id = ?");
    query.addBindValue(recordId);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "更新维修记录失败");
        return;
    }

    refresh();
    QMessageBox::information(this, "成功", 
        QString("设备 %1 已报废\n报废原因: %2").arg(equipmentName).arg(scrapReason));
}
