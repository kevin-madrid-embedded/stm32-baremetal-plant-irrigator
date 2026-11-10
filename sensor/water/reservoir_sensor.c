
#include "reservoir_sensor.h"
#include <sensor/driver/adc_driver.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>


#define ADC_CHANNEL0           0
#define RESERVOIR_ADC_CHANNEL  ADC_CHANNEL0
#define RESERVOIR_GPIO_PORT    GPIOA
#define RESERVOIR_GPIO_PIN     GPIO0


void reservoir_sensor_init(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(RESERVOIR_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, RESERVOIR_GPIO_PIN);
}

uint16_t reservoir_sensor_read_raw(void) {
    return adc_read_channel(RESERVOIR_ADC_CHANNEL);
}