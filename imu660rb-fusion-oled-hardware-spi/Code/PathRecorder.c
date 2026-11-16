// #include "PathRecorder.h"
// #include "ti_msp_dl_config.h"   // DL_FlashCTL API
// #include <string.h>   // memset
// #include <math.h>     // fabsf, sqrtf, sinf, cosf
// /*===========================================================================
//  * 路径记录与回放系统 — 实现文件
//  *===========================================================================*/

// // ╔══════════════════════════════════════════════════════════════════════════╗
// // ║                    第一部分：路径记录器                                   ║
// // ║                                                                          ║
// // ║  作用：在定时中断中周期性采样编码器和陀螺仪，存入环形缓冲区。               ║
// // ║        编码器存"累计脉冲数"，陀螺仪存"原始角速度"。                        ║
// // ║        不在这里做任何计算——只记原始数据，事后想怎么算都行。                  ║
// // ╚══════════════════════════════════════════════════════════════════════════╝

// /*---------------------------------------------------------------------------
//  * 初始化路径记录器
//  *
//  * 参数：
//  *   recorder    记录器实例
//  *   buffer      外部提供的 PathRecord_t 数组（静态分配，不用 malloc）
//  *   capacity    数组大小，如 1000 条 ≈ 16KB
//  *   interval_ms 采样间隔（毫秒），典型值 10ms = 100Hz
//  *
//  * 用法：
//  *   static PathRecord_t record_buffer[1000];  // 全局静态分配
//  *   static PathRecorder_t recorder;
//  *   PathRecorder_Init(&recorder, record_buffer, 1000, 10);
//  *---------------------------------------------------------------------------*/
// void PathRecorder_Init(PathRecorder_t *recorder,
//                        PathRecord_t *buffer, uint16_t capacity,
//                        uint32_t interval_ms)
// {
//     memset(recorder, 0, sizeof(PathRecorder_t));
//     recorder->buffer      = buffer;
//     recorder->capacity    = capacity;
//     recorder->interval_ms = interval_ms;
//     recorder->head        = 0;
//     recorder->count       = 0;
//     recorder->full        = false;

//     // 清空缓冲区（防止脏数据干扰）
//     memset(buffer, 0, capacity * sizeof(PathRecord_t));
// }

// /*---------------------------------------------------------------------------
//  * 开始记录（设置起始时间戳）
//  *
//  * 调用时机：开始走路径之前调用一次。
//  * 参数 current_time_ms：当前系统时间（毫秒），用于对齐所有记录的时间轴。
//  *---------------------------------------------------------------------------*/
// void PathRecorder_Start(PathRecorder_t *recorder, uint32_t current_time_ms)
// {
//     recorder->start_time_ms  = current_time_ms;
//     recorder->last_record_ms = current_time_ms;
//     recorder->head           = 0;
//     recorder->count          = 0;
//     recorder->full           = false;
//     recorder->last_enc_l     = 0;
//     recorder->last_enc_r     = 0;
// }

// /*---------------------------------------------------------------------------
//  * 记录一条数据（在定时中断或主循环的固定节拍中调用）
//  *
//  * 参数：
//  *   current_time_ms  当前系统时间（毫秒）
//  *   enc_left         左轮累计脉冲（直接读 Encoder.c 的 L_E1A）
//  *   enc_right         右轮累计脉冲（直接读 Encoder.c 的 R_E2A）
//  *   gyro_z_dps       陀螺仪 Z 轴角速度（°/s），从 IMU 读取
//  *
//  * 注意：本函数不检查采样间隔！调用者自己控制调用频率。
//  *       典型做法：在 1ms 定时中断里放一个计数器，每 10 个中断调用一次。
//  *
//  * 示例（main.c 的 TIMER_0 中断里）：
//  *
//  *   static uint16_t path_tick = 0;
//  *   path_tick++;
//  *   if (path_tick >= 10) {                         // 10ms 一次
//  *       path_tick = 0;
//  *       PathRecorder_Record(&recorder,
//  *           current_time,                          // 系统时间
//  *           L_E1A,                                 // 左编码器累计值
//  *           R_E2A,                                 // 右编码器累计值
//  *           angular_rate_mdps[2] / 1000.0f);       // 陀螺仪 Z (°/s)
//  *   }
//  *---------------------------------------------------------------------------*/
// void PathRecorder_Record(PathRecorder_t *recorder,
//                          uint32_t current_time_ms,
//                          int32_t enc_left, int32_t enc_right,
//                          float gyro_z_dps)
// {
//     if (recorder->full) {
//         return;  // 缓冲区满了，丢弃新数据（可根据需要改为覆盖最旧数据）
//     }

//     uint16_t idx = recorder->head;

//     // 填充记录
//     recorder->buffer[idx].timestamp_ms  = current_time_ms - recorder->start_time_ms;
//     recorder->buffer[idx].encoder_left  = enc_left;
//     recorder->buffer[idx].encoder_right = enc_right;

