#include "Arduino.h"
#include "FreeRTOS.h"
#include "task.h"

void delay(unsigned long ms) {
    vTaskDelay(ms);
}

void delayMicroseconds(unsigned int us) {
    vTaskDelay(2);
}

unsigned long micros(void){
    return xTaskGetTickCount() * portTICK_PERIOD_MS * 1000;
}

MySerial Serial;
