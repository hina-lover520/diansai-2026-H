// #ifndef __PATH_RECORDER_H
// #define __PATH_RECORDER_H

// #include <stdint.h>
// #include <stdbool.h>

// /*===========================================================================
//  * 路径记录与回放系统 — 头文件
//  *===========================================================================
//  *
//  * 【功能概述】
//  *   1. 实时记录编码器 + 陀螺仪数据到 SRAM 环形缓冲区
//  *   2. 离线融合编码器里程计 + 陀螺仪航向 → 重建 (x, y, θ) 轨迹
//  *   3. 打滑检测：编码器和陀螺仪说的角度差太多 → 信陀螺仪
//  *   4. RTS 反向平滑：用整条轨迹的前后信息消除积分漂移
//  *   5. Flash 存储指南（注释形式，按需启用）
//  *
//  * 【硬件依赖】
//  *   编码器：两路 A/B 相正交编码，A 相上升沿外部中断计数（你的 Encoder.c）
//  *   陀螺仪：IMU660RB 的 gyro_z（绕 Z 轴的角速度，°/s）
//  *   SRAM：   MSPM0G3507 有 32KB，每条记录 12 字节
//  *            1000 条 = 12KB，安全可用
//  *
//  * 【坐标系约定】
//  *   机器人启动位置为原点 (0, 0)，正前方为 X 轴正方向
//  *   航向角 θ：X 轴逆时针旋转为正，单位：度
//  *        Y ↑
//  *          │
//  *          │   θ=90°
//  *          │   ╱
//  *          │  ╱
//  *          │ ╱  θ=0°  → X
//  *          │╱────────────
//  *          │
//  *===========================================================================*/

// /*===========================================================================
//  * 一、机器人物理参数（上线前一定改成你自己的值！）
//  *===========================================================================*/
// #define WHEEL_RADIUS_MM         32.5f       // 轮子半径（毫米）
// #define WHEEL_TRACK_MM          145.0f      // 两轮间距 / 轮距（毫米）
// #define ENCODER_PPR             11          // 编码器每圈脉冲数（A 相 1x 计数）
//                                             // 注意：你用单边沿触发，所以 1x 倍频
//                                             // 如果你的电机有减速比，乘在后面

// /* 衍生参数 —— 每个脉冲机器人走了多远 / 转了多少 */
// // 每个脉冲对应的轮子转动弧长（毫米）
// // = 轮子周长 / 每圈脉冲数 = 2πR / PPR
// #define MM_PER_PULSE            (2.0f * 3.14159265f * WHEEL_RADIUS_MM / ENCODER_PPR)

// // 每脉冲对应的机器人朝向变化（弧度）
// // 差速转弯：Δθ ≈ (ΔR - ΔL) / 轮距
// // 编码器差 1 个脉冲 → 轮距方向差 = 弧长差 / 轮距
// #define RAD_PER_PULSE_DIFF      (MM_PER_PULSE / WHEEL_TRACK_MM)

// /*===========================================================================
//  * 二、单条记录的数据结构（16 字节）
//  *===========================================================================*/
// typedef struct {
//     uint32_t timestamp_ms;      // 时间戳（从开始记录算起的毫秒数）
//     int32_t  encoder_left;      // 左轮编码器累计脉冲数（不是增量！丢数据也能恢复）
//     int32_t  encoder_right;     // 右轮编码器累计脉冲数
//     int16_t  gyro_z_raw;        // 陀螺仪 Z 轴角速度 × 10（如 -1234 = -123.4 °/s）
//     uint16_t flags;             // bit0: 打滑标志  bit1: 碰撞标志  bit2-15: 保留
// } PathRecord_t;                 // 16 字节/条

// /* flags 位定义 */
// #define PATH_FLAG_SLIP          0x0001      // 打滑检测触发
// #define PATH_FLAG_BUMP          0x0002      // 碰撞/急停
// #define PATH_FLAG_IMU_FAULT     0x0004      // IMU 数据异常

// /*===========================================================================
//  * 三、路径记录器主结构
//  *===========================================================================*/
// typedef struct {
//     PathRecord_t *buffer;       // 环形缓冲区指针
//     uint16_t     capacity;      // 缓冲区最大可存条数
//     uint16_t     head;          // 写指针（下一条写到这）
//     uint16_t     count;         // 已记录条数（满容量后不再增长）
//     bool         full;          // 缓冲区是否已满

//     uint32_t     start_time_ms; // 开始记录时刻的系统时间
//     uint32_t     last_record_ms;// 上一条记录的时间戳
//     uint32_t     interval_ms;   // 记录间隔（默认 10ms = 100Hz）

//     // 上一次记录时的编码器值（用于计算增量，检测跳变）
//     int32_t      last_enc_l;
//     int32_t      last_enc_r;
// } PathRecorder_t;

