#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define IT_SOURCE_BLE (uint8_t)0b00000001
#define IT_SOURCE_TIMER (uint8_t)0b00000010

typedef struct
{
    // uint32_t mask_value;
    void *mask_value;
    uint8_t source;
} interrupt_ctx_t;

extern QueueHandle_t evt_queue;

void configure_interrupts();

#endif