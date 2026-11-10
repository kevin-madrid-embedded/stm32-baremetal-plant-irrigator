#include "adc_driver.h"
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>

void adc_driver_init(void) {
    rcc_periph_clock_enable(RCC_ADC1);
    adc_power_off(ADC1);
    adc_set_clk_prescale(ADC_CCR_ADCPRE_BY4);
    adc_set_multi_mode(ADC_CCR_MULTI_INDEPENDENT);
    adc_set_resolution(ADC1, ADC_CR1_RES_12BIT);
    adc_set_right_aligned(ADC1);
    adc_set_single_conversion_mode(ADC1);
    adc_power_on(ADC1);
    for (volatile int i = 0; i < 1000; i++) __asm__("nop");
}

uint16_t adc_read_channel(uint8_t channel) {
    adc_set_regular_sequence(ADC1, 1, &channel);
    adc_set_sample_time(ADC1, channel, ADC_SMPR_SMP_84CYC);
    adc_start_conversion_regular(ADC1);
    while (!adc_eoc(ADC1));
    return adc_read_regular(ADC1);
}