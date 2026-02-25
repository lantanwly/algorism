# Apollo LQR 横向控制模块详细解析

## 1. 模块概述

**模块名称**: LQR-Based Lateral Controller（基于LQR的横向控制器）  
**位置**: `modules/control/controllers/lat_based_lqr_controller/`  
**核心算法**: 线性二次型调节器 (Linear Quadratic Regulator, LQR)  
**功能**: 基于车辆状态和规划轨迹，计算转向角命令以实现车辆横向控制

## 2. 模块架构

```
lat_based_lqr_controller/
├── lat_controller.h          # 核心头文件
├── lat_controller.cc         # 核心实现文件（938行）
├── lat_controller_test.cc    # 单元测试
├── proto/
│   └── lat_based_lqr_controller_conf.proto  # 配置参数
├── conf/                     # 配置文件目录
├── docs/                     # 文档
└── lateral_controller_test/  # 测试数据
```

## 3. 核心类结构

### 3.1 LatController 类

继承自 `ControlTask`，是一个插件（通过 `CYBER_PLUGIN_MANAGER_REGISTER_PLUGIN` 注册）。

#### 关键方法：
```cpp
// 初始化
common::Status Init(std::shared_ptr<DependencyInjector> injector)

// 计算控制命令（主核心方法）
common::Status ComputeControlCommand(
    const localization::LocalizationEstimate *localization,
    const canbus::Chassis *chassis,
    const planning::ADCTrajectory *trajectory,
    ControlCommand *cmd)

// 重置控制器
common::Status Reset()

// 停止控制器
void Stop()

// 获取控制器名字
std::string Name() const
```

## 4. LQR 控制算法原理

### 4.1 车辆动力学模型

系统采用**离散线性车辆动力学模型**，基于自行车模型：

#### 状态向量 (State Vector)
```
x = [y_e, dy_e/dt, θ_e, dθ_e/dt]ᵀ

其中：
- y_e: 横向误差（侧向距离相对轨迹的偏差）
- dy_e/dt: 横向误差率（侧向速度）
- θ_e: 航向误差（车头方向与轨迹切向的差）
- dθ_e/dt: 航向误差率（角速度）
```

#### 控制输入
```
u = δ  # 转向角（steering angle）
```

### 4.2 A 矩阵（状态转移矩阵）

**前进挡（GEAR_DRIVE）**:
```
A = [0.0,  1.0,  0.0,    0.0           ]
    [0.0,  a11,  a12,    a13           ]
    [0.0,  0.0,  0.0,    1.0           ]
    [0.0,  a31,  a32,    a33           ]

其中：
a11 = -(cf + cr) / (m*v)
a12 = (cf + cr) / m
a13 = (lr*cr - lf*cf) / (m*v)
a31 = (lr*cr - lf*cf) / (iz*v)
a32 = (lf*cf - lr*cr) / iz
a33 = -(lf²*cf + lr²*cr) / (iz*v)
```

**倒车挡（GEAR_REVERSE）**:
```
将 cf 和 cr 取负，同时调整a11的偏导数
```

其中车辆参数：
- `cf`, `cr`: 前后轮胎侧偏刚度 (N/rad)
- `m`: 车辆质量 (kg)
- `v`: 纵向速度 (m/s)
- `lf`: 重心到前轮距离 (m)
- `lr`: 重心到后轮距离 (m)
- `iz`: 车辆绕Z轴的转动惯量 (kg*m²)

### 4.3 B 矩阵（控制影响矩阵）

```
B = [0.0, cf/m, 0.0, lf*cf/iz]ᵀ
```

### 4.4 离散化

系统使用欧拉前向差分进行离散化，采样时间 ts（通常为 0.01s）：
```
Ad = I + A*ts
Bd = B*ts
```

### 4.5 预看模型（Preview Model）

通过引入**预看窗口**扩展状态向量，对未来路径进行前向预测：

```
x_extended = [y_e, dy_e/dt, θ_e, dθ_e/dt, 
              y_preview_1, y_preview_2, ..., y_preview_n]ᵀ

其中 n = preview_window（预看窗口大小）
```

扩展矩阵：
```
Adc, Bdc: 复合状态转移和控制矩阵（包含预看部分）
```

### 4.6 成本函数（Cost Function）

LQR问题的目标是最小化二次成本函数：

