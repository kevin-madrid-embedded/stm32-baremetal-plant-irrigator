#ifndef PUMP_H
#define PUMP_H

#include <stdbool.h>

void pump_init(void);
void pump_on(void);
void pump_off(void);
bool pump_is_on(void);

#endif