#ifndef ADC_DRIVER_H
#define ADC_DRIVER_H

#include <stdint.h>

void     adc_driver_init(void);
uint16_t adc_read_channel(uint8_t channel);   // generic, no domain meaning

#endif