#include "borrowreturnwidget.h"
#include "ui_borrowreturnwidget.h"

BorrowReturnWidget::BorrowReturnWidget(QWidget *parent) : 
    QWidget(parent),
    ui(new Ui::BorrowReturnWidget)
{
    ui->setupUi(this);

    //  创建并配置模型（数据层 + 转换逻辑）
    model = new QSqlRelationalTableModel(this);
    model->setTable("borrow_return");//把模型绑定到该表
    model->setRelation(1, QSqlRelation("equipment", "id", "name"));//设置外键关系：将第 1 列（索引从 0 开始）关联到 equipment 表的 id 字段，并显示 name 字段。
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->setHeaderData(1, Qt::Horizontal, "设备名称");
    model->setHeaderData(2, Qt::Horizontal, "借用人");
    model->setHeaderData(3, Qt::Horizontal, "借用日期");
    model->setHeaderData(4, Qt::Horizontal, "归还日期");
    model->setHeaderData(5, Qt::Horizontal, "用途");
    model->select();

    // 创建并配置视图（纯显示层）
    ui->tableView->setModel(model);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->resizeColumnsToContents();

    // 连接信号和槽
    connect(ui->borrowButton, &QPushButton::clicked, this, &BorrowReturnWidget::borrowEquipment);
    connect(ui->returnButton, &QPushButton::clicked, this, &BorrowReturnWidget::returnEquipment);
    connect(ui->searchButton, &QPushButton::clicked, this, &BorrowReturnWidget::searchRecords);
    connect(ui->refreshButton, &QPushButton::clicked, this, &BorrowReturnWidget::refresh);
    connect(ui->borrowRankingButton, &QPushButton::clicked, this, &BorrowReturnWidget::showBorrowRanking);
}

BorrowReturnWidget::~BorrowReturnWidget()
{
    delete ui;
}

void BorrowReturnWidget::refresh() {
    model->setFilter(""); // 清空可能的过滤条件
    if (!model->select()) {
        QMessageBox::warning(this, "错误", "刷新失败: " + model->lastError().text());
    }
}
void BorrowReturnWidget::borrowEquipment() {
    bool ok;
    //快速创建一个文本输入对话框，获取信息
    QString equipmentName = QInputDialog::getText(this, "借用设备", "请输入设备名称:", QLineEdit::Normal, "", &ok);
    if (!ok || equipmentName.trimmed().isEmpty()) return;
    QString equipmentModel = QInputDialog::getText(this, "借用设备", "请输入设备型号:", QLineEdit::Normal, "", &ok);
    if (!ok || equipmentModel.trimmed().isEmpty()) return;
    QString borrower = QInputDialog::getText(this, "借用设备", "请输入借用人姓名:", QLineEdit::Normal, "", &ok);
    if (!ok || borrower.trimmed().isEmpty()) return;
    QString purpose = QInputDialog::getText(this, "借用设备", "请输入用途:", QLineEdit::Normal, "", &ok);
    if (!ok) return;

    QSqlDatabase db = QSqlDatabase::database();//返回程序之前通过 addDatabase() 建立的默认数据库连接
    db.transaction();
    QSqlQuery query(db);//创建该数据库的查询对象
    // 查找设备，忽略大小写和空格，且状态为正常且库存>0
    query.prepare("SELECT id, quantity FROM equipment WHERE TRIM(LOWER(name)) = TRIM(LOWER(?)) AND TRIM(LOWER(model)) = TRIM(LOWER(?)) AND status = '正常' AND quantity > 0");
    //                    查询内容           查询来源          查询条件
    query.addBindValue(equipmentName.trimmed().toLower());//绑定处理后的信息到查询位置
    query.addBindValue(equipmentModel.trimmed().toLower());
    if (!query.exec() || !query.next()) {
        db.rollback();
        // 新增：调试信息，显示所有设备信息
        QSqlQuery debugQuery(db);
        QString allEquipments;
        if (debugQuery.exec("SELECT name, device_model, status, quantity FROM equipment")) {
            while (debugQuery.next()) {//逐行遍历查询结果集
                allEquipments += QString("名称:%1 型号:%2 状态:%3 库存:%4\n")
                    .arg(debugQuery.value(0).toString())
                    .arg(debugQuery.value(1).toString())
                    .arg(debugQuery.value(2).toString())
                    .arg(debugQuery.value(3).toString());
            }
        }
        QMessageBox::warning(this, "错误", QString("设备不存在、状态不可借或库存不足\n\n输入名称: %1\n输入型号: %2\n\n所有设备:\n%3")
            .arg(equipmentName.trimmed())
            .arg(equipmentModel.trimmed())
            .arg(allEquipments));
        return;
    }
    int equipmentId = query.value(0).toInt();
    int quantity = query.value(1).toInt();

    // 库存-1
    query.prepare("UPDATE equipment SET quantity = quantity - 1 WHERE id = ?");//更新设备表 库存数量减1 限定只更新指定ID的设备
    query.addBindValue(equipmentId);
    if (!query.exec()) {
        db.rollback();
        QMessageBox::warning(this, "错误", "更新库存失败");
        return;
    }
    // 插入借用记录
    query.prepare("INSERT INTO borrow_return (equipment_id, borrower_name, borrow_date, purpose) VALUES (?, ?, datetime('now'), ?)");
    query.addBindValue(equipmentId);
    query.addBindValue(borrower.trimmed());
    query.addBindValue(purpose.trimmed());
    if (!query.exec()) {
        db.rollback();
        QMessageBox::warning(this, "错误", "添加借用记录失败: " + query.lastError().text());
        return;
    }
    // 再查一次库存，若为0则设备状态设为已借出，否则仍为正常
    query.prepare("SELECT quantity FROM equipment WHERE id = ?");
    query.addBindValue(equipmentId);
    if (!query.exec() || !query.next()) {
        db.rollback();
        QMessageBox::warning(this, "错误", "库存查询失败");
        return;
    }
    int left = query.value(0).toInt();
    QString newStatus = (left == 0) ? "已借出" : "正常";
    query.prepare("UPDATE equipment SET status = ? WHERE id = ?");
    query.addBindValue(newStatus);
    query.addBindValue(equipmentId);
    if (!query.exec()) {
        db.rollback();
        QMessageBox::warning(this, "错误", "更新设备状态失败");
        return;
    }
    db.commit();
    refresh();
    QMessageBox::information(this, "成功", "设备借用成功");
}

