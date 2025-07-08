#include "borrowreturnwidget.h"

BorrowReturnWidget::BorrowReturnWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建表格视图
    model = new QSqlRelationalTableModel(this);
    model->setTable("borrow_return");
    model->setRelation(1, QSqlRelation("equipment", "id", "name"));
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setHeaderData(1, Qt::Horizontal, "设备名称");
    model->setHeaderData(2, Qt::Horizontal, "借用人");
    model->setHeaderData(3, Qt::Horizontal, "借用日期");
    model->setHeaderData(4, Qt::Horizontal, "归还日期");
    model->setHeaderData(5, Qt::Horizontal, "用途");
    model->select();

    view = new QTableView;
    view->setModel(model);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSelectionBehavior(QAbstractItemView::SelectRows);
    view->resizeColumnsToContents();
    layout->addWidget(view);

    // 创建按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout;

    QPushButton *borrowButton = new QPushButton("借用");
    connect(borrowButton, &QPushButton::clicked, this, &BorrowReturnWidget::borrowEquipment);

    QPushButton *returnButton = new QPushButton("归还");
    connect(returnButton, &QPushButton::clicked, this, &BorrowReturnWidget::returnEquipment);

    QPushButton *searchButton = new QPushButton("搜索");
    connect(searchButton, &QPushButton::clicked, this, &BorrowReturnWidget::searchRecords);

    QPushButton *refreshButton = new QPushButton("刷新");
    connect(refreshButton, &QPushButton::clicked, this, &BorrowReturnWidget::refresh);

    buttonLayout->addWidget(borrowButton);
    buttonLayout->addWidget(returnButton);
    buttonLayout->addWidget(searchButton);
    buttonLayout->addWidget(refreshButton);
    buttonLayout->addStretch();

    layout->addLayout(buttonLayout);
}

void BorrowReturnWidget::refresh() {
    model->select();
}

void BorrowReturnWidget::borrowEquipment() {
    bool ok;
    QString equipmentName = QInputDialog::getText(this, "借用设备", "请输入设备名称:", QLineEdit::Normal, "", &ok);
    if (!ok || equipmentName.isEmpty()) return;

    QString borrower = QInputDialog::getText(this, "借用设备", "请输入借用人姓名:", QLineEdit::Normal, "", &ok);
    if (!ok || borrower.isEmpty()) return;

    QString purpose = QInputDialog::getText(this, "借用设备", "请输入用途:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    // 查询设备ID
    QSqlQuery query;
    query.prepare("SELECT id, quantity FROM equipment WHERE name = ?");
    query.addBindValue(equipmentName);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "错误", "设备不存在或查询失败");
        return;
    }

    int equipmentId = query.value(0).toInt();
    int quantity = query.value(1).toInt();

    if (quantity <= 0) {
        QMessageBox::warning(this, "错误", "该设备库存不足");
        return;
    }

    // 减少库存
    query.prepare("UPDATE equipment SET quantity = quantity - 1 WHERE id = ?");
    query.addBindValue(equipmentId);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "更新库存失败");
        return;
    }

    // 添加借用记录
    query.prepare("INSERT INTO borrow_return (equipment_id, borrower_name, borrow_date, purpose) "
                  "VALUES (?, ?, datetime('now'), ?)");
    query.addBindValue(equipmentId);
    query.addBindValue(borrower);
    query.addBindValue(purpose);

    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "添加借用记录失败: " + query.lastError().text());
        // 回滚库存
        query.prepare("UPDATE equipment SET quantity = quantity + 1 WHERE id = ?");
        query.addBindValue(equipmentId);
        query.exec();
        return;
    }

    refresh();
    QMessageBox::information(this, "成功", "设备借用成功");
}

void BorrowReturnWidget::returnEquipment() {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择要归还的记录");
        return;
    }

    int recordId = model->data(model->index(index.row(), 0)).toInt();
    int equipmentId = model->data(model->index(index.row(), 1)).toInt();

    // 检查是否已归还
    QSqlQuery query;
    query.prepare("SELECT return_date FROM borrow_return WHERE id = ?");
    query.addBindValue(recordId);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "错误", "查询记录失败");
        return;
    }

    if (!query.value(0).isNull()) {
        QMessageBox::warning(this, "警告", "该设备已归还");
        return;
    }

    // 更新归还日期
    query.prepare("UPDATE borrow_return SET return_date = datetime('now') WHERE id = ?");
    query.addBindValue(recordId);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "更新归还日期失败");
        return;
    }

    // 增加库存
    query.prepare("UPDATE equipment SET quantity = quantity + 1 WHERE id = ?");
    query.addBindValue(equipmentId);
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "更新库存失败");
        return;
    }

    refresh();
    QMessageBox::information(this, "成功", "设备归还成功");
}

void BorrowReturnWidget::searchRecords() {
    QString keyword = QInputDialog::getText(this, "搜索记录", "请输入设备名称或借用人:");
    if (!keyword.isEmpty()) {
        model->setFilter(QString("equipment.name LIKE '%%1%' OR borrower_name LIKE '%%1%'").arg(keyword));
        model->select();
    }
}
