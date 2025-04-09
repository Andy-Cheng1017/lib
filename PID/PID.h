#ifndef _PID_H_
#define _PID_H_

#ifdef __cplusplus
extern "C" {
#endif

// PIDController 結構，存放 PID 參數及狀態等資訊
typedef struct {
  // 當前 PID 參數
  float kp;  // 當前比例增益
  float ki;  // 當前積分增益
  float kd;  // 當前微分增益

  // 基礎 PID 參數（在某個參考工況下整定出來的參數）
  float kp_base;
  float ki_base;
  float kd_base;

  // 自適應部分參數
  float G_base;      // 參考系統增益（基準值）
  float G_filtered;  // 經過濾波後的系統增益估算

  // PID 狀態
  float integral;
  float prev_error;

  // 時間控制參數
  float dt;                           // 更新週期 (秒)
  float gain_update_interval;         // 參數自適應更新間隔 (秒)
  float time_since_last_gain_update;  // 自適應累計時間

  // 在自適應窗格開始時所儲存的數值
  float last_measured;  // 上一次自適應窗格開始的溫度值
  float last_control;   // 上一次自適應窗格開始的控制輸出

  float alpha;  // 自適應濾波係數 (0 < alpha < 1)

  // 輸出限制 (例如風扇 duty cycle 範圍 0～1000)
  int output_min;
  int output_max;
} PIDController;

/**
 * @brief 初始化 PIDController 結構
 *
 * @param pid                   PIDController 結構指標
 * @param kp_base               初始（基礎）比例增益
 * @param ki_base               初始（基礎）積分增益
 * @param kd_base               初始（基礎）微分增益
 * @param G_base                系統基準增益（用來做比例調整的參考值）
 * @param gain_update_interval  自適應更新間隔 (秒)
 * @param dt                    控制更新週期 (秒)
 * @param alpha                 自適應濾波係數 (0~1間，建議 0.1 ~ 0.3)
 * @param output_min            控制輸出下限 (例如 0)
 * @param output_max            控制輸出上限 (例如 1000)
 */
void PID_Init(PIDController *pid, float kp_base, float ki_base, float kd_base, float G_base, float gain_update_interval, float dt, float alpha,
              int output_min, int output_max);

/**
 * @brief 重置 PID 控制器內部變數，如積分、前次誤差與自適應計時器等
 *
 * @param pid  PIDController 結構指標
 */
void PID_Reset(PIDController *pid);

/**
 * @brief 根據目標與實際數值執行一次 PID 更新，並輸出控制信號
 *
 * @param pid     PIDController 結構指標
 * @param target  目標溫度 (浮點數)
 * @param measured 實際溫度 (浮點數)
 * @return int    控制輸出，範圍在 [output_min, output_max]（例如 0～1000）
 */
int PID_Update(PIDController *pid, float target, float measured);

#ifdef __cplusplus
}
#endif

#endif /* _PID_H_ */
