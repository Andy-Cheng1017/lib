#include "PID.h"
#include <math.h>

static float constrain_float(float v, float lo, float hi) { return (v < lo ? lo : (v > hi ? hi : v)); }

void PID_Init(PIDController *pid, float kp, float ki, float kd, float dt, int output_min, int output_max, PID_Mode mode) {
  pid->kp = kp;
  pid->ki = ki;
  pid->kd = kd;

  pid->integral = 0.0f;
  pid->prev_error = 0.0f;

  pid->dt = dt;
  pid->output_min = output_min;
  pid->output_max = output_max;
  pid->mode = mode;
}

void PID_Reset(PIDController *pid) {
  pid->integral = 0.0f;
  pid->prev_error = 0.0f;
}

int PID_Update(PIDController *pid, float target, float measured) {
  // 1. 計算誤差
  float error = (pid->mode == PID_MODE_COOL) ? (measured - target) : (target - measured);

  // 2. 計算微分
  float derivative = (error - pid->prev_error) / pid->dt;

  // 3. 計算 P, I, D 分量
  float P = pid->kp * error;
  float I = pid->ki * pid->integral;
  float D = pid->kd * derivative;

  // 4. 原始輸出
  float raw_output = P + I + D;

  // 5. 限幅
  float limited = constrain_float(raw_output, (float)pid->output_min, (float)pid->output_max);

  // 6. 條件積分 (未飽和時才累積)
  if (fabsf(raw_output - limited) < 1e-6f) {
    pid->integral += error * pid->dt;
  }

  // 7. 更新
  pid->prev_error = error;

  // 8. 回傳
  return (int)(limited + 0.5f);
}