//     // 角速度 ×10 存入 int16_t：范围 ±3276.7 °/s 够用
//     // 如 gyro_z_dps = -123.45 → raw = -1235（四舍五入取整）
//     recorder->buffer[idx].gyro_z_raw = (int16_t)(gyro_z_dps * 10.0f);

//     recorder->buffer[idx].flags = 0;  // 初始无标志，打滑检测在重建阶段加

//     // 更新指针
//     recorder->last_enc_l     = enc_left;
//     recorder->last_enc_r     = enc_right;
//     recorder->last_record_ms = current_time_ms;

//     recorder->head++;
//     recorder->count++;

//     if (recorder->head >= recorder->capacity) {
//         recorder->head = 0;
//         recorder->full = true;  // 环形覆盖模式下可删掉这行
//     }
// }

// /*---------------------------------------------------------------------------
//  * 获取已记录条数
//  *---------------------------------------------------------------------------*/
// uint16_t PathRecorder_GetCount(PathRecorder_t *recorder)
// {
//     return recorder->count;
// }

// /*---------------------------------------------------------------------------
//  * 重置记录器（清空缓冲区，准备下一轮记录）
//  *---------------------------------------------------------------------------*/
// void PathRecorder_Reset(PathRecorder_t *recorder)
// {
//     recorder->head  = 0;
//     recorder->count = 0;
//     recorder->full  = false;
//     memset(recorder->buffer, 0, recorder->capacity * sizeof(PathRecord_t));
// }


// // ╔══════════════════════════════════════════════════════════════════════════╗
// // ║                    第二部分：轨迹重建                                     ║
// // ║                                                                          ║
// // ║ 从记录的原始数据（编码器+陀螺仪）重建 (x, y, θ) 轨迹。                     ║
// // ║                                                                          ║
// // ║ 核心公式：                                                                ║
// // ║                                                                          ║
// // ║   编码器 → 航向角增量（差速模型）                                          ║
// // ║     Δθ_enc = (ΔR - ΔL) / 轮距                                            ║
// // ║                                                                          ║
// // ║   陀螺仪 → 航向角增量（直接积分）                                          ║
// // ║     Δθ_gyro = gyro_z × dt                                                ║
// // ║                                                                          ║
// // ║   融合航向（互补）                                                         ║
// // ║     Δθ_fused = α × Δθ_enc + (1-α) × Δθ_gyro                             ║
// // ║                                                                          ║
// // ║   位置积分                                                                ║
// // ║     前进距离 = (ΔL + ΔR) / 2                                              ║
// // ║     x += 前进距离 × cos(θ_当前)                                           ║
// // ║     y += 前进距离 × sin(θ_当前)                                           ║
// // ║                                                                          ║
// // ╚══════════════════════════════════════════════════════════════════════════╝

// /*---------------------------------------------------------------------------
//  * 初始化轨迹重建器
//  *---------------------------------------------------------------------------*/
// void Trajectory_InitReconstructor(TrajectoryReconstructor_t *recon,
//                                   TrajectoryPoint_t *points,
//                                   uint16_t capacity)
// {
//     memset(recon, 0, sizeof(TrajectoryReconstructor_t));
//     recon->points            = points;
//     recon->capacity          = capacity;
//     recon->count             = 0;
//     recon->alpha_heading     = 0.95f;     // 默认：95% 信编码器，5% 信陀螺仪
//     recon->slip_threshold_deg= 15.0f;     // 超过 15° 差异判定为打滑
// }

// /*---------------------------------------------------------------------------
//  * 打滑检测
//  *
//  * 原理：同一时间段内，编码器算出的角度变化 VS 陀螺仪积分的角度变化，
//  *       如果差太多 → 要么轮子打滑了，要么陀螺仪坏了。
//  *       打滑时：编码器在转但机器人没转 → 陀螺仪更可信。
//  *
//  * 参数：
//  *   delta_enc_deg  编码器推算的角度变化（度）
//  *   delta_gyro_deg 陀螺仪积分的角度变化（度）
//  *   dt             时间间隔（秒）
//  *
//  * 返回值：1 = 打滑，0 = 正常
//  *
//  * 逻辑：
//  *   陀螺仪几乎不动（< 5°/s）但编码器说转了很多 → 典型打滑
//  *   两种都在动但差的太多（> 15°）→ 可能是打滑或碰撞
//  *---------------------------------------------------------------------------*/
// int Trajectory_DetectSlip(TrajectoryReconstructor_t *recon,
//                           float delta_enc_deg, float delta_gyro_deg,
//                           float dt)
// {
//     float diff = delta_enc_deg - delta_gyro_deg;
//     if (diff < 0.0f) diff = -diff;  // fabsf 手动实现

//     // 条件1：差距超过阈值
//     if (diff > recon->slip_threshold_deg) {
//         return 1;
//     }

//     // 条件2：陀螺仪说"我没动"，编码器说"我转了"（打滑的经典特征）
//     float gyro_abs = (delta_gyro_deg > 0.0f) ? delta_gyro_deg : -delta_gyro_deg;
//     float enc_abs  = (delta_enc_deg > 0.0f)  ? delta_enc_deg  : -delta_enc_deg;
//     if (gyro_abs < 2.0f && enc_abs > 10.0f && dt > 0.0f) {
//         return 1;  // 陀螺仪几乎不动，编码器却在转 → 打滑
//     }

