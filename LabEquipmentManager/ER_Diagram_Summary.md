# 实验室设备管理系统 E-R 图总结

## 核心实体

### 1. Equipment (设备表) - 核心实体
```
┌─────────────────────────────────┐
│           Equipment             │
├─────────────────────────────────┤
│ PK: id                         │
│ name (设备名称)                 │
│ device_model (设备型号)         │
│ serial_number (序列号, 唯一)    │
│ purchase_date (采购日期)        │
│ price (价格)                   │
│ quantity (库存数量)             │
│ location (存放地点)             │
│ status (状态)                  │
│ threshold (预警阈值)            │
│ scrap_time (报废时间)           │
│ scrap_reason (报废原因)         │
└─────────────────────────────────┘
```

### 2. Borrow_Return (借还记录表)
```
┌─────────────────────────────────┐
│        Borrow_Return            │
├─────────────────────────────────┤
│ PK: id                         │
│ FK: equipment_id → Equipment.id │
│ borrower_name (借用人)          │
│ borrow_date (借用日期)          │
│ return_date (归还日期)          │
│ purpose (用途)                  │
└─────────────────────────────────┘
```

### 3. Maintenance (维修记录表)
```
┌─────────────────────────────────┐
│         Maintenance             │
├─────────────────────────────────┤
│ PK: id                         │
│ FK: equipment_id → Equipment.id │
│ maintenance_date (报修日期)     │
│ issue_description (问题描述)    │
│ repair_description (维修描述)   │
│ cost (维修费用)                │
│ technician (维修人员)           │
│ completion_date (完成日期)      │
│ status (维修状态)              │
└─────────────────────────────────┘
```

## 实体关系

```
Equipment (1) ─────── (N) Borrow_Return
     │
     │ (1)
     │
     │ (N)
     ▼
Maintenance
```

## 关系说明

1. **Equipment → Borrow_Return (1:N)**
   - 一个设备可以有多个借还记录
   - 通过 equipment_id 外键关联

2. **Equipment → Maintenance (1:N)**
   - 一个设备可以有多个维修记录
   - 通过 equipment_id 外键关联

## 状态管理

### 设备状态 (Equipment.status)
- **正常**: 设备可用，库存 > 0
- **维修中**: 设备正在维修，不可借用
- **已借出**: 设备库存 = 0，但未报废
- **报废**: 设备永久不可用

### 维修状态 (Maintenance.status)
- **待维修**: 刚报修，等待处理
- **维修中**: 正在维修
- **已完成**: 维修完成

## 业务流程

### 设备借用流程
```
Equipment(库存检查) → Borrow_Return(添加记录) → Equipment(更新库存和状态)
```

### 设备归还流程
```
Borrow_Return(更新归还日期) → Equipment(增加库存和恢复状态)
```

### 设备维修流程
```
Equipment(状态检查) → Maintenance(添加记录) → Equipment(状态设为维修中)
```

### 维修完成流程
```
Maintenance(更新完成状态) → Equipment(状态恢复为正常)
```

## 数据约束

### 主键约束
- Equipment.id: PRIMARY KEY AUTOINCREMENT
- Borrow_Return.id: PRIMARY KEY AUTOINCREMENT
- Maintenance.id: PRIMARY KEY AUTOINCREMENT

### 外键约束
- Borrow_Return.equipment_id → Equipment.id
- Maintenance.equipment_id → Equipment.id

### 唯一约束
- Equipment.serial_number: UNIQUE

### 业务约束
- 设备状态只能是: 正常/维修中/已借出/报废
- 维修状态只能是: 待维修/维修中/已完成
- 库存数量不能为负数
- 报废设备不能进行维修或借用

## 视图

### v_maintenance_full
- 关联 Equipment 和 Maintenance 表
- 提供维修记录的完整信息视图
- 便于维修管理界面使用

## 功能模块

1. **设备管理模块**: 设备增删改查、状态管理、报废信息查看
2. **借还管理模块**: 设备借用、归还、记录查询
3. **维修管理模块**: 维修记录添加、状态更新、完成处理
4. **库存管理模块**: 库存检查、预警管理 