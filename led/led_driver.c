/******************************************************************************
 * blink
 * 
 * Program 1: Blink the built-in LED of STM32 Black Pill
 * 
 * Demostrate the basic setup and GPIO output of a STM32 using libopencm3
 *****************************************************************************/
#include "led_driver.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LED_RCC_PORT (RCC_GPIOC)
#define LED_PORT (GPIOC)
#define LED_PIN  (GPIO13)

/* A simple delay function
 * 
 * Note: each cycle takes about 5 ticks under -Os optimization
 */
static void delay_cycles(uint32_t cycles)
{
    while (cycles--) {
        __asm__("nop");     // assembly of no operation
    }
}

void led_init(void){
    // Setup system clock
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]); // set system clock

    // setup GPIO
    rcc_periph_clock_enable(LED_RCC_PORT);
    gpio_mode_setup(LED_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_PIN);
    gpio_set_output_options(LED_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, LED_PIN);
}

void blinking_led (void){
    /*Loop for task led, commenting out to test on main*/
    //while (1) {
    gpio_toggle(LED_PORT, LED_PIN);
    delay_cycles(10500000 / 3);     // delay 1s
    //}
}

void set_led(void){
    gpio_set(LED_PORT,LED_PIN);
}