```
J = Σ(xᵀ*Q*x + uᵀ*R*u)

其中：
- Q: 状态权重矩阵（对角）
  Q = diag(q₀, q₁, q₂, q₃, q₄, ..., q_{n+3})
  - q₀: 横向误差权重
  - q₂: 航向误差权重
  - q₄～q_{n+3}: 预看横向误差权重

- R: 控制权重矩阵（[1] 的 1x1 矩阵）
  控制平滑性权重
```

### 4.7 LQR 求解

使用迭代Riccati方程求解器计算增益矩阵K：

```cpp
common::math::SolveLQRProblem(
    matrix_adc_,      // 扩展状态转移矩阵
    matrix_bdc_,      // 扩展控制影响矩阵
    matrix_q_updated_, // 更新的状态权重矩阵
    matrix_r_,         // 控制权重矩阵
    lqr_eps_,          // 收敛阈值
    lqr_max_iteration_, // 最大迭代次数
    &matrix_k_,        // 输出：增益矩阵
    &num_iteration,    // 实际迭代次数
    &result_diff       // 收敛差值
)
```

### 4.8 反馈控制律

```
u = -K * x = -[k₀, k₁, k₂, k₃, ...] * [y_e, dy_e, θ_e, dθ_e, ...]ᵀ

转换为转向角（度数，0-100%）：
steer_angle = -(K*state)(0,0) * 180/π * steer_ratio / max_steer_degree * 100
```

## 5. 主要功能流程

### 5.1 初始化流程（Init）

```
1. 加载配置参数（LoadControlConf）
   - 采样时间 ts
   - 前后轮胎侧偏刚度 cf, cr
   - 预看窗口大小
   - 车辆参数

2. 初始化矩阵（基础4x4 + 预看）
   - matrix_a_, matrix_b_
   - matrix_q_, matrix_r_

3. 初始化过滤器（InitializeFilters）
   - 数字滤波器（平滑转向命令）
   - 均值滤波器（平滑误差）

4. 加载增益调度器（LoadLatGainScheduler）
   - 横向误差增益随速度变化的映射表
   - 航向误差增益随速度变化的映射表

5. 启用高级功能
   - Lead/Lag 控制器（反向倒车补偿）
   - MRAC 控制器（模型参考自适应控制）
```

### 5.2 控制计算流程（ComputeControlCommand）

```
1. 导航模式处理
   如果启用导航模式，变换轨迹坐标到当前车辆坐标系

2. 轨迹分析器初始化
   trajectory_analyzer_.Init(planning_trajectory)

3. 倒车模型调整
   根据档位（GEAR_DRIVE/GEAR_REVERSE）调整cf, cr符号和模型

4. 更新系统矩阵
   UpdateMatrix()  // A, B 矩阵根据当前速度更新
   UpdateMatrixCompound()  // 组合预看模型

5. 更新状态向量
   UpdateState()  // 计算 y_e, dy_e, θ_e, dθ_e 和预看误差

6. 更新Q矩阵权重
   - 倒车时使用 reverse_matrix_q
   - 前进时使用 matrix_q
   - 如果启用增益调度器，根据速度调整权重

7. 求解LQR问题
   获得增益矩阵 K

8. 计算控制命令
   a) 反馈控制: steer_angle_feedback = -K * state
   b) 前馈控制: steer_angle_feedforward = 基于曲率的前馈项
   c) 增强反馈: steer_angle_feedback_augment = Lead/Lag补偿（可选）

9. 应用约束和滤波
   - 限制最大转向角（基于最大侧向加速度）
   - 限制转向速率（基于车辆最大转向速率）
   - Lead/Lag 控制补偿（倒车）
   - MRAC 自适应控制（可选）
   - 数字滤波平滑

10. 特殊处理
    - 低速转向锁定（保持前一个转向角）
    - 路径剩余判断

11. 输出控制命令
    cmd->set_steering_target(steer_angle)
    cmd->set_steering_rate(steer_rate)
```

## 6. 关键误差计算

### 6.1 横向误差（Lateral Error）

```cpp
// 找最近的轨迹点
matched_point = trajectory_analyzer.QueryNearestPointByPosition(x, y)

// 计算误差
dx = x - matched_point.x
dy = y - matched_point.y

// 转换到Frenet坐标系
cos_theta = cos(matched_point.theta)
sin_theta = sin(matched_point.theta)

lateral_error = cos_theta * dy - sin_theta * dx
```

### 6.2 航向误差（Heading Error）

```cpp
heading_error = vehicle_heading - trajectory_heading
```

### 6.3 预看误差（Preview Lateral Error）

