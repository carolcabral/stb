#ifndef __TIMER_H__
#define __TIMER_H__

void start_timer();
void stop_timer();
void configure_timer(int timer_interval_sec);
void configure_timer_alarm(int timer_interval_sec);
void clear_timer();
#endif