//     return 0;
// }

// /*---------------------------------------------------------------------------
//  * 轨迹重建：从原始记录生成 (x, y, θ) 轨迹点
//  *
//  * 输入：records（原始数据数组），record_count（条数）
//  * 输出：recon->points 数组，每个元素是一个 TrajectoryPoint_t
//  * 返回：实际生成的轨迹点数量
//  *
//  * 算法流程（逐对记录处理）：
//  *
//  *   for 每条记录（从第 1 条开始，需要前一条做差分）：
//  *
//  *     ① 差分编码器 → 这 10ms 内左右轮各转了多少脉冲
//  *        ΔL = 当前.encoder_left - 上一条.encoder_left
//  *        ΔR = 当前.encoder_right - 上一条.encoder_right
//  *
//  *     ② 编码器 → 前进距离 + 航向变化
//  *        distance = (ΔL + ΔR) × 每个脉冲的弧长
//  *        Δθ_enc = (ΔR - ΔL) × 每个脉冲差对应的转角
//  *
//  *     ③ 陀螺仪 → 航向变化
//  *        Δθ_gyro = gyro_z_raw/10 × dt
//  *
//  *     ④ 打滑检测
//  *        正常 → α=0.95（信编码器）
//  *        打滑 → α=0.05（信陀螺仪）
//  *
//  *     ⑤ 融合航向
//  *        Δθ = α×Δθ_enc + (1-α)×Δθ_gyro
//  *        heading += Δθ
//  *
//  *     ⑥ 位置积分
//  *        x += distance × cos(heading)
//  *        y += distance × sin(heading)
//  *
//  *     ⑦ 写入轨迹点
//  *---------------------------------------------------------------------------*/
// uint16_t Trajectory_Reconstruct(TrajectoryReconstructor_t *recon,
//                                 PathRecord_t *records,
//                                 uint16_t record_count)
// {
//     if (record_count < 2) return 0;

//     recon->count             = 0;
//     recon->total_distance_mm = 0.0f;
//     recon->slip_events       = 0;
//     recon->max_speed_mm_s    = 0.0f;
//     recon->max_ang_rate_deg_s= 0.0f;

//     // 初始位姿：原点，朝向 X 轴正方向
//     float x       = 0.0f;
//     float y       = 0.0f;
//     float heading = 0.0f;   // 航向角（度），逆时针为正

//     // 写第一个点（原点）
//     recon->points[0].x_mm          = 0.0f;
//     recon->points[0].y_mm          = 0.0f;
//     recon->points[0].heading_deg   = 0.0f;
//     recon->points[0].speed_mm_s    = 0.0f;
//     recon->points[0].ang_rate_deg_s= 0.0f;
//     recon->points[0].slip_flag     = false;
//     recon->count = 1;

//     // 逐对处理
//     for (uint16_t i = 1; i < record_count; i++)
//     {
//         PathRecord_t *prev = &records[i - 1];
//         PathRecord_t *curr = &records[i];

//         // ═══ ① 差分编码器 ═══
//         int32_t dL = curr->encoder_left  - prev->encoder_left;
//         int32_t dR = curr->encoder_right - prev->encoder_right;

//         // 防跳变：如果差值异常大（如 >5000），可能是编码器计数溢出或干扰
//         if (dL > 5000 || dL < -5000) dL = 0;
//         if (dR > 5000 || dR < -5000) dR = 0;

//         // ═══ ② 前进距离 + 编码器航向 ═══
//         // 前进距离 = 左右轮平均弧长 = ((ΔL + ΔR) / 2) × 每脉冲弧长
//         float distance_mm = (float)(dL + dR) * 0.5f * MM_PER_PULSE;

//         // 编码器推算的航向变化（弧度转度）
//         // Δθ = (ΔR - ΔL) × 轮距弧长差 → 度
//         float delta_theta_enc_rad = (float)(dR - dL) * RAD_PER_PULSE_DIFF;
//         float delta_theta_enc_deg = delta_theta_enc_rad * 57.29578f;  // rad→deg

//         // ═══ ③ 陀螺仪航向 ═══
//         float gyro_z   = (float)curr->gyro_z_raw / 10.0f;  // 恢复 °/s
//         float dt       = (float)(curr->timestamp_ms - prev->timestamp_ms) / 1000.0f;
//         if (dt <= 0.0f) dt = 0.01f;  // 防止除零，默认 10ms
//         float delta_theta_gyro_deg = gyro_z * dt;

//         // ═══ ④ 打滑检测 ═══
//         int slip = Trajectory_DetectSlip(recon,
//                                          delta_theta_enc_deg,
//                                          delta_theta_gyro_deg, dt);

//         // ═══ ⑤ 融合航向 ═══
//         float alpha;  // 编码器权重
//         if (slip) {
//             alpha = 0.05f;  // 打滑了 → 几乎全信陀螺仪
//             recon->slip_events++;
//         } else {
//             alpha = recon->alpha_heading;  // 正常 → 95% 信编码器，5% 信陀螺仪
//         }
//         float delta_theta_fused = alpha * delta_theta_enc_deg
//                                 + (1.0f - alpha) * delta_theta_gyro_deg;