```cpp
// 预看时间 = ts * (i + 1)，其中 i = 0, 1, ..., preview_window-1
preview_point = trajectory_analyzer.QueryByRelativeTime(preview_time)

// 计算预看点的横向误差
// 用于改进前向跟踪性能
preview_lateral_error = ...
```

## 7. 高级特性

### 7.1 增益调度器（Gain Scheduler）

根据车速动态调整Q矩阵的权重：

```cpp
if (enable_gain_scheduler) {
    matrix_q_updated_(0, 0) = 
        matrix_q_(0, 0) * lat_err_interpolation->Interpolate(|v|)
    
    matrix_q_updated_(2, 2) = 
        matrix_q_(2, 2) * heading_err_interpolation->Interpolate(|v|)
}
```

**目的**: 在低速时增加横向精度，高速时平衡稳定性

### 7.2 Lead/Lag 控制（反向倒车补偿）

```cpp
if (enable_leadlag_) {
    steer_angle_feedback_augment = 
        leadlag_controller.Control(-matrix_state_(0, 0), ts_)
}
```

用传递函数模型补偿倒车时的转向响应延迟

### 7.3 MRAC 自适应控制（模型参考自适应）

```cpp
if (enable_mrac_) {
    steer_state = [steering_position, steering_rate]ᵀ
    steer_angle = mrac_controller.Control(
        steer_angle, steer_state, steer_limit, steer_rate_limit)
}
```

**优势**:
- 在线参数调整
- 适应转向系统的动态变化
- 提高鲁棒性

### 7.4 看前控制（Look-Ahead Back Control）

```cpp
if (enable_look_ahead_back_control_) {
    matrix_state_(0, 0) = debug->lateral_error_feedback()
    matrix_state_(2, 0) = debug->heading_error_feedback()
} else {
    matrix_state_(0, 0) = debug->lateral_error()
    matrix_state_(2, 0) = debug->heading_error()
}
```

在倒车或切换时使用look-back估计改进精度

## 8. 配置参数说明

### 8.1 proto 配置文件结构

```protobuf
message LatBaseLqrControllerConf {
    // 基础参数
    optional double ts = 1;                    // 采样时间（0.01s）
    optional int32 preview_window = 2;         // 预看窗口大小
    optional double cf = 3;                    // 前轮胎侧偏刚度
    optional double cr = 4;                    // 后轮胎侧偏刚度
    
    // LQR 权重矩阵
    repeated double matrix_q = 10;             // 状态权重（4+preview_window个）
    repeated double reverse_matrix_q = 11;    // 倒车时的状态权重
    optional int32 max_iteration = 14;         // LQR求解最大迭代次数
    
    // 约束参数
    optional double max_lateral_acceleration = 15;  // 最大侧向加速度
    optional double lock_steer_speed = 27;          // 转向锁定速度
    
    // 增益调度
    optional apollo.control.GainScheduler 
        lat_err_gain_scheduler = 16;
    optional apollo.control.GainScheduler 
        heading_err_gain_scheduler = 17;
    
    // 高级功能开关
    optional bool enable_reverse_leadlag_compensation = 19;
    optional bool enable_look_ahead_back_control = 20;
    optional bool enable_steer_mrac_control = 24;
    optional LeadlagConf reverse_leadlag_conf = 18;
    optional MracConf steer_mrac_conf = 23;
}
```

### 8.2 典型参数配置

```
ts: 0.01                          # 10ms 控制周期
preview_window: 5                 # 预看5步
cf: 80000, cr: 80000             # 轮胎侧偏刚度

matrix_q: [1.0, 0.0, 1.0, 0.0,   # 4个基础状态权重
           1.0, 1.0, 1.0, 1.0, 1.0]  # 5个预看权重

max_iteration: 15                 # LQR求解最大15次迭代
max_lateral_acceleration: 3.0     # 最多允许3 m/s² 侧向加速度
```

## 9. 数据流与关键变量

### 9.1 输入数据

```
from localization:
- x, y: 车辆全局坐标
- heading: 车辆航向角
- v: 纵向速度
- angular_velocity: 角速度

from planning:
- trajectory_points[]: 规划轨迹点序列

from chassis:
- steering_percentage: 当前转向百分比（-100~100）
- gear: 档位（前进/倒车）
```

### 9.2 核心矩阵（Matrices）

