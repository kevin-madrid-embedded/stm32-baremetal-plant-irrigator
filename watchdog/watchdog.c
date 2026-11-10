#include <libopencm3/stm32/iwdg.h>
#include <led/led_driver.h>
#include "watchdog.h"

void watchdog_init(int user_period){
    iwdg_set_period_ms(user_period);
}

void watchdog_start(void){
    iwdg_start();
}

void watchdog_kick(void){
    blinking_led();
    iwdg_reset();
}