// /*===========================================================================
//  * 四、轨迹点（重建后的输出）
//  *===========================================================================*/
// typedef struct {
//     float x_mm;                 // X 坐标（毫米）
//     float y_mm;                 // Y 坐标（毫米）
//     float heading_deg;          // 航向角（度，逆时针为正）
//     float speed_mm_s;           // 瞬时线速度（毫米/秒）
//     float ang_rate_deg_s;       // 瞬时角速度（度/秒）
//     bool  slip_flag;            // 该点打滑标志
// } TrajectoryPoint_t;

// /*===========================================================================
//  * 五、轨迹重建器（离线处理用）
//  *===========================================================================*/
// typedef struct {
//     TrajectoryPoint_t *points;  // 轨迹点数组
//     uint16_t          count;    // 轨迹点数量
//     uint16_t          capacity; // 最大容量

//     // 融合参数
//     float alpha_heading;        // 编码器航向权重（0~1）
//                                 // 正常时：0.95 信编码器，0.05 信陀螺仪
//                                 // 打滑时：反过来
//     float slip_threshold_deg;   // 打滑检测阈值（度）
//                                 // |编码器角度增量 - 陀螺仪角度增量| > 阈值 → 打滑

//     // 统计信息
//     float total_distance_mm;    // 总行驶距离
//     float max_speed_mm_s;       // 最大速度
//     float max_ang_rate_deg_s;   // 最大角速度
//     int   slip_events;          // 打滑事件次数
// } TrajectoryReconstructor_t;

// /*===========================================================================
//  * 六、RTS 平滑器
//  *===========================================================================*/
// typedef struct {
//     float *P_fwd;               // 正向卡尔曼协方差（每个点一个值）
//     float *P_bwd;               // 反向协方差
//     float *x_fwd;               // 正向估计状态 [x, y, θ] 展开为 3×N 的数组
//     float *x_bwd;               // 反向估计
//     float *x_smoothed;          // 平滑后结果

//     float Q_pos;                // 位置过程噪声（编码器积分的不确定性）
//     float Q_hdg;                // 航向过程噪声（陀螺仪积分的不确定性）
//     float R_enc;                // 编码器测量噪声
//     float R_gyro;               // 陀螺仪测量噪声
// } RTSSmoother_t;

// /*===========================================================================
//  * 函数声明
//  *===========================================================================*/

// // --- 路径记录器 ---
// void PathRecorder_Init(PathRecorder_t *recorder,
//                        PathRecord_t *buffer, uint16_t capacity,
//                        uint32_t interval_ms);
// void PathRecorder_Start(PathRecorder_t *recorder, uint32_t current_time_ms);
// void PathRecorder_Record(PathRecorder_t *recorder,
//                          uint32_t current_time_ms,
//                          int32_t enc_left, int32_t enc_right,
//                          float gyro_z_dps);
// uint16_t PathRecorder_GetCount(PathRecorder_t *recorder);
// void PathRecorder_Reset(PathRecorder_t *recorder);

// // --- 轨迹重建 ---
// void Trajectory_InitReconstructor(TrajectoryReconstructor_t *recon,
//                                   TrajectoryPoint_t *points, uint16_t capacity);
// uint16_t Trajectory_Reconstruct(TrajectoryReconstructor_t *recon,
//                                 PathRecord_t *records, uint16_t record_count);
// int Trajectory_DetectSlip(TrajectoryReconstructor_t *recon,
//                           float delta_enc_deg, float delta_gyro_deg,
//                           float dt);

// // --- RTS 平滑 ---
// void RTS_Init(RTSSmoother_t *rts,
//               float *P_fwd_buf, float *x_fwd_buf,
//               float *P_bwd_buf, float *x_bwd_buf,
//               float *x_smoothed_buf,
//               uint16_t point_count,
//               float Q_pos, float Q_hdg, float R_enc, float R_gyro);
// void RTS_Smooth(RTSSmoother_t *rts,
//                 TrajectoryPoint_t *points, uint16_t count);

// // --- Flash 存储（MSPM0 平台）---
// // 数据存储区：Flash 末尾 32KB，避开固件代码
// // MSPM0G3507: 256KB Flash, 最后一个扇区起始偏移约 0x0003C000
// #define FLASH_DATA_BASE       0x0003C000UL
// #define FLASH_SECTOR_SIZE     1024U           // MSPM0G3507 扇区大小 1KB

// uint32_t PathRecorder_SaveToFlash(PathRecorder_t *recorder);
// uint16_t PathRecorder_LoadFromFlash(PathRecord_t *dest_buffer,
//                                     uint16_t max_count,
//                                     uint32_t *first_timestamp_out);
// void PathRecorder_EraseDataSector(uint32_t sector_count);

// #endif /* __PATH_RECORDER_H */
