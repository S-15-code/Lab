#include "equipmentwidget.h"

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