| 矩阵 | 大小 | 说明 |
|------|------|------|
| matrix_a_ | 4×4 | 连续状态转移矩阵 |
| matrix_ad_ | 4×4 | 离散状态转移矩阵 |
| matrix_b_ | 4×1 | 连续控制影响矩阵 |
| matrix_bd_ | 4×1 | 离散控制影响矩阵 |
| matrix_adc_ | (4+n)×(4+n) | 扩展离散矩阵（含预看） |
| matrix_bdc_ | (4+n)×1 | 扩展控制矩阵 |
| matrix_q_ | (4+n)×(4+n) | 状态权重矩阵 |
| matrix_r_ | 1×1 | 控制权重（通常为1.0） |
| matrix_k_ | 1×(4+n) | LQR增益矩阵（求解结果） |
| matrix_state_ | (4+n)×1 | 状态向量 |

### 9.3 输出数据

```
steering_target: 转向目标角（-100~100%）
steering_rate: 转向速率（°/s）

调试信息（SimpleLateralDebug）:
- lateral_error: 横向误差
- heading_error: 航向误差
- steer_angle_feedback: 反馈控制项
- steer_angle_feedforward: 前馈控制项
- steer_angle_lateral_contribution: 各项分解贡献
```

## 10. 算法性能特点

### 10.1 优势

✅ **快速响应**: 闭环反馈，反应迅速  
✅ **全局最优**: 求解的是二次成本函数的全局最优解  
✅ **稳定性好**: 理论上能保证系统稳定  
✅ **可预测性**: 预看窗口提前规划轨迹  
✅ **鲁棒性**: 支持多种补偿方式（Lead/Lag, MRAC）  
✅ **适应性**: 增益调度随速度变化  

### 10.2 局限性

⚠️ **线性模型**: 仅在小角度假设下准确，大转向角精度下降  
⚠️ **参数依赖**: 轮胎侧偏刚度需要精确标定  
⚠️ **计算量**: 每个控制周期需要求解LQR问题  
⚠️ **冷启动**: 对初始轨迹跟踪误差敏感  

## 11. 与其他模块的关联

```
┌─────────────────────┐
│   Localization      │
│   (位置/姿态)       │
└──────────┬──────────┘
           │
           ↓
┌─────────────────────┐        ┌──────────────────┐
│   Planning          │───────→│  LatController   │
│   (轨迹规划)        │        │  (LQR控制)       │
└──────────┬──────────┘        └────────┬─────────┘
           │                            │
           │                            ↓
           │                   ┌──────────────────┐
           │                   │  LonController   │
           │                   │  (纵向控制)      │
           │                   └────────┬─────────┘
           │                            │
           └────────────────┬───────────┘
                            ↓
                   ┌──────────────────┐
                   │   ControlTask    │
                   │   Manager        │
                   └────────┬─────────┘
                            ↓
                   ┌──────────────────┐
                   │   CAN Bus        │
                   │   (车辆CAN)      │
                   └──────────────────┘
```

## 12. 调试和日志

### 12.1 日志输出

```cpp
FLAGS_enable_csv_debug = true  // 启用CSV日志

日志文件: /tmp/steer_log_simple_optimal_[日期时间].csv

内容：
current_lateral_error,
current_ref_heading,
current_heading,
current_heading_error,
heading_error_rate,
lateral_error_rate,
current_curvature,
steer_angle,
steer_angle_feedforward,
...
```

### 12.2 调试信息

```protobuf
SimpleLateralDebug:
- lateral_error
- lateral_error_rate
- heading_error
- heading_error_rate
- curvature
- steer_angle_feedback
- steer_angle_feedforward
- steer_angle_limited
- heading (driving_orientation)
- ref_speed
- steer_mrac_debug (if enabled)
```

## 13. 性能优化建议

1. **预看窗口调优**: 
   - 低速增加（如5-7），精度优先
   - 高速减少（如3-5），稳定性优先

2. **权重矩阵调参**:
   - matrix_q[0]（横向误差权重）：越大越快纠正横向偏差
   - matrix_q[2]（航向误差权重）：越大航向跟踪越紧

3. **增益调度**:
   - 低速倍数增大（如1.5-2.0）
   - 高速倍数减小（如0.8-1.0）

4. **采样时间**:
   - 理论上越小越准确，但计算量增加
   - 推荐范围：0.005s - 0.02s

## 14. 文献参考

- **Rajamani, R.** (2011). *Vehicle dynamics and control*. Springer Science & Business Media.
  - 自行车模型和LQR理论基础

- **Levine, W. S.** (2010). *The Control Handbook (2nd Edition)*. CRC Press.
  - 控制理论详解