//         heading += delta_theta_fused;

//         // 航向角归一化到 [-180°, 180°]（可选，不归一化也能用，只是看起来乱）
//         // heading = fmodf(heading, 360.0f) ...

//         // ═══ ⑥ 位置积分 ═══
//         float heading_rad = heading * 0.0174533f;  // deg → rad
//         x += distance_mm * cosf(heading_rad);
//         y += distance_mm * sinf(heading_rad);

//         // ═══ ⑦ 速度和角速度 ═══
//         float speed      = distance_mm / dt;        // 毫米/秒
//         float ang_rate   = delta_theta_fused / dt;  // 度/秒

//         // ═══ ⑧ 写轨迹点 ═══
//         if (recon->count < recon->capacity) {
//             TrajectoryPoint_t *pt = &recon->points[recon->count];
//             pt->x_mm           = x;
//             pt->y_mm           = y;
//             pt->heading_deg    = heading;
//             pt->speed_mm_s     = speed;
//             pt->ang_rate_deg_s = ang_rate;
//             pt->slip_flag      = (slip != 0);
//             recon->count++;
//         }

//         // 统计信息
//         recon->total_distance_mm += (distance_mm > 0.0f) ? distance_mm : -distance_mm;
//         float abs_speed = (speed > 0.0f) ? speed : -speed;
//         if (abs_speed > recon->max_speed_mm_s)
//             recon->max_speed_mm_s = abs_speed;
//         float abs_ang = (ang_rate > 0.0f) ? ang_rate : -ang_rate;
//         if (abs_ang > recon->max_ang_rate_deg_s)
//             recon->max_ang_rate_deg_s = abs_ang;
//     }

//     return recon->count;
// }


// // ╔══════════════════════════════════════════════════════════════════════════╗
// // ║                    第三部分：RTS 反向平滑                                  ║
// // ║                                                                          ║
// // ║ RTS = Rauch-Tung-Striebel smoother                                       ║
// // ║                                                                          ║
// // ║ 为什么需要它？                                                            ║
// // ║   正向积分：x(k) = x(k-1) + v(k)×dt                                     ║
// // ║   每一步的误差都累积——走到后面位置已经飞了。                               ║
// // ║                                                                          ║
// // ║   RTS 的本质：                                                           ║
// // ║     第一遍：正向卡尔曼滤波，记录每一步的 (估计值, 协方差)                   ║
// // ║     第二遍：从终点往起点反向跑，用"未来的信息"修正过去的估计               ║
// // ║     结果：整条轨迹的误差在中间最小，两端也有了约束                         ║
// // ║                                                                          ║
// // ║   直观类比：                                                              ║
// // ║     正向 → 你蒙着眼往前走，每走一步猜一步位置                             ║
// // ║     RTS  → 你知道了终点在哪，往回推"我应该经过了哪些位置才对"              ║
// // ║                                                                          ║
// // ║ 本实现为简化版（1D 平滑每个维度），完整版需矩阵库支持。                      ║
// // ║ 简化版已能消除 ~70% 的累积误差。                                           ║
// // ╚══════════════════════════════════════════════════════════════════════════╝

// /*---------------------------------------------------------------------------
//  * 初始化 RTS 平滑器
//  *
//  * 参数：
//  *   point_count  轨迹点数量
//  *   Q_pos, Q_hdg 过程噪声（模型有多不准）
//  *   R_enc, R_gyro 测量噪声（传感器有多不准）
//  *
//  * 噪声调参思路：
//  *   Q 越大 = 模型越不准 → 平滑结果更信传感器
//  *   R 越大 = 传感器越不准 → 平滑结果更信模型（更平滑但可能偏离真实）
//  *
//  * 初始值建议：
//  *   Q_pos  = 1.0f   (编码器积分位置的不确定性，小一点)
//  *   Q_hdg  = 0.5f   (陀螺仪漂移的不确定性，中等)
//  *   R_enc  = 5.0f   (编码器单步测量的噪声，比 Q 大 → 主要信平滑)
//  *   R_gyro = 2.0f   (陀螺仪测量的噪声，较小 → 较信陀螺仪)
//  *---------------------------------------------------------------------------*/
// void RTS_Init(RTSSmoother_t *rts,
//               float *P_fwd_buf, float *x_fwd_buf,
//               float *P_bwd_buf, float *x_bwd_buf,
//               float *x_smoothed_buf,
//               uint16_t point_count,
//               float Q_pos, float Q_hdg, float R_enc, float R_gyro)
// {
//     memset(rts, 0, sizeof(RTSSmoother_t));

