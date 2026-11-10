#include <usart/usart.h>
#include <led/led_driver.h>
#include <pump/pump.h>
#include <sensor/water/reservoir_sensor.h>
#include <sensor/soil/soil_sensor.h>
#include <sensor/driver/adc_driver.h>
#include <timer/timer_driver.h>
#include <watchdog/watchdog.h>

#define WATCHDOG_PERIOD_TIME 10000

int main(void){

    soil_sensor_init();
    reservoir_sensor_init();
    adc_driver_init();  
    pump_init();
    led_init();
    watchdog_init(WATCHDOG_PERIOD_TIME);
    timer_init();

    

    uint32_t last_soil_time = 0;
    uint32_t last_pump_time = 0;
    uint32_t last_rtc_time = 0;
    usart_write("System start: ", timer_now_ms());            
            
    watchdog_start();
    while(1){
        uint32_t current_time = rtc_now_seconds();

        // 1. Check Soil Sensor every 3 second
        if ((current_time - last_soil_time) >= 1) {
            last_soil_time = current_time; // Update the timestamp
            
            usart_write("Soil sensor:", soil_sensor_read_raw());
            
            pump_on();
        }
        // 2. Turn pump OFF 4 second AFTER it was turned ON
        if ((current_time - last_pump_time) >= 4 && pump_is_on()) {
            usart_write("Reservoir sensor:", reservoir_sensor_read_raw());
            pump_off();
            last_pump_time = current_time; // Reset pump timer
        }
        // 3. Print the RTC time
        if((current_time - last_rtc_time) >= 5){
            last_rtc_time = current_time;
            usart_write("RTC read", rtc_now_seconds());            
            watchdog_kick();
        }
    }
    return 0;
}