void BorrowReturnWidget::returnEquipment() {
    //获取当前选中行
    QModelIndex index = ui->tableView->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请选择要归还的记录");
        return;
    }
    // 获取借还记录ID
    int recordId = model->data(model->index(index.row(), 0)).toInt();
    // 正确获取equipment_id
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

    // 归还成功后，更新设备状态为正常
    QSqlQuery updateQuery;
    updateQuery.prepare("UPDATE equipment SET status = '正常' WHERE id = ?");
    updateQuery.addBindValue(equipmentId);
    updateQuery.exec();

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

    // 使用JOIN查询进行搜索:JOIN查询返回的是复杂的结果集
    QSqlQuery query;
    query.prepare(
        "SELECT DISTINCT br.id FROM borrow_return br "
        "LEFT JOIN equipment eq ON br.equipment_id = eq.id "
        "WHERE eq.name LIKE ? OR br.borrower_name LIKE ?"//eq.name OR br.borrower_name
    );
    query.addBindValue("%" + keyword + "%");
    query.addBindValue("%" + keyword + "%");
    
    if (!query.exec()) {
        QMessageBox::warning(this, "错误", "搜索查询失败: " + query.lastError().text());
        return;
    }
    
    // 收集匹配的记录ID
    QList<int> matchingIds;
    while (query.next()) {//遍历查询结果
        matchingIds.append(query.value(0).toInt());//query.value(0)	获取当前行第一列（对应SQL中的br.id）
    }
    
    if (matchingIds.isEmpty()) {
        QMessageBox::information(this, "搜索结果", "未找到匹配的记录");
        return;
    }
    
    // 构建ID过滤器：将复杂的跨表JOIN查询结果转换为简单的单表过滤条件
    //borrow_return.id IN (1,5,8)精确批量筛选所需要的信息记录
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

void BorrowReturnWidget::showBorrowRanking() {
    QSqlQuery query;
    // 按设备名称和型号分组，累计借用次数
    query.exec(
        "SELECT e.name, e.model, COUNT(br.id) as borrow_count "
        "FROM borrow_return br "
        "LEFT JOIN equipment e ON br.equipment_id = e.id "
        "WHERE e.name IS NOT NULL AND e.model IS NOT NULL "
        "GROUP BY e.name, e.model "
        "ORDER BY borrow_count DESC "
        "LIMIT 5"
    );
    QVector<QVector<QString>> data;
    while (query.next()) {
        QVector<QString> row;
        for (int i = 0; i < 3; ++i) {
            row.append(query.value(i).toString());
        }
        if (!row[0].isEmpty())
            data.append(row);
    }
    if (data.isEmpty()) {
        QMessageBox::information(this, "借用排行", "当前没有有效的借用记录");
        return;
    }
    QDialog dlg(this);
    dlg.setWindowTitle("借用频率排行");
    QVBoxLayout *layout = new QVBoxLayout(&dlg);
    QTableWidget *table = new QTableWidget(data.size(), 3, &dlg);
    table->setHorizontalHeaderLabels(QStringList() << "设备名称" << "型号" << "借用次数");
    table->verticalHeader()->setVisible(false);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionMode(QAbstractItemView::NoSelection);
    for (int i = 0; i < data.size(); ++i) {
        for (int j = 0; j < 3; ++j) {
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
