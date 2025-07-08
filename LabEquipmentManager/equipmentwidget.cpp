#include "equipmentwidget.h"
#include "statusitemdelegate.h"  // 确保文件名与实际一致
#include <QSqlQuery>  // 必须包含此头文件
#include <QDebug>  // 必须添加这行


EquipmentWidget::EquipmentWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建表格视图
    model = new QSqlTableModel(this);
    model->setTable("equipment");
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();

    view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->resizeColumnsToContents();
    layout->addWidget(view);

    // 创建按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *addButton = new QPushButton("添加");
    connect(addButton, &QPushButton::clicked, this, &EquipmentWidget::addEquipment);

    QPushButton *editButton = new QPushButton("编辑");
    connect(editButton, &QPushButton::clicked, this, &EquipmentWidget::editEquipment);

    QPushButton *deleteButton = new QPushButton("删除");
    connect(deleteButton, &QPushButton::clicked, this, &EquipmentWidget::deleteEquipment);

    QPushButton *searchButton = new QPushButton("搜索");
    connect(searchButton, &QPushButton::clicked, this, &EquipmentWidget::searchEquipment);

    QPushButton *refreshButton = new QPushButton("刷新");
    connect(refreshButton, &QPushButton::clicked, this, &EquipmentWidget::refresh);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(editButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(searchButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();

    // 在按钮布局中添加状态管理按钮
       QPushButton *maintenanceBtn = new QPushButton("送修");
       connect(maintenanceBtn, &QPushButton::clicked, this, &EquipmentWidget::markAsMaintenance);

       QPushButton *scrapBtn = new QPushButton("报废");
       connect(scrapBtn, &QPushButton::clicked, this, &EquipmentWidget::markAsScrapped);

       QPushButton *borrowBtn = new QPushButton("借出");
       connect(borrowBtn, &QPushButton::clicked, this, &EquipmentWidget::markAsBorrowed);

       QPushButton *returnBtn = new QPushButton("归还");
       connect(returnBtn, &QPushButton::clicked, this, &EquipmentWidget::markAsReturned);

       // 添加到按钮布局
       buttonLayout->addWidget(maintenanceBtn);
       buttonLayout->addWidget(scrapBtn);
       buttonLayout->addWidget(borrowBtn);
       buttonLayout->addWidget(returnBtn);

       view->setItemDelegate(new StatusItemDelegate(this));

    layout->addLayout(buttonLayout);
}

void EquipmentWidget::refresh() {
    model->select();
}

void EquipmentWidget::addEquipment() {
    int row = model->rowCount();
    model->insertRow(row);
    model->setData(model->index(row, 1), "新设备"); // name
    model->setData(model->index(row, 6), 1); // quantity
    model->setData(model->index(row, 8), "正常"); // status
    model->setData(model->index(row, 9), 1); // threshold

    if (!model->submitAll()) {
        QMessageBox::warning(this, "错误", "添加设备失败: " + model->lastError().text());
    }
}

void EquipmentWidget::editEquipment() {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择要编辑的设备");
        return;
    }
    view->edit(index);
}

void EquipmentWidget::deleteEquipment() {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择要删除的设备");
        return;
    }

    int id = model->data(model->index(index.row(), 0)).toInt();
    QString name = model->data(model->index(index.row(), 1)).toString();

    if (QMessageBox::question(this, "确认", QString("确定要删除设备 '%1' 吗?").arg(name)) == QMessageBox::Yes) {
        model->removeRow(index.row());
        if (!model->submitAll()) {
            QMessageBox::warning(this, "错误", "删除设备失败: " + model->lastError().text());
        }
    }
}

void EquipmentWidget::searchEquipment() {
    QString keyword = QInputDialog::getText(this, "搜索设备", "请输入设备名称或型号:");
    if (!keyword.isEmpty()) {
        model->setFilter(QString("name LIKE '%%1%' OR model LIKE '%%1%'").arg(keyword));
        model->select();
    }
}
// equipmentwidget.cpp
void EquipmentWidget::setEquipmentStatus(const QString& status) {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择设备");
        return;
    }

    int id = model->data(model->index(index.row(), 0)).toInt();
    QString name = model->data(model->index(index.row(), 1)).toString();

    if (QMessageBox::question(this, "确认",
        QString("确定要将设备 '%1' 状态改为 '%2' 吗?").arg(name).arg(status)) == QMessageBox::Yes) {

        if (changeStatus(id, status)) {
            refresh();
        } else {
            QMessageBox::warning(this, "错误", "状态更新失败");
        }
    }
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

void EquipmentWidget::markAsMaintenance() {
    setEquipmentStatus("维修中");

    // 可选：自动创建维修记录
    QModelIndex index = view->currentIndex();
    if (index.isValid()) {
        int id = model->data(model->index(index.row(), 0)).toInt();
        QSqlQuery query;
        query.prepare("INSERT INTO maintenance (equipment_id, maintenance_date, status) "
                     "VALUES (?, datetime('now'), '待处理')");
        query.addBindValue(id);
        query.exec();
    }
}

void EquipmentWidget::markAsScrapped() {
    setEquipmentStatus("已报废");
}

void EquipmentWidget::markAsBorrowed() {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) return;

    int id = model->data(model->index(index.row(), 0)).toInt();
    QString borrower = QInputDialog::getText(this, "借出登记", "请输入借用人姓名:");

    if (!borrower.isEmpty()) {
        if (changeStatus(id, "已借出")) {
            QSqlQuery query;
            query.prepare("INSERT INTO borrow_return (equipment_id, borrower_name, borrow_date) "
                         "VALUES (?, ?, datetime('now'))");
            query.addBindValue(id);
            query.addBindValue(borrower);
            query.exec();
            refresh();
        }
    }
}

void EquipmentWidget::markAsReturned() {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) return;

    int id = model->data(model->index(index.row(), 0)).toInt();

    if (changeStatus(id, "正常")) {
        QSqlQuery query;
        query.prepare("UPDATE borrow_return SET return_date = datetime('now') "
                     "WHERE equipment_id = ? AND return_date IS NULL");
        query.addBindValue(id);
        query.exec();
        refresh();
    }
}
