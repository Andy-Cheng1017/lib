#ifndef _PID_H_
#define _PID_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * 加熱或冷卻模式
 */
typedef enum {
  PID_MODE_COOL,  // 冷卻：溫度高於目標時啟動輸出
  PID_MODE_HEAT,  // 加熱：溫度低於目標時啟動輸出
} PID_Mode;

/**
 * PID 控制器結構
 */
typedef struct {
  /* 當前 PID 參數 */
  float kp;  // 比例增益
  float ki;  // 積分增益
  float kd;  // 微分增益

  /* PID 狀態 */
  float integral;
  float prev_error;

  /* 時間參數 */
  float dt;  // 更新週期 (秒)

  /* 輸出限制 */
  int output_min;
  int output_max;

  /* 模式：加熱或冷卻 */
  PID_Mode mode;
} PIDController;

/**
 * 初始化 PID 控制器
 */
void PID_Init(PIDController *pid, float kp, float ki, float kd, float dt, int output_min, int output_max, PID_Mode mode);

/**
 * 重置 PID 控制器狀態
 */
void PID_Reset(PIDController *pid);

/**
 * 執行一次 PID 更新 (含條件積分抗飽和)
 * @return 控制輸出 (output_min ~ output_max)
 */
int PID_Update(PIDController *pid, float target, float measured);

#ifdef __cplusplus
}
#endif

#endif /* _PID_H_ */
