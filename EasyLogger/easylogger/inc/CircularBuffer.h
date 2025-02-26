#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

// 配置緩衝區大小，需為 2 的倍數
#define BUFFER_SIZE 1024

// 環形緩衝區結構體
typedef struct {
  uint8_t buffer[BUFFER_SIZE];  // 緩衝區數組
  volatile uint16_t head;       // 寫指標
  volatile uint16_t tail;       // 讀指標
  volatile uint16_t count;      // 當前緩衝區中數據數量
} CircularBuffer;

extern CircularBuffer txBuffer;

// 緩衝區初始化
void CircularBuffer_Init(CircularBuffer *cb);

// 將數據寫入環形緩衝區
bool CircularBuffer_Write(CircularBuffer *cb, const uint8_t data);

// 批量寫入數據
uint16_t CircularBuffer_WriteBuffer(CircularBuffer *cb, const uint8_t *data, uint16_t len);

// 從環形緩衝區讀取數據
bool CircularBuffer_Read(CircularBuffer *cb, uint8_t *data);

// 批量讀取數據
uint16_t CircularBuffer_ReadBuffer(CircularBuffer *cb, uint8_t *data, uint16_t len);

// 獲取緩衝區可用空間
uint16_t CircularBuffer_AvailableSpace(CircularBuffer *cb);

// 檢查緩衝區是否為空
bool CircularBuffer_IsEmpty(CircularBuffer *cb);

// 清空環形緩衝區
void CircularBuffer_Clear(CircularBuffer *cb);

#endif  // CIRCULAR_BUFFER_H
