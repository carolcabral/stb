#ifndef __MY_TIMER_H__
#define __MY_TIMER_H__

void configure_pins();
void start_timer();
void stop_timer();
void configure_timer_alarm(int timer_interval_sec);
#endif