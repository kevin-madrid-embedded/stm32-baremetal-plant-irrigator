#include <usart/usart.h>
#include <led/led_driver.h>
#include <pump/pump.h>
#include <sensor/water/reservoir_sensor.h>
#include <sensor/soil/soil_sensor.h>
#include <sensor/driver/adc_driver.h>
#include <timer/timer_driver.h>
#include <watchdog/watchdog.h>
#include <infrastructure/state_machine/state_machine.h>


int main(void){
    SYSTEM_CONTEXT_T sys;
    sys.current_state = STATE_INIT;
    action_hardware_init();

    while(1){
        fsm_run(&sys);
        watchdog_kick();
    }
    return 0;
}