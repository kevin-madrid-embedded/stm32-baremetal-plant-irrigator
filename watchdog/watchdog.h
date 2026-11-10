#ifndef WATCHDOG_H
#define WATCHDOG_H

void watchdog_init(int user_period);
void watchdog_start(void);
void watchdog_kick(void);

#endif