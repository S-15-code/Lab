#include "maintenancewidget.h"
#include <QDate>
#include <QDateTime>

MaintenanceWidget::MaintenanceWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

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

    view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->resizeColumnsToContents();
    layout->addWidget(view);

    // 创建按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *addButton = new QPushButton("添加维修");
    connect(addButton, &QPushButton::clicked, this, &MaintenanceWidget::addMaintenance);

    QPushButton *updateButton = new QPushButton("更新状态");
    connect(updateButton, &QPushButton::clicked, this, &MaintenanceWidget::updateMaintenance);

    QPushButton *completeButton = new QPushButton("完成维修");
    connect(completeButton, &QPushButton::clicked, this, &MaintenanceWidget::completeMaintenance);

    QPushButton *searchButton = new QPushButton("搜索");
    connect(searchButton, &QPushButton::clicked, this, &MaintenanceWidget::searchMaintenance);

    QPushButton *refreshButton = new QPushButton("刷新");
    connect(refreshButton, &QPushButton::clicked, this, &MaintenanceWidget::refresh);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(completeButton);
    buttonLayout->addWidget(searchButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);
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
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择要更新的记录");
        return;
    }

    int recordId = model->data(model->index(index.row(), 0)).toInt();

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

    refresh();
    QMessageBox::information(this, "成功", "维修记录更新成功");
}

void MaintenanceWidget::completeMaintenance() {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择要完成的记录");
        return;
    }

    int recordId = model->data(model->index(index.row(), 0)).toInt();

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
    // 直接在视图字段上模糊匹配
    QString filter = QString(
        "equipment_name LIKE '%%1%%' OR "
        "serial_number LIKE '%%1%%' OR "
        "issue_description LIKE '%%1%%' OR "
        "maintenance_date LIKE '%%1%%'"
    ).arg(keyword);

    model->setFilter(filter);
    model->select();

    if (model->rowCount() == 0) {
        QMessageBox::information(this, "搜索结果", "未找到匹配的维修记录");
    } else {
        QMessageBox::information(this, "搜索结果", QString("找到 %1 条匹配的维修记录").arg(model->rowCount()));
    }
}