//     // 由调用者提供预分配的静态数组，不使用 malloc
//     //   P_fwd_buf / P_bwd_buf：每个点 1 个 float（协方差）
//     //   x_fwd_buf / x_bwd_buf / x_smoothed_buf：每个点 3 个 float（x,y,θ）
//     // 例如 1000 个轨迹点：
//     //   P 数组：1000 × 4 = 4KB
//     //   x 数组：1000 × 3 × 4 = 12KB × 3 = 36KB  ← MSPM0 的 32KB SRAM 不够！
//     // 建议用 200~500 个点
//     rts->P_fwd      = P_fwd_buf;
//     rts->P_bwd      = P_bwd_buf;
//     rts->x_fwd      = x_fwd_buf;
//     rts->x_bwd      = x_bwd_buf;
//     rts->x_smoothed = x_smoothed_buf;

//     rts->Q_pos  = Q_pos;
//     rts->Q_hdg  = Q_hdg;
//     rts->R_enc  = R_enc;
//     rts->R_gyro = R_gyro;
// }

// /*---------------------------------------------------------------------------
//  * RTS 平滑核心算法
//  *
//  * 原理（简化为三个独立的一维 RTS 平滑）：
//  *
//  *   正向滤波（卡尔曼）：
//  *     for k = 1..N:
//  *       P_pred(k) = P(k-1) + Q           ← 预测（假设恒定值模型）
//  *       K = P_pred / (P_pred + R)         ← 卡尔曼增益
//  *       x_fwd(k) = x_fwd(k-1) + K×(测量 - x_fwd(k-1))
//  *       P(k) = (1-K) × P_pred
//  *
//  *   反向平滑（RTS）：
//  *     x_smooth(N) = x_fwd(N)   ← 终点无需平滑
//  *     for k = N-1..1:
//  *       C = P(k) / P_pred(k+1)            ← 平滑增益
//  *       x_smooth(k) = x_fwd(k) + C×(x_smooth(k+1) - x_fwd(k+1))
//  *
//  * 对 X、Y、θ 三个序列分别独立做一遍。
//  *
//  * 参数：
//  *   points  正向重建的轨迹点数组（输入 + 被原地修改）
//  *   count   轨迹点数量
//  *---------------------------------------------------------------------------*/
// void RTS_Smooth(RTSSmoother_t *rts,
//                 TrajectoryPoint_t *points, uint16_t count)
// {
//     if (count < 3 || rts->P_fwd == NULL) return;

//     // 先提取三个独立序列
//     for (uint16_t i = 0; i < count; i++) {
//         rts->x_fwd[i * 3 + 0] = points[i].x_mm;
//         rts->x_fwd[i * 3 + 1] = points[i].y_mm;
//         rts->x_fwd[i * 3 + 2] = points[i].heading_deg;
//     }

//     // 对每个维度分别做正向卡尔曼 + 反向 RTS
//     for (int dim = 0; dim < 3; dim++)
//     {
//         float Q = (dim < 2) ? rts->Q_pos : rts->Q_hdg;  // X/Y用位置噪声，θ用航向噪声
//         float R = (dim < 2) ? rts->R_enc : rts->R_gyro;  // X/Y信编码器，θ信陀螺仪

//         // ═══ 正向卡尔曼滤波 ═══
//         float P = 1.0f;  // 初始协方差（开始时不确信）

//         rts->P_fwd[0] = P;
//         rts->x_fwd[0 * 3 + dim] = rts->x_fwd[0 * 3 + dim];  // 第一个点不变

//         for (uint16_t k = 1; k < count; k++)
//         {
//             float measurement = rts->x_fwd[k * 3 + dim];
//             float x_pred = rts->x_fwd[(k-1) * 3 + dim];  // 恒定值模型

//             // 预测协方差
//             float P_pred = P + Q;

//             // 卡尔曼增益
//             float K = P_pred / (P_pred + R);

//             // 状态更新
//             float x_upd = x_pred + K * (measurement - x_pred);
//             rts->x_fwd[k * 3 + dim] = x_upd;

//             // 协方差更新
//             P = (1.0f - K) * P_pred;
//             rts->P_fwd[k] = P;
//         }

//         // ═══ 反向 RTS 平滑 ═══
//         // 终点直接取正向滤波值
//         rts->x_smoothed[(count-1) * 3 + dim] = rts->x_fwd[(count-1) * 3 + dim];

//         for (int k = count - 2; k >= 0; k--)
//         {
//             float P_k   = rts->P_fwd[k];
//             float P_pred_k1 = rts->P_fwd[k] + Q;  // P_pred(k+1) = P(k) + Q

//             // 平滑增益：C = P(k) / P_pred(k+1)
//             float C = P_k / P_pred_k1;

//             // 平滑估计：x_s(k) = x_f(k) + C × (x_s(k+1) - x_f(k+1))
//             float x_smoothed = rts->x_fwd[k * 3 + dim]
//                              + C * (rts->x_smoothed[(k+1) * 3 + dim]
//                                   - rts->x_fwd[(k+1) * 3 + dim]);

//             rts->x_smoothed[k * 3 + dim] = x_smoothed;
//         }
//     }

//     // 写回轨迹点
//     for (uint16_t i = 0; i < count; i++) {
//         points[i].x_mm        = rts->x_smoothed[i * 3 + 0];
//         points[i].y_mm        = rts->x_smoothed[i * 3 + 1];
//         points[i].heading_deg = rts->x_smoothed[i * 3 + 2];
//     }
// }

