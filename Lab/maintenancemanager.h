#ifndef MAINTENANCEMANAGER_H
#define MAINTENANCEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QDate>
#include <QDebug>

struct MaintenanceRecord {
    int id;
    int equipmentId;
    QDate maintenanceDate;
    QString maintenanceType;
    QString description;
    double cost;
    QString technician;
    int completionStatus;
    QDate nextMaintenanceDate;
};

class MaintenanceManager : public QObject
{
    Q_OBJECT
public:
    explicit MaintenanceManager(QSqlDatabase db, QObject *parent = nullptr);

    bool addMaintenanceRecord(int equipmentId, const QDate &date, const QString &type,
                             const QString &desc, double cost, const QString &tech,
                             int status);
    bool updateMaintenanceRecord(const MaintenanceRecord &record);
    bool completeMaintenance(int recordId, const QDate &completionDate,
                            const QDate &nextMaintenanceDate);
    QList<MaintenanceRecord> getMaintenanceHistory(int equipmentId);
    QList<MaintenanceRecord> getPendingMaintenance();

private:
    QSqlDatabase m_db;
};

#endif // MAINTENANCEMANAGER_H
