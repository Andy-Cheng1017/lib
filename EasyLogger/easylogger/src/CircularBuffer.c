#include "CircularBuffer.h"

// 初始化環形緩衝區
void CircularBuffer_Init(CircularBuffer *cb) {
  cb->head = 0;
  cb->tail = 0;
  cb->count = 0;
}

// 將數據寫入環形緩衝區
bool CircularBuffer_Write(CircularBuffer *cb, const uint8_t data) {
  // 緩衝區是否已滿
  if (cb->count == BUFFER_SIZE) {
    return false;  // 緩衝區滿，寫入失敗
  }

  cb->buffer[cb->head] = data;
  cb->head = (cb->head + 1) % BUFFER_SIZE;
  cb->count++;
  return true;
}

// 批量寫入數據
uint16_t CircularBuffer_WriteBuffer(CircularBuffer *cb, const uint8_t *data, uint16_t len) {
  uint16_t bytesWritten = 0;

  for (uint16_t i = 0; i < len; i++) {
    if (CircularBuffer_Write(cb, data[i])) {
      bytesWritten++;
    } else {
      break;  // 緩衝區滿，停止寫入
    }
  }
  return bytesWritten;
}

// 從環形緩衝區讀取數據
bool CircularBuffer_Read(CircularBuffer *cb, uint8_t *data) {
  // 檢查緩衝區是否為空
  if (cb->count == 0) {
    return false;
  }

  *data = cb->buffer[cb->tail];
  cb->tail = (cb->tail + 1) % BUFFER_SIZE;
  cb->count--;
  return true;
}

// 批量讀取數據
uint16_t CircularBuffer_ReadBuffer(CircularBuffer *cb, uint8_t *data, uint16_t len) {
  uint16_t bytesRead = 0;

  for (uint16_t i = 0; i < len; i++) {
    if (CircularBuffer_Read(cb, &data[i])) {
      bytesRead++;
    } else {
      break;  // 緩衝區已空，停止讀取
    }
  }
  return bytesRead;
}

// 獲取緩衝區剩餘空間
uint16_t CircularBuffer_AvailableSpace(CircularBuffer *cb) { return BUFFER_SIZE - cb->count; }

// 檢查緩衝區是否為空
bool CircularBuffer_IsEmpty(CircularBuffer *cb) { return cb->count == 0; }

// 清空環形緩衝區
void CircularBuffer_Clear(CircularBuffer *cb) {
  cb->head = 0;
  cb->tail = 0;
  cb->count = 0;
}
