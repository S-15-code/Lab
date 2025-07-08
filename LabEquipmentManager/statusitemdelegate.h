#ifndef STATUSITEMDELEGATE_H
#define STATUSITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>
#include <QColor>

/**
 * @brief 自定义委托类，用于根据设备状态显示不同背景色
 */
class StatusItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit StatusItemDelegate(QObject *parent = nullptr);

    /**
     * @brief 重写绘制方法
     * @param painter 绘图设备
     * @param option 样式选项
     * @param index 模型索引
     */
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;

    /**
     * @brief 获取状态对应的颜色
     * @param status 设备状态字符串
     * @return 对应的颜色
     */
    static QColor statusColor(const QString &status);
};

#endif // STATUSITEMDELEGATE_H
