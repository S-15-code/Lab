#ifndef EQUIPMENTMANAGER_H
#define EQUIPMENTMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

struct Equipment {
    int id;
    QString name;
    QString model;
    QString serialNumber;
    QDate purchaseDate;
    double price;
    int status;
    QString location;
    QString category;
    int warrantyPeriod;
    QDate lastMaintenanceDate;
    QDate nextMaintenanceDate;
    int threshold;
};

class EquipmentManager : public QObject
{
    Q_OBJECT
public:
    explicit EquipmentManager(QSqlDatabase db, QObject *parent = nullptr);

    bool addEquipment(const Equipment &equipment);
    bool updateEquipment(const Equipment &equipment);
    bool deleteEquipment(int id);
    QList<Equipment> getAllEquipment();
    Equipment getEquipmentById(int id);
    QList<Equipment> getEquipmentByStatus(int status);
    QList<Equipment> getEquipmentNeedMaintenance();
    QList<Equipment> getEquipmentBelowThreshold();

private:
    QSqlDatabase m_db;
};

#endif // EQUIPMENTMANAGER_H
