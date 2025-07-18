# 实验室设备管理系统 E-R 图 (可视化版本)

```
┌─────────────────────────────────────────────────────────────────────────────────┐
│                          实验室设备管理系统 E-R 图                              │
└─────────────────────────────────────────────────────────────────────────────────┘

                                    ┌─────────────────┐
                                    │    Equipment    │
                                    │   (设备表)      │
                                    ├─────────────────┤
                                    │ PK: id          │
                                    │ name            │
                                    │ device_model    │
                                    │ serial_number   │
                                    │ purchase_date   │
                                    │ price           │
                                    │ quantity        │
                                    │ location        │
                                    │ status          │
                                    │ threshold       │
                                    │ scrap_time      │
                                    │ scrap_reason    │
                                    └─────────────────┘
                                            │
                                            │ 1
                                            │
                                            │
                                            │ N
                                            │
                                            ▼
                                    ┌─────────────────┐
                                    │ Borrow_Return   │
                                    │  (借还记录表)   │
                                    ├─────────────────┤
                                    │ PK: id          │
                                    │ FK: equipment_id│
                                    │ borrower_name   │
                                    │ borrow_date     │
                                    │ return_date     │
                                    │ purpose         │
                                    └─────────────────┘

                                    ┌─────────────────┐
                                    │   Maintenance   │
                                    │   (维修记录表)   │
                                    ├─────────────────┤
                                    │ PK: id          │
                                    │ FK: equipment_id│
                                    │ maintenance_date│
                                    │ issue_desc      │
                                    │ repair_desc     │
                                    │ cost            │
                                    │ technician      │
                                    │ completion_date │
                                    │ status          │
                                    └─────────────────┘

                                    ┌─────────────────┐
                                    │ v_maintenance   │
                                    │    _full        │
                                    │   (维修视图)    │
                                    ├─────────────────┤
                                    │ id              │
                                    │ equipment_name  │
                                    │ serial_number   │
                                    │ maintenance_date│
                                    │ issue_desc      │
                                    │ repair_desc     │
                                    │ cost            │
                                    │ technician      │
                                    │ completion_date │
                                    │ status          │
                                    └─────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────┐
│                              关系说明                                          │
└─────────────────────────────────────────────────────────────────────────────────┘

1. Equipment (1) ─────── (N) Borrow_Return
   - 一个设备可以有多个借还记录
   - 通过 equipment_id 外键关联

2. Equipment (1) ─────── (N) Maintenance  
   - 一个设备可以有多个维修记录
   - 通过 equipment_id 外键关联

3. v_maintenance_full 视图
   - 关联 Equipment 和 Maintenance 表
   - 提供维修记录的完整信息视图

┌─────────────────────────────────────────────────────────────────────────────────┐
│                              状态流转图                                        │
└─────────────────────────────────────────────────────────────────────────────────┘

    正常 ──────→ 已借出 ──────→ 正常
     │           │
     │           │
     ▼           ▼
   维修中 ──────→ 报废
     │
     ▼
    正常

状态说明:
- 正常: 设备可用，库存 > 0
- 已借出: 设备库存 = 0，但未报废  
- 维修中: 设备正在维修，不可借用
- 报废: 设备永久不可用

┌─────────────────────────────────────────────────────────────────────────────────┐
│                              业务流程                                          │
└─────────────────────────────────────────────────────────────────────────────────┘

1. 设备借用流程:
   ┌─────────┐    ┌─────────────┐    ┌─────────┐
   │Equipment│───▶│Borrow_Return│───▶│Equipment│
   │库存检查 │    │添加借用记录 │    │更新库存 │
   └─────────┘    └─────────────┘    └─────────┘

2. 设备归还流程:
   ┌─────────────┐    ┌─────────┐
   │Borrow_Return│───▶│Equipment│
   │更新归还日期 │    │增加库存 │
   └─────────────┘    └─────────┘

3. 设备维修流程:
   ┌─────────┐    ┌─────────────┐    ┌─────────┐
   │Equipment│───▶│Maintenance  │───▶│Equipment│
   │状态检查 │    │添加维修记录 │    │设为维修 │
   └─────────┘    └─────────────┘    └─────────┘

4. 维修完成流程:
   ┌─────────────┐    ┌─────────┐
   │Maintenance  │───▶│Equipment│
   │更新完成状态 │    │恢复正常 │
   └─────────────┘    └─────────┘

┌─────────────────────────────────────────────────────────────────────────────────┐
│                              数据约束                                          │
└─────────────────────────────────────────────────────────────────────────────────┘

1. 主键约束:
   - Equipment.id: PRIMARY KEY AUTOINCREMENT
   - Borrow_Return.id: PRIMARY KEY AUTOINCREMENT  
   - Maintenance.id: PRIMARY KEY AUTOINCREMENT

2. 外键约束:
   - Borrow_Return.equipment_id → Equipment.id
   - Maintenance.equipment_id → Equipment.id

3. 唯一约束:
   - Equipment.serial_number: UNIQUE

4. 非空约束:
   - Equipment.name: NOT NULL

5. 业务约束:
   - 设备状态只能是: 正常/维修中/已借出/报废
   - 维修状态只能是: 待维修/维修中/已完成
   - 库存数量不能为负数
   - 报废设备不能进行维修或借用 