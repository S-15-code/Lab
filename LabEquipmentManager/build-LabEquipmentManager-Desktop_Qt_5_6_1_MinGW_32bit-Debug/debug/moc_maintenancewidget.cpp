/****************************************************************************
** Meta object code from reading C++ file 'maintenancewidget.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.6.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../LabEquipmentManager/maintenancewidget.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'maintenancewidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.6.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_MaintenanceWidget_t {
    QByteArrayData data[6];
    char stringdata0[90];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_MaintenanceWidget_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_MaintenanceWidget_t qt_meta_stringdata_MaintenanceWidget = {
    {
QT_MOC_LITERAL(0, 0, 17), // "MaintenanceWidget"
QT_MOC_LITERAL(1, 18, 14), // "addMaintenance"
QT_MOC_LITERAL(2, 33, 0), // ""
QT_MOC_LITERAL(3, 34, 17), // "updateMaintenance"
QT_MOC_LITERAL(4, 52, 19), // "completeMaintenance"
QT_MOC_LITERAL(5, 72, 17) // "searchMaintenance"

    },
    "MaintenanceWidget\0addMaintenance\0\0"
    "updateMaintenance\0completeMaintenance\0"
    "searchMaintenance"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_MaintenanceWidget[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   34,    2, 0x08 /* Private */,
       3,    0,   35,    2, 0x08 /* Private */,
       4,    0,   36,    2, 0x08 /* Private */,
       5,    0,   37,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void MaintenanceWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        MaintenanceWidget *_t = static_cast<MaintenanceWidget *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->addMaintenance(); break;
        case 1: _t->updateMaintenance(); break;
        case 2: _t->completeMaintenance(); break;
        case 3: _t->searchMaintenance(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObject MaintenanceWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MaintenanceWidget.data,
      qt_meta_data_MaintenanceWidget,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *MaintenanceWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MaintenanceWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_MaintenanceWidget.stringdata0))
        return static_cast<void*>(const_cast< MaintenanceWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int MaintenanceWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 4)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
