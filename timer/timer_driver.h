#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <stdint.h>
#include <stdbool.h>


void timer_init(void);
uint32_t timer_now_ms(void);
bool timer_elapsed_ms(uint32_t start, uint32_t duration_ms);
uint32_t rtc_now_seconds(void);
bool rtc_elapsed_seconds(uint32_t start_seconds, uint32_t duration_sec);


#endif