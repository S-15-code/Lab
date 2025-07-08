#include "statusitemdelegate.h"

StatusItemDelegate::StatusItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{
}

void StatusItemDelegate::paint(QPainter *painter,
                             const QStyleOptionViewItem &option,
                             const QModelIndex &index) const
{
    // 只处理状态列（假设是第8列）
    if (index.column() == 8) {
        QString status = index.data().toString();
        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);

        // 设置背景色
        opt.backgroundBrush = QBrush(statusColor(status));

        // 保持文本颜色与背景对比
        if (status == "维修中" || status == "已报废") {
            opt.palette.setColor(QPalette::Text, Qt::black);
        }

        QStyledItemDelegate::paint(painter, opt, index);
    } else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QColor StatusItemDelegate::statusColor(const QString &status)
{
    // 定义状态颜色映射
    static const QHash<QString, QColor> colorMap = {
        {"正常", QColor(0, 255, 0, 50)},     // 绿色，半透明
        {"维修中", QColor(255, 255, 0, 127)}, // 黄色
        {"已报废", QColor(255, 0, 0, 127)},    // 红色
        {"已借出", QColor(0, 0, 255, 127)},    // 蓝色
        {"待检", QColor(255, 165, 0, 127)}     // 橙色
    };

    return colorMap.value(status, Qt::transparent); // 默认透明
}