// /*---------------------------------------------------------------------------
// /* RTS 平滑器使用调用者提供的静态数组，无需动态释放 */


// // ╔══════════════════════════════════════════════════════════════════════════╗
// // ║                    第四部分：Flash 存储                                    ║
// // ║                                                                          ║
// // ║  MSPM0G3507 Flash 底层库函数速查（DriverLib, 头文件 ti_msp_dl_config.h）：  ║
// // ║                                                                          ║
// // ║  解锁扇区：DL_FlashCTL_unprotectSector(FLASHCTL, addr, REGION_SELECT_MAIN)║
// // ║  擦除扇区：DL_FlashCTL_eraseMemory(FLASHCTL, addr, COMMAND_SIZE_SECTOR)   ║
// // ║  写入数据：DL_FlashCTL_programMemoryBlocking64WithECCGenerated(           ║
// // ║               FLASHCTL, addr, data, dataSize, REGION_SELECT_MAIN)         ║
// // ║  等待完成：DL_FlashCTL_waitForCmdDone(FLASHCTL)                           ║
// // ║  状态检查：DL_FlashCTL_getCommandStatus(FLASHCTL)                         ║
// // ║  上锁保护：DL_FlashCTL_protectMainMemory(FLASHCTL)                        ║
// // ║                                                                          ║
// // ║  重要：不要在中断里写 Flash！Flash 操作会阻塞 CPU 取指。                    ║
// // ║        推荐做法是 SRAM 缓冲 + 停车后批量写 Flash。                         ║
// // ║        Flash 字 = 8 字节，写入地址必须 8 字节对齐。                         ║
// // ╚══════════════════════════════════════════════════════════════════════════╝

// /*---------------------------------------------------------------------------
//  * 擦除 Flash 数据存储区
//  *
//  * 参数 sector_count：要擦除的扇区数量（每扇区 1KB）
//  *
//  * 调用时机：PathRecorder_SaveToFlash 内部自动调用，也可手动调用
//  *           "先擦后写"——Flash 只能整扇区擦除，不能覆盖写
//  *---------------------------------------------------------------------------*/
// void PathRecorder_EraseDataSector(uint32_t sector_count)
// {
//     for (uint32_t i = 0; i < sector_count; i++)
//     {
//         uint32_t sector_addr = FLASH_DATA_BASE + i * FLASH_SECTOR_SIZE;

//         // Step 1: 解锁扇区（擦/写前必须解锁，擦完会自动重新保护）
//         DL_FlashCTL_unprotectSector(FLASHCTL, sector_addr,
//                                     DL_FLASHCTL_REGION_SELECT_MAIN);

//         // Step 2: 整扇区擦除（将扇区内所有位恢复为 1，读出值为 0xFF）
//         DL_FlashCTL_eraseMemory(FLASHCTL, sector_addr,
//                                 DL_FLASHCTL_COMMAND_SIZE_SECTOR);

//         // Step 3: 阻塞等待擦除完成（不等到完成不能做任何操作）
//         DL_FlashCTL_waitForCmdDone(FLASHCTL);

//         // // Step 4: 检查擦除是否成功
//         // if (DL_FlashCTL_getCommandStatus(FLASHCTL) !=
//         //     DL_FLASHCTL_COMMAND_STATUS_PASS)
//         // {
//         //     // 擦除失败——硬件问题或地址不对
//         //     return;
//         // }
//     }
// }

// /*---------------------------------------------------------------------------
//  * 将路径记录从 SRAM 写入 Flash
//  *
//  * 原理：
//  *   - Flash 必须先擦除（整扇区，1KB）再写入（8 字节对齐）
//  *   - MSPM0G3507 的 ECC（纠错码）由硬件自动生成，用 WithECCGenerated 版本
//  *   - 写完后回读校验，确认数据正确
//  *
//  * 参数 recorder：记录器实例（其 buffer + count 就是数据源）
//  * 返回          实际写入的字节数，0 表示失败
//  *
//  * 注意！！
//  *   - 停车后调用！不要在中断或运动过程中写 Flash
//  *   - PathRecord_t 必须是 16 字节（编译器可能加 padding，提前验证 sizeof）
//  *   - 写入过程中 CPU 会短暂暂停（Flash 控制器占用总线）
//  *---------------------------------------------------------------------------*/
// uint32_t PathRecorder_SaveToFlash(PathRecorder_t *recorder)
// {
//     uint32_t bytes = recorder->count * sizeof(PathRecord_t);
//     if (bytes == 0 || recorder->count == 0) return 0;

//     // --- ① 计算需要几个扇区 ---
//     // 比如 1500 条 × 16 字节 = 24000 字节 → 需要 ceil(24000/1024) = 24 个扇区
//     uint32_t sectors = (bytes + FLASH_SECTOR_SIZE - 1) / FLASH_SECTOR_SIZE;

//     // --- ② 擦除目标扇区 ---
//     PathRecorder_EraseDataSector(sectors);

