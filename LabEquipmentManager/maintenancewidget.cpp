#include "maintenancewidget.h"

MaintenanceWidget::MaintenanceWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建表格视图
    model = new QSqlRelationalTableModel(this);
    model->setTable("maintenance");
    model->setRelation(1, QSqlRelation("equipment", "id", "name"));
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setHeaderData(1, Qt::Horizontal, "设备名称");
    model->setHeaderData(2, Qt::Horizontal, "报修日期");
    model->setHeaderData(3, Qt::Horizontal, "问题描述");
    model->setHeaderData(4, Qt::Horizontal, "维修描述");
    model->setHeaderData(5, Qt::Horizontal, "费用");
    model->setHeaderData(6, Qt::Horizontal, "维修人员");
    model->setHeaderData(7, Qt::Horizontal, "完成日期");
    model->setHeaderData(8, Qt::Horizontal, "状态");
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
    model->select();
}

void MaintenanceWidget::addMaintenance() {
    bool ok;

    // 输入设备名称
    QString equipmentName = QInputDialog::getText(this, "添加维修", "请输入设备名称:", QLineEdit::Normal, "", &ok);
    if (!ok || equipmentName.isEmpty()) {
        return; // 用户取消或输入为空
    }

    // 检查设备是否存在
    QSqlQuery query;
    query.prepare("SELECT id FROM equipment WHERE name = ?");
    query.addBindValue(equipmentName);

    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "查询设备失败: " + query.lastError().text());
        return;
    }

    if (!query.next()) {
        // 没有找到对应的设备
        QMessageBox::critical(this, "错误", "设备不存在，请重新选择或添加设备。");
        return;
    }

    int equipmentId = query.value(0).toInt();

    // 输入问题描述
    QString issue = QInputDialog::getText(this, "添加维修", "请输入问题描述:", QLineEdit::Normal, "", &ok);
    if (!ok || issue.isEmpty()) {
        return;
    }

    // 插入维修记录
    query.prepare("INSERT INTO maintenance (equipment_id, maintenance_date, issue_description, status) "
                  "VALUES (?, datetime('now'), ?, '待维修')");
    query.addBindValue(equipmentId);
    query.addBindValue(issue);

    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "添加维修记录失败: " + query.lastError().text());
        return;
    }

    refresh();
    QMessageBox::information(this, "成功", "维修记录添加成功");
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

    refresh();
    QMessageBox::information(this, "成功", "维修记录已完成");
}

void MaintenanceWidget::searchMaintenance() {
    QString keyword = QInputDialog::getText(this, "搜索维修", "请输入设备名称或状态:");
    if (!keyword.isEmpty()) {
        model->setFilter(QString("equipment.name LIKE '%%1%' OR maintenance.status LIKE '%%1%'").arg(keyword));
        model->select();
    }
}
