#include "timer_driver.h"
#include <led/led_driver.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/rtc.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/cm3/nvic.h>

// Global millisecond counter, incremented by SysTick
static volatile uint32_t _ms_ticks = 0;

uint32_t timer_now_ms(void) {
    return _ms_ticks;
}

bool timer_elapsed_ms(uint32_t start, uint32_t duration_ms) {
    // Handle 32-bit overflow safely
    return (uint32_t)(_ms_ticks - start) >= duration_ms;
}

// SysTick interrupt handler
void sys_tick_handler(void) {
    _ms_ticks++;
}

/**
 * Configures SysTick to fire every 1ms.
 * Assumes the core clock is configured to 96MHz (as in your previous adc_setup).
 * SysTick_LOAD = (Core_Clock / 1000) - 1
 */
static void _systick_setup(void) {
    // 96MHz / 1000 = 96000 ticks per millisecond
    systick_set_reload(96000 - 1); 
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    systick_counter_enable();
    systick_interrupt_enable();
}

/**
 * Configures the RTC to use the LSE (32.768kHz) oscillator.
 * This requires a 32.768kHz crystal on the board.
 * Falls back to LSI if LSE fails to start.
 */
static void _rtc_setup(void) {
    // 1. Enable Power Interface Clock and RTC Clock
    rcc_periph_clock_enable(RCC_PWR);
    rcc_periph_clock_enable(RCC_RTC);

    // 2. Enable write access to the Backup Domain
    pwr_disable_backup_domain_write_protect();

    // 3. Try to start LSE with a timeout to prevent system freeze
    rcc_osc_on(RCC_LSE);
    uint32_t timeout = 0;
    bool lse_ok = false;
    
    // Wait up to 2 seconds for LSE to be ready
    while (!timer_elapsed_ms(timer_now_ms(), 4000)) {
        if (RCC_BDCR & RCC_BDCR_LSERDY) {
            lse_ok = true;
            set_led();
            break;
        }
    }

    // 4. Select the RTC clock source (Fallback to LSI if LSE failed)
     if (lse_ok) {
        // Select LSE as RTC clock source by writing to RCC_BDCR
        RCC_BDCR &= ~(RCC_BDCR_RTCSEL_MASK << RCC_BDCR_RTCSEL_SHIFT);
        RCC_BDCR |= (RCC_BDCR_RTCSEL_LSE << RCC_BDCR_RTCSEL_SHIFT);
    } else {
        rcc_osc_off(RCC_LSE);
        rcc_osc_on(RCC_LSI);
        rcc_wait_for_osc_ready(RCC_LSI);
        
        // Select LSI as RTC clock source
        RCC_BDCR &= ~(RCC_BDCR_RTCSEL_MASK << RCC_BDCR_RTCSEL_SHIFT);
        RCC_BDCR |= (RCC_BDCR_RTCSEL_LSI << RCC_BDCR_RTCSEL_SHIFT);
    }

    // 5. Unlock RTC registers and enter init mode
    rtc_unlock();
    rtc_set_init_flag();
    rtc_wait_for_init_ready();

    // 6. Set prescalers (Update this if you fell back to LSI!)
    if (lse_ok) {
        // LSE: 32768 Hz / (127+1)*(255+1) = 1 Hz
        rtc_set_prescaler(255, 127);
    } else {
        // LSI: ~32000 Hz / (124+1)*(255+1) = ~1 Hz
        rtc_set_prescaler(255, 124); 
    }

        // --- NEW: Explicitly set the time to 00:00:00 ---
    // This ensures the RTC counts up from zero instead of staying at zero
    RTC_TR = 0x00000000; 
    RTC_DR = 0x00002101; // (Optional) Set a dummy date, e.g., 2000-01-01

    // 7. Exit init mode and lock
    rtc_clear_init_flag();
    rtc_lock();
    rtc_wait_for_synchro();

    // 8. Re-enable write protection
    pwr_enable_backup_domain_write_protect();
}
void timer_init(void) {
    _systick_setup();
    _rtc_setup();
}

uint32_t rtc_now_seconds(void) {
    //rtc_wait_for_synchro(); 
    // Read the time register (RTC_TR)
    // The BCD fields are: [23:20] Hour Tens, [19:16] Hour Units,
    // [15:12] Min Tens, [11:8] Min Units, [7:4] Sec Tens, [3:0] Sec Units
    uint32_t tr = RTC_TR;

    // Extract BCD digits and convert to binary
    uint32_t seconds = ((tr >> 0) & 0xF) + (((tr >> 4) & 0x7) * 10);
    uint32_t minutes = ((tr >> 8) & 0xF) + (((tr >> 12) & 0x7) * 10);
    uint32_t hours   = ((tr >> 16) & 0xF) + (((tr >> 20) & 0x3) * 10);

    // Convert to total seconds since midnight
    return (hours * 3600) + (minutes * 60) + seconds;
}


bool rtc_elapsed_seconds(uint32_t start_seconds, uint32_t duration_sec) {
    uint32_t now = rtc_now_seconds();
    // Handle potential RTC counter wrap-around (rare, but possible after 136 years)
    return (uint32_t)(now - start_seconds) >= duration_sec;
}