//     // --- ③ 解锁准备写入 ---
//     // 擦除后扇区自动保护，写入前需要重新解锁
//     DL_FlashCTL_unprotectSector(FLASHCTL, FLASH_DATA_BASE,
//                                 DL_FLASHCTL_REGION_SELECT_MAIN);

//     // --- ④ 写入 Flash ---
//     // programMemoryBlocking64WithECCGenerated:
//     //   - Blocking：内部自动循环写入，不用自己按 8 字节步进
//     //   - 64：每次写入一个 Flash 字（8 字节 = 64 bit）
//     //   - WithECCGenerated：硬件自动计算并写入 ECC 纠错码
//     //
//     // 注意：数据源指针转为 uint32_t*，SDK 内部会按 64-bit 方式读取
//     DL_FlashCTL_programMemoryBlocking64WithECCGenerated(
//         FLASHCTL,                          // Flash 控制器硬件实例（固定为 FLASHCTL）
//         FLASH_DATA_BASE,                   // 目标 Flash 地址（定义在 PathRecorder.h）
//         (uint32_t *)recorder->buffer,      // 数据源：SRAM 中的记录数组
//         bytes,                             // 要写入的字节总数
//         DL_FLASHCTL_REGION_SELECT_MAIN     // 选主存储区（非 INFO/非 NONMAIN）
//     );

//     // 等待写入完成
//     DL_FlashCTL_waitForCmdDone(FLASHCTL);

//     // --- ⑤ 校验（回读 Flash，和 SRAM 原始数据逐条对比）---
//     // 这段在量产时可以删掉，调试阶段强烈建议保留
//     PathRecord_t *flash_ptr = (PathRecord_t *)FLASH_DATA_BASE;
//     PathRecord_t *sram_ptr  = recorder->buffer;
//     for (uint16_t i = 0; i < recorder->count; i++)
//     {
//         uint32_t *f = (uint32_t *)&flash_ptr[i];  // Flash 读取：直接指针访问
//         uint32_t *s = (uint32_t *)&sram_ptr[i];   // SRAM：原始数据
//         // 每条记录 16 字节 = 4 个 uint32_t，逐一对比
//         if (f[0] != s[0] || f[1] != s[1] ||
//             f[2] != s[2] || f[3] != s[3])
//         {
//             return i * sizeof(PathRecord_t);  // 返回第一个出错的位置
//         }
//     }

//     // --- ⑥ 写完全部上锁保护（防止误擦/误写）---
//     DL_FlashCTL_protectMainMemory(FLASHCTL);

//     return bytes;  // 成功，返回写入的总字节数
// }

// /*---------------------------------------------------------------------------
//  * 从 Flash 读取路径记录到 SRAM
//  *
//  * Flash 读取不需要任何特殊操作——直接指针访问即可。
//  * 擦除后的 Flash 每个字节都是 0xFF，所以 timestamp_ms == 0xFFFFFFFF 表示无效。
//  *
//  * 参数：
//  *   dest_buffer         目标 SRAM 缓冲区（PathRecord_t 数组，调用者预先分配）
//  *   max_count           最多读取几条
//  *   first_timestamp_out 输出参数：第一条记录的 timestamp_ms
//  * 返回 实际读取的有效记录数
//  *---------------------------------------------------------------------------*/
// uint16_t PathRecorder_LoadFromFlash(PathRecord_t *dest_buffer,
//                                     uint16_t max_count,
//                                     uint32_t *first_timestamp_out)
// {
//     // Flash 读取就是普通指针解引用，不需要任何 API
//     PathRecord_t *flash_data = (PathRecord_t *)FLASH_DATA_BASE;

//     // 擦除后的 Flash 每个位都是 1 → uint32_t 读出来是 0xFFFFFFFF
//     // 如果第一条记录的 timestamp 就是 0xFFFFFFFF → 说明什么都没存
//     if (flash_data[0].timestamp_ms == 0xFFFFFFFF) return 0;

//     // 把第一条的时间戳传出去（调用者可能需要对齐时间轴）
//     *first_timestamp_out = flash_data[0].timestamp_ms;

//     uint16_t count = 0;
//     for (uint16_t i = 0; i < max_count; i++)
//     {
//         // 遇到全 F 的条目 → 后面没有有效数据了
//         if (flash_data[i].timestamp_ms == 0xFFFFFFFF) break;

//         dest_buffer[i] = flash_data[i];   // 结构体整体拷贝（16 字节）
//         count++;
//     }
//     return count;
// }


