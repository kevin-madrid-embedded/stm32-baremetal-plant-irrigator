#include "pump.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>

#define PUMP_RCC_PORT    (RCC_GPIOB)
#define PUMP_PORT        (GPIOB)
#define PUMP_PIN         (GPIO0)

void pump_init(void){
    // Setup system clock
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]); // set system clock

    // setup GPIO
    rcc_periph_clock_enable(PUMP_RCC_PORT);
    gpio_mode_setup(PUMP_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, PUMP_PIN);
    gpio_set_output_options(PUMP_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, PUMP_PIN);

}
void pump_on(void){
    gpio_set( PUMP_PORT,PUMP_PIN); 	
}
void pump_off(void){
    gpio_clear(PUMP_PORT,PUMP_PIN);
}
bool pump_is_on(void){
    return gpio_get(PUMP_PORT,PUMP_PIN);
}