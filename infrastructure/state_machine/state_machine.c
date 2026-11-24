#include <usart/usart.h>
#include <led/led_driver.h>
#include <pump/pump.h>
#include <sensor/water/reservoir_sensor.h>
#include <sensor/soil/soil_sensor.h>
#include <sensor/driver/adc_driver.h>
#include <timer/timer_driver.h>
#include <watchdog/watchdog.h>
#include <infrastructure/event_queue/event_queue.h>
#include <stddef.h>
#include "state_machine.h"

EventQueue_t queue = {0};  
uint32_t watering_timeout = 0;
uint32_t sleep_timeout = 0;


typedef enum{
    EVENT_INVALID, 
    EVENT_INIT_COMPLETE,
    EVENT_WATER_LEVEL_OK, 
    EVENT_WATER_LEVEL_LOW,
    EVENT_MOISTURE_LEVEL_OK,
    EVENT_MOISTURE_LEVEL_LOW,
    EVENT_WATERING,
    EVENT_WATERING_TIMEOUT,
    EVENT_SLEEP,
    EVENT_SLEEP_TIMEOUT
} EVENTS;


typedef void (*fptr_t)(void);

typedef struct state_finite_machine{
    STATES current_state;
    EVENTS event;
    STATES new_state;
    fptr_t action;
} SYSTEM_STATE_MACHINE;


//Action Functions 
void action_check_reservoir_moisture_sensor_level(void);
void action_check_soil_moisture_sensor_level(void);
void action_activate_pump(void);
void action_check_watering_timeout(void);
void action_set_error_signal(void);
void action_start_sleep_timer(void);
void action_check_sleep_timeout(void);

//Helper functions
void set_working_signal(void);
void set_sleep_signal(void);