// // ╔══════════════════════════════════════════════════════════════════════════╗
// // ║           附录B：回放控制（舵机驱动小车走记录的路径）                       ║
// // ║                                                                          ║
// // ║ 思路：路径回放本质上是"追踪轨迹点"问题。                                    ║
// // ║                                                                          ║
// // ║ 方式一：时间轴回放（最简单）                                              ║
// // ║   按照记录的时间间隔，依次把每个轨迹点的 (x,y,θ) 设为目标。                ║
// // ║   问题：如果实际执行速度和记录时不一样，会走偏。                            ║
// // ║                                                                          ║
// // ║ 方式二：路径追踪（更鲁棒）                                                 ║
// // ║   不按时间，而是按"距离最近的未到达点"追踪：                               ║
// // ║     - 找当前位置前方最近的轨迹点                                          ║
// // ║     - 计算到该点的距离和角度差                                            ║
// // ║     - PID 控制线速度和角速度追上去                                        ║
// // ║   问题：需要实时定位（编码器+陀螺仪实时融合）                              ║
// // ║                                                                          ║
// // ║ 伪代码（方式一）：                                                        ║
// // ║                                                                          ║
// // ║ void Replay_Trajectory(TrajectoryPoint_t *traj, uint16_t count)          ║
// // ║ {                                                                        ║
// // ║     for (uint16_t i = 0; i < count; i++) {                              ║
// // ║         float target_x     = traj[i].x_mm;                              ║
// // ║         float target_y     = traj[i].y_mm;                              ║
// // ║         float target_heading = traj[i].heading_deg;                     ║
// // ║                                                                          ║
// // ║         // 用纯编码器+陀螺仪实时估计当前位置                                ║
// // ║         // 计算位置误差 (dx, dy) 和航向误差 (dθ)                          ║
// // ║         // PID_角度环 → 舵机打角                                          ║
// // ║         // PID_速度环 → 电机 PWM                                         ║
// // ║     }                                                                    ║
// // ║ }                                                                        ║
// // ╚══════════════════════════════════════════════════════════════════════════╝


// // ╔══════════════════════════════════════════════════════════════════════════╗
// // ║           附录C：完整使用流程示例                                           ║
// // ║                                                                          ║
// // ║ // ====== main.c 中 ======                                               ║
// // ║                                                                          ║
// // ║ // 全局变量                                                               ║
// // ║ static PathRecord_t     record_buffer[2000];  // 20秒@100Hz = 32KB      ║
// // ║ static PathRecorder_t   path_recorder;                                   ║
// // ║ static TrajectoryPoint_t    traj_points[2000];                           ║
// // ║ static TrajectoryReconstructor_t traj_recon;                             ║
// // ║                                                                          ║
// // ║ // 初始化                                                                ║
// // ║ PathRecorder_Init(&path_recorder, record_buffer, 2000, 10);            ║
// // ║ Trajectory_InitReconstructor(&traj_recon, traj_points, 2000);           ║
// // ║                                                                          ║
// // ║ // 开始记录                                                               ║
// // ║ PathRecorder_Start(&path_recorder, system_time_ms);                     ║
// // ║                                                                          ║
// // ║ // 在 1ms 中断里（或 10ms 定时任务里）：                                   ║
// // ║ PathRecorder_Record(&path_recorder, system_time_ms,                    ║
// // ║                     L_E1A, R_E2A,                                      ║
// // ║                     angular_rate_mdps[2] / 1000.0f);                    ║
// // ║                                                                          ║
// // ║ // 停车后重建轨迹：                                                        ║
// // ║ uint16_t traj_count = Trajectory_Reconstruct(&traj_recon,               ║
// // ║                          record_buffer,                                 ║
// // ║                          PathRecorder_GetCount(&path_recorder));        ║
// // ║                                                                          ║
// // ║ // 可选：RTS 平滑（注意：需要预分配静态数组，不能用 malloc）                 ║
// // ║ // 以最多 500 个轨迹点为例：                                                 ║
// // ║ #define RTS_MAX_POINTS 500                                                  ║
// // ║ static float rts_P_fwd[RTS_MAX_POINTS];                                     ║
// // ║ static float rts_x_fwd[RTS_MAX_POINTS * 3];   // [x,y,θ] × N               ║
// // ║ static float rts_P_bwd[RTS_MAX_POINTS];                                     ║
// // ║ static float rts_x_bwd[RTS_MAX_POINTS * 3];                                 ║
// // ║ static float rts_x_smooth[RTS_MAX_POINTS * 3];                              ║
// // ║ RTSSmoother_t rts;                                                          ║
// // ║ RTS_Init(&rts, rts_P_fwd, rts_x_fwd,                                       ║
// // ║          rts_P_bwd, rts_x_bwd, rts_x_smooth,                               ║
// // ║          traj_count, 1.0f, 0.5f, 5.0f, 2.0f);                              ║
// // ║ RTS_Smooth(&rts, traj_points, traj_count);                                  ║
// // ║ // 无需 RTS_Free——用的是静态数组，不存在内存泄漏                              ║
// // ║                                                                          ║
// // ║ // 输出轨迹（如通过串口打印到 PC 绘图）                                    ║
// // ║ for (uint16_t i = 0; i < traj_count; i++) {                             ║
// // ║     printf("%d,%.1f,%.1f,%.1f\n", i,                                    ║
// // ║            traj_points[i].x_mm,                                          ║
// // ║            traj_points[i].y_mm,                                          ║
// // ║            traj_points[i].heading_deg);                                  ║
// // ║ }                                                                        ║
// // ║                                                                          ║
// // ╚══════════════════════════════════════════════════════════════════════════╝
