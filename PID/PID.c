#include "PID.h"
#include <stdlib.h>
#include <math.h>

/*
 * 輔助函式：限制 value 在 min 與 max 之間
 */
static float constrain_float(float value, float min, float max) {
  if (value < min) return min;
  if (value > max) return max;
  return value;
}

void PID_Init(PIDController *pid, float kp_base, float ki_base, float kd_base, float G_base, float gain_update_interval, float dt, float alpha,
              int output_min, int output_max) {
  if (!pid) return;

  pid->kp_base = kp_base;
  pid->ki_base = ki_base;
  pid->kd_base = kd_base;

  // 初始時，當前 PID 參數直接採用基礎參數
  pid->kp = kp_base;
  pid->ki = ki_base;
  pid->kd = kd_base;

  pid->G_base = G_base;
  pid->G_filtered = G_base;  // 初始濾波增益設為基準增益

  pid->integral = 0.0f;
  pid->prev_error = 0.0f;

  pid->dt = dt;
  pid->gain_update_interval = gain_update_interval;
  pid->time_since_last_gain_update = 0.0f;

  // 自適應窗格初始時，設定為 0（或可以在第一次更新時再初始化）
  pid->last_measured = 0.0f;
  pid->last_control = 0.0f;

  pid->alpha = alpha;

  pid->output_min = output_min;
  pid->output_max = output_max;
}

void PID_Reset(PIDController *pid) {
  if (!pid) return;

  pid->integral = 0.0f;
  pid->prev_error = 0.0f;
  pid->time_since_last_gain_update = 0.0f;
  pid->last_measured = 0.0f;
  pid->last_control = 0.0f;
}

int PID_Update(PIDController *pid, float target, float measured) {
  if (!pid) return 0;

  // ---------------------
  // 1. 計算 PID 控制部分
  // ---------------------
  // 誤差 = 目標 - 實際
  float error = measured- target;

  // 更新積分項（簡單積分，考慮 dt）
  pid->integral += error * pid->dt;

  // 計算微分項
  float derivative = (error - pid->prev_error) / pid->dt;

  // PID 控制輸出（未限制前）
  float control = pid->kp * error + pid->ki * pid->integral + pid->kd * derivative;

  // 更新前次誤差
  pid->prev_error = error;

  // -----------------------------
  // 2. 自適應參數更新機制
  // -----------------------------
  // 累計時間
  pid->time_since_last_gain_update += pid->dt;

  // // 若累計時間達到預設的更新間隔，則進行自適應估算
  // if (pid->time_since_last_gain_update >= pid->gain_update_interval) {
  //   // 計算這段期間內溫度與控制輸出的變化量
  //   float delta_measured = measured - pid->last_measured;
  //   float delta_control = control - pid->last_control;

  //   // 為避免除以零，檢查 delta_control 是否超過一個最小閾值
  //   float threshold = 0.01f;  // 可根據實際情況調整
  //   if (fabs(delta_control) > threshold) {
  //     // 估算系統增益：Δ溫度 / Δ控制輸出
  //     float G_est = delta_measured / delta_control;

  //     // 使用指數濾波更新 G_filtered
  //     pid->G_filtered = (1.0f - pid->alpha) * pid->G_filtered + pid->alpha * G_est;

  //     // 避免 G_filtered 過小造成過大調整
  //     if (fabs(pid->G_filtered) < 0.0001f) pid->G_filtered = 0.0001f;

  //     // 根據 G_base 與 G_filtered 的比值動態調整 PID 參數
  //     pid->kp = pid->kp_base * (pid->G_base / pid->G_filtered);
  //     pid->ki = pid->ki_base * (pid->G_base / pid->G_filtered);
  //     pid->kd = pid->kd_base * (pid->G_base / pid->G_filtered);
  //   }

  //   // 重置自適應窗格：儲存當前數值，並清空累計時間
  //   pid->last_measured = measured;
  //   pid->last_control = control;
  //   pid->time_since_last_gain_update = 0.0f;
  // }

  // -----------------------------
  // 3. 限制控制輸出
  // -----------------------------
  control = constrain_float(control, (float)pid->output_min, (float)pid->output_max);

  // 將控制輸出取整後回傳，這裡控制輸出就是風扇 duty cycle (0 ~ 1000)
  return (int)(control + 0.5f);  // 四捨五入
}
