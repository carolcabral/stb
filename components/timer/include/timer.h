#ifndef __TIMER_H__
#define __TIMER_H__

#include "driver/gptimer.h"

#define TIMERS_RESOLUTION_HZ (1 * 1000 * 1000) // Resolution is 1 MHz, i.e., 1 tick equals 1 microsecond

typedef struct
{
    gptimer_handle_t handle;
    uint8_t is_running;
    int alarm_interval_sec;
} timer_ctx_t;

extern timer_ctx_t water_timer;

void configure_timer(timer_ctx_t *timer);
void clear_timer(timer_ctx_t *timer);
void delete_timer(timer_ctx_t *timer);
double get_current_time(timer_ctx_t *timer);
#endif