const SYSTEM_STATE_MACHINE fsm_table[] = {
     //Current state                  Event                     Next State                       Execution
    {STATE_INIT,                      EVENT_INIT_COMPLETE,      STATE_CHECK_WATER_LEVEL,         action_check_reservoir_moisture_sensor_level},
    {STATE_INIT,                      EVENT_WATER_LEVEL_OK,     STATE_INIT,                      NULL},
    {STATE_INIT,                      EVENT_WATER_LEVEL_LOW,    STATE_INIT,                      NULL},
    {STATE_INIT,                      EVENT_MOISTURE_LEVEL_OK,  STATE_INIT,                      NULL},
    {STATE_INIT,                      EVENT_MOISTURE_LEVEL_LOW, STATE_INIT,                      NULL},
    {STATE_INIT,                      EVENT_WATERING_TIMEOUT,   STATE_INIT,                      NULL},
    {STATE_INIT,                      EVENT_SLEEP_TIMEOUT,      STATE_INIT,                      NULL},

    {STATE_CHECK_WATER_LEVEL,         EVENT_INIT_COMPLETE,      STATE_CHECK_WATER_LEVEL,         NULL },
    {STATE_CHECK_WATER_LEVEL,         EVENT_WATER_LEVEL_OK,     STATE_CHECK_SOIL_MOISTURE_LEVEL, action_check_soil_moisture_sensor_level },
    {STATE_CHECK_WATER_LEVEL,         EVENT_WATER_LEVEL_LOW,    STATE_ERROR,                     action_set_error_signal},
    {STATE_CHECK_WATER_LEVEL,         EVENT_MOISTURE_LEVEL_OK,  STATE_CHECK_WATER_LEVEL,         NULL},
    {STATE_CHECK_WATER_LEVEL,         EVENT_MOISTURE_LEVEL_LOW, STATE_CHECK_WATER_LEVEL,         NULL},
    {STATE_CHECK_WATER_LEVEL,         EVENT_WATERING_TIMEOUT,   STATE_CHECK_WATER_LEVEL,         NULL},
    {STATE_CHECK_WATER_LEVEL,         EVENT_SLEEP_TIMEOUT,      STATE_CHECK_WATER_LEVEL,         NULL},
    {STATE_CHECK_WATER_LEVEL,         EVENT_WATER_LEVEL_OK,     STATE_CHECK_WATER_LEVEL,         NULL},

    {STATE_CHECK_SOIL_MOISTURE_LEVEL, EVENT_INIT_COMPLETE,      STATE_CHECK_SOIL_MOISTURE_LEVEL, NULL},
    {STATE_CHECK_SOIL_MOISTURE_LEVEL, EVENT_WATER_LEVEL_OK,     STATE_CHECK_SOIL_MOISTURE_LEVEL, NULL},
    {STATE_CHECK_SOIL_MOISTURE_LEVEL, EVENT_WATER_LEVEL_LOW,    STATE_CHECK_SOIL_MOISTURE_LEVEL, NULL},
    {STATE_CHECK_SOIL_MOISTURE_LEVEL, EVENT_MOISTURE_LEVEL_OK,  STATE_SLEEP,                     action_start_sleep_timer},
    {STATE_CHECK_SOIL_MOISTURE_LEVEL, EVENT_MOISTURE_LEVEL_LOW, STATE_WATERING,                  action_activate_pump },
    {STATE_CHECK_SOIL_MOISTURE_LEVEL, EVENT_WATERING_TIMEOUT,   STATE_CHECK_SOIL_MOISTURE_LEVEL, NULL},
    {STATE_CHECK_SOIL_MOISTURE_LEVEL, EVENT_SLEEP_TIMEOUT,      STATE_CHECK_SOIL_MOISTURE_LEVEL, NULL},
    
    {STATE_WATERING,                  EVENT_INIT_COMPLETE,      STATE_WATERING,                  NULL},
    {STATE_WATERING,                  EVENT_WATER_LEVEL_OK,     STATE_WATERING,                  NULL},
    {STATE_WATERING,                  EVENT_WATER_LEVEL_LOW,    STATE_WATERING,                  NULL},
    {STATE_WATERING,                  EVENT_MOISTURE_LEVEL_OK,  STATE_WATERING,                  NULL},
    {STATE_WATERING,                  EVENT_MOISTURE_LEVEL_LOW, STATE_WATERING,                  NULL},
    {STATE_WATERING,                  EVENT_WATERING,           STATE_WATERING,                  action_check_watering_timeout},
    {STATE_WATERING,                  EVENT_WATERING_TIMEOUT,   STATE_CHECK_WATER_LEVEL,         action_check_reservoir_moisture_sensor_level},
    {STATE_WATERING,                  EVENT_SLEEP_TIMEOUT,      STATE_WATERING,                  NULL},


    {STATE_SLEEP,                     EVENT_INIT_COMPLETE,      STATE_SLEEP,                     NULL},
    {STATE_SLEEP,                     EVENT_WATER_LEVEL_OK,     STATE_SLEEP,                     NULL},
    {STATE_SLEEP,                     EVENT_WATER_LEVEL_LOW,    STATE_SLEEP,                     NULL},
    {STATE_SLEEP,                     EVENT_MOISTURE_LEVEL_OK,  STATE_SLEEP,                     NULL},
    {STATE_SLEEP,                     EVENT_MOISTURE_LEVEL_LOW, STATE_SLEEP,                     NULL},
    {STATE_SLEEP,                     EVENT_WATERING_TIMEOUT,   STATE_SLEEP,                     NULL},
    {STATE_SLEEP,                     EVENT_SLEEP,              STATE_SLEEP,                     action_check_sleep_timeout},
    {STATE_SLEEP,                     EVENT_SLEEP_TIMEOUT,      STATE_CHECK_WATER_LEVEL,         action_check_reservoir_moisture_sensor_level},

    {STATE_ERROR,                     EVENT_INIT_COMPLETE,      STATE_ERROR,                     NULL},
    {STATE_ERROR,                     EVENT_WATER_LEVEL_OK,     STATE_ERROR,                     NULL},
    {STATE_ERROR,                     EVENT_WATER_LEVEL_LOW,    STATE_ERROR,                     NULL},
    {STATE_ERROR,                     EVENT_MOISTURE_LEVEL_OK,  STATE_ERROR,                     NULL},
    {STATE_ERROR,                     EVENT_MOISTURE_LEVEL_LOW, STATE_ERROR,                     NULL},
    {STATE_ERROR,                     EVENT_WATERING_TIMEOUT,   STATE_ERROR,                     NULL},
    {STATE_ERROR,                     EVENT_SLEEP_TIMEOUT,      STATE_ERROR,                     NULL},
};

#define FSM_TABLE_SIZE (sizeof(fsm_table)/sizeof(fsm_table[0]))

void fsm_run ( SYSTEM_CONTEXT_T *sys){
    Events_t ev;
    while (queue_pop(&queue, &ev)){
        for(uint32_t i = 0; i < FSM_TABLE_SIZE; i++){
            if(fsm_table[i].current_state == sys->current_state && 
                    fsm_table[i].event == (EVENTS)ev){
                if(fsm_table[i].action != NULL){
                    fsm_table[i].action(); 
                }
                sys->current_state = fsm_table[i].new_state;
                return;
            }
        }
    }
    //Log an error for invalid event for state
} 
void action_hardware_init(void){
    soil_sensor_init();
    reservoir_sensor_init();
    adc_driver_init();  
    pump_init();
    led_init();
    watchdog_init(WATCHDOG_PERIOD_TIME);
    timer_init();
    usart_write("System start: ", timer_now_ms());              
    watchdog_start();
    queue_init(&queue);
    blinking_led();
    queue_push(&queue, EV_INIT_COMPLETE);
}

void action_check_reservoir_moisture_sensor_level(void){

    int const empty = 4000;
    int const full = 3000;
    if(pump_is_on() == true)
        pump_off();

    
    int read = reservoir_sensor_read_raw();
    if (read >= empty){
        queue_push(&queue,EV_WATER_LEVEL_LOW);
        usart_write("System Event: EVENT_WATER_LEVEL_LOW Sensor read=%d",read);

    }else if(read <= full){
        queue_push(&queue,EV_WATER_LEVEL_OK);
        usart_write("System Event: EVENT_WATER_LEVEL_OK Sensor read=%d",read);
    }else {
        usart_write("Unexpected sensor read Sensor read=%d",read);

    }
}
void action_check_soil_moisture_sensor_level(void){
    int const dry = 1000;
    int const wet = 2000;
    int read = soil_sensor_read_raw();
    if (read >= wet){
        queue_push(&queue, EV_MOISTURE_LEVEL_OK);
        usart_write("System Event: EVENT_WATER_LEVEL_LOW Sensor read=%d",read);

    }else if(read <= dry){
        queue_push(&queue, EV_MOISTURE_LEVEL_LOW);
        usart_write("System Event: EVENT_WATER_LEVEL_OK Sensor read=%d",read);
    }else {
        usart_write("Unexpected sensor read Sensor read=%d",read);
    }
}
void action_activate_pump(void){
    pump_on();
    // I would need to check on the timer here to set the timeout here, so I can use the global queue declared in this scope
    queue_push(&queue, EV_WATERING);
    watering_timeout = rtc_now_seconds();
}
void action_check_watering_timeout(void){
    if(!rtc_elapsed_seconds(watering_timeout,4)){
        queue_push(&queue, EV_WATERING);
    }
    else{
        queue_push(&queue, EV_WATERING_TIMEOUT);
        watering_timeout = 0;
    }
}

void action_start_sleep_timer(void){
    queue_push(&queue, EV_SLEEP);
    sleep_timeout = rtc_now_seconds();
}

void action_check_sleep_timeout(void){
    if(!rtc_elapsed_seconds(sleep_timeout,30)){
        queue_push(&queue, EV_SLEEP);
    }
    else{
        queue_push(&queue, EV_SLEEP_TIMEOUT);
        sleep_timeout = 0;
    }
}

void action_set_error_signal(void){
    usart_write("Error signal",0);
}