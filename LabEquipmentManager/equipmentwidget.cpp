#include "equipmentwidget.h"
#include "statusitemdelegate.h"  // 确保文件名与实际一致
#include <QSqlQuery>  // 必须包含此头文件
#include <QDebug>  // 必须添加这行
#include "mainwindow.h"  // 确保路径正确
#include <QSqlTableModel>

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

        // 设置各字段数据
        model->setData(model->index(row, 1), name);          // name
        model->setData(model->index(row, 2), deviceModel);   // model
        model->setData(model->index(row, 3), serialNumber);  // serial_number
        model->setData(model->index(row, 4), QDate::currentDate().toString("yyyy-MM-dd")); // 登记日期
        model->setData(model->index(row, 5), 1000.00);       // price
        model->setData(model->index(row, 6), 1);             // quantity (单台)
        model->setData(model->index(row, 7), "西南交大");     // location
        model->setData(model->index(row, 8), "正常");         // status
        model->setData(model->index(row, 9), 1);             // threshold
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
    QModelIndex index = view->currentIndex();
        if (!index.isValid()) {
            QMessageBox::warning(this, "警告", "请选择要编辑的设备");
            return;
        }

        view->edit(index);

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
    QModelIndex index = view->currentIndex();
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
    QString keyword = QInputDialog::getText(this, "搜索设备",
        "请输入设备名称、型号或序列号:");
    if (!keyword.isEmpty()) {
        model->setFilter(QString("name LIKE '%%1%' OR device_model LIKE '%%1%' OR serial_number LIKE '%%1%'")
                      .arg(keyword));  // 注意这里也使用了 device_model
        model->select();
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

void EquipmentWidget::setEquipmentStatus(const QString& status) {
    QModelIndex index = view->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "警告", "请先选择设备");
        return;
    }

    int id = model->data(model->index(index.row(), 0)).toInt();
    QString name = model->data(model->index(index.row(), 1)).toString();

    if (QMessageBox::question(this, "确认",
        QString("确定要将设备 '%1' 状态改为 '%2' 吗?").arg(name).arg(status)) == QMessageBox::Yes)
    {
        if (changeStatus(id, status)) {
            refresh();
        } else {
            QMessageBox::warning(this, "错误", "状态更新失败");
        }
    }
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
