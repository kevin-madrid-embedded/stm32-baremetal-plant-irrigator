#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H


typedef enum{
    STATE_INIT,
    STATE_CHECK_WATER_LEVEL,
    STATE_CHECK_SOIL_MOISTURE_LEVEL,
    STATE_WATERING,
    STATE_SLEEP,
    STATE_ERROR
} STATES;

typedef struct {
    STATES current_state;
}SYSTEM_CONTEXT_T;


void fsm_run(SYSTEM_CONTEXT_T *sys);
void action_hardware_init(void);

#endif