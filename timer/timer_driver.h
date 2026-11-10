#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/**
 * Initializes the RTC for long-term tracking (days) and SysTick for millisecond resolution.
 * Must be called once during system initialization.
 */
void timer_init(void);

/**
 * Returns the elapsed time in milliseconds since timer_init() was called.
 * This is safe to call from the main loop and interrupt context.
 * @return uint32_t Milliseconds since boot (wraps after ~49 days).
 */
uint32_t timer_now_ms(void);

/**
 * Checks if a given duration has elapsed since a start timestamp.
 * Handles timer wraparound safely.
 * @param start      The timestamp returned by timer_now_ms() or rtc_get_timestamp().
 * @param duration_ms The duration to wait in milliseconds.
 * @return true if the duration has elapsed.
 */
bool timer_elapsed_ms(uint32_t start, uint32_t duration_ms);

/**
 * Returns the current RTC timestamp in seconds.
 * Use this for durations longer than ~49 days.
 * @return uint32_t Seconds since RTC was initialized (or since epoch if set).
 */
uint32_t rtc_now_seconds(void);

/**
 * Checks if a given duration (in seconds) has elapsed using the RTC.
 * Useful for the 3-day (259200 seconds) cycle.
 * @param start_seconds  The timestamp from rtc_now_seconds().
 * @param duration_sec   The duration to wait in seconds.
 * @return true if the duration has elapsed.
 */
bool rtc_elapsed_seconds(uint32_t start_seconds, uint32_t duration_sec);

#endif