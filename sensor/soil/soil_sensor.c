#include "soil_sensor.h"
#include "sensor/driver/adc_driver.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>


#define ADC_CHANNEL1      1
#define SOIL_ADC_CHANNEL  ADC_CHANNEL1
#define SOIL_GPIO_PORT    GPIOA
#define SOIL_GPIO_PIN     GPIO1

void soil_sensor_init(void) {
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(SOIL_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, SOIL_GPIO_PIN);
}

uint16_t soil_sensor_read_raw(void) {
    return adc_read_channel(SOIL_ADC_CHANNEL);
}