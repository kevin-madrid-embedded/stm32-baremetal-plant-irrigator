#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdint.h>
#include <stdio.h>



static void clock_setup(void)
{
    /* Set system clock to 84 MHz using the internal oscillator (HSI) */
    rcc_clock_setup_pll(&rcc_hsi_configs[RCC_CLOCK_3V3_84MHZ]);

    /* Enable peripheral clocks for GPIOA and USART2 */
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART2);
}

static void usart_setup(void)
{
    /* Configure PA2 (TX) and PA3 (RX) as Alternate Function (AF) pins */
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
    
    /* Set the specific alternate function for USART2 (AF7 for STM32F4) */
    gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);

    /* Configure USART2 parameters */
    usart_set_baudrate(USART2, 115200);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX_RX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    /* Finally, enable the peripheral */
    usart_enable(USART2);
}

/* Helper function to transmit a single character (blocking) */
void uart_putc(char c)
{
    usart_send_blocking(USART2, c);
}

/* Helper function to transmit a null-terminated string */
void uart_puts(const char *str)
{
    while (*str) {
        uart_putc(*str++);
    }
}


int usart_write(const char*str, uint16_t input)
{
    static char setup = false;
    if(setup == 0){
        clock_setup();
        usart_setup();
        setup = 1;
    }
    /* Send the frame once upon startup */
    //uart_puts("Hello World\r\n");

    /*Lets comment out the infinite loop for testing multi task in main*/
    //while (1) {
        /* If you want to send it repeatedly, uncomment the code below: */
    char buf[32];
    snprintf(buf, sizeof(buf), "%s %d\r\n",str, input);    
    uart_puts(buf);
    //for (volatile int i = 0; i < 2000000; i++); // Crude delay
    
    //}

    return 0;
}
