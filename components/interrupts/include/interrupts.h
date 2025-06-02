#ifndef __INTERRUPTS_H__
#define __INTERRUPTS_H__

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef struct
{
    bool is_ble;
    uint32_t mask_value;
    bool is_timer;

} interrupt_source_t;

extern QueueHandle_t evt_queue;

void configure_interrupts();

#endif