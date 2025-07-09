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
    model->setFilter(""); // 清除 filter
    if (!model->select()) {
        QMessageBox::warning(this, "错误", "刷新失败: " + model->lastError().text());
    }
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
    
    // 正确获取equipment_id，它应该是borrow_return表中的第二列（索引1）
    // 但由于设置了关系，我们需要通过查询来获取
    QSqlQuery query;
    query.prepare("SELECT equipment_id FROM borrow_return WHERE id = ?");
    query.addBindValue(recordId);
    if (!query.exec() || !query.next()) {
        QMessageBox::warning(this, "错误", "查询记录失败");
        return;
    }
    int equipmentId = query.value(0).toInt();

    // 检查是否已归还
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
    if (keyword.isEmpty()) {
        // 如果搜索关键词为空，清除过滤器并刷新
        model->setFilter("");
        model->select();
        return;
    }

    // 使用更可靠的方法：先查询匹配的记录ID，然后设置过滤器
    QSqlQuery query;
    query.prepare(
        "SELECT DISTINCT br.id FROM borrow_return br "
        "LEFT JOIN equipment eq ON br.equipment_id = eq.id "
        "WHERE eq.name LIKE ? OR br.borrower_name LIKE ?"
    );
    query.addBindValue("%" + keyword + "%");
    query.addBindValue("%" + keyword + "%");
    
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "搜索查询失败: " + query.lastError().text());
        return;
    }
    
    // 收集匹配的记录ID
    QList<int> matchingIds;
    while (query.next()) {
        matchingIds.append(query.value(0).toInt());
    }
    
    if (matchingIds.isEmpty()) {
        QMessageBox::information(this, "搜索结果", "未找到匹配的记录");
        return;
    }
    
    // 构建ID过滤器，明确指定表名
    QString idFilter = "borrow_return.id IN (";
    for (int i = 0; i < matchingIds.size(); ++i) {
        if (i > 0) idFilter += ",";
        idFilter += QString::number(matchingIds[i]);
    }
    idFilter += ")";
    
    // 设置过滤器
    model->setFilter(idFilter);
    
    // 刷新模型数据
    if (!model->select()) {
        QMessageBox::warning(this, "错误", "搜索失败: " + model->lastError().text());
        return;
    }
    
    // 显示搜索结果
    QMessageBox::information(this, "搜索结果", QString("找到 %1 条匹配的记录").arg(model->rowCount()));
}
