#ifndef RESERVOIR_SENSOR_H
#define RESERVOIR_SENSOR_H

#include <stdint.h>

void     reservoir_sensor_init(void);
uint16_t reservoir_sensor_read_raw(void);

#endif