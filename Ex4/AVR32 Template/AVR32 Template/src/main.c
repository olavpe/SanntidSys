#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


__attribute__((__interrupt__)) static void interrupt_J3(void);
void test_A();
void test_A_init();
void test_ABC();
void test_ABC_init();
void test_ABC_interrupts_init();

volatile int FLAG_A = 0;
volatile int FLAG_B = 0;
volatile int FLAG_C = 0;


void init(){
    sysclk_init();
    board_init();
    busy_delay_init(BOARD_OSC0_HZ);
    

    cpu_irq_disable();
    INTC_init_interrupts();
	//Added by us START

    test_ABC_interrupts_init();
	//Added by us END
	INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);  // Task C
    cpu_irq_enable();
    
    stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
        setbuf(stdout, NULL);
        setbuf(stdin,  NULL);
    #endif
}

// Task C
__attribute__((__interrupt__)) static void interrupt_J3(void){
	//printf("inside handler\n\r");
	if (gpio_get_pin_interrupt_flag(TEST_A)) {
		//gpio_set_pin_low(RESPONSE_A);
		//busy_delay_us(5);
		//gpio_set_pin_high(RESPONSE_A);
		//gpio_clear_pin_interrupt_flag(TEST_A);	
		FLAG_A = 1;
		gpio_clear_pin_interrupt_flag(TEST_A);
	}
	if (gpio_get_pin_interrupt_flag(TEST_B)){
		//gpio_set_pin_low(RESPONSE_B);
		//// Changes for D START
		//busy_delay_us(5);
		////busy_delay_us(100);
		//// Changes for D END
		//gpio_set_pin_high(RESPONSE_B);
		//gpio_clear_pin_interrupt_flag(TEST_B);
		FLAG_B = 1;
		gpio_clear_pin_interrupt_flag(TEST_B);
		
	}
	if (gpio_get_pin_interrupt_flag(TEST_C)){
		//gpio_set_pin_low(RESPONSE_C);
		//busy_delay_us(5);
		//gpio_set_pin_high(RESPONSE_C);
		//gpio_clear_pin_interrupt_flag(TEST_C);
		FLAG_C = 1;
		gpio_clear_pin_interrupt_flag(TEST_C);
	}
}


int main (void){
    init();
    ////Task A
    //test_A_init();
    ////Task B
    //test_ABC_init();
    ////Task C
	
	while(1){
		//// Initial code
        //gpio_toggle_pin(LED0_GPIO);
        //printf("tick\n");
        //busy_delay_ms(5);
		if (FLAG_A){
			gpio_set_pin_low(RESPONSE_A);
			busy_delay_us(5);
			gpio_set_pin_high(RESPONSE_A);
			//gpio_clear_pin_interrupt_flag(TEST_A);
			FLAG_A = 0;
		}
		if (FLAG_B){
			gpio_set_pin_low(RESPONSE_B);
			// Changes for D START
			//busy_delay_us(5);
			busy_delay_us(100);
			// Changes for D END
			gpio_set_pin_high(RESPONSE_B);
			//gpio_clear_pin_interrupt_flag(TEST_B);
			FLAG_B = 0;
		}
		if (FLAG_C){
			gpio_set_pin_low(RESPONSE_C);
			busy_delay_us(5);
			gpio_set_pin_high(RESPONSE_C);
			//gpio_clear_pin_interrupt_flag(TEST_C);
			FLAG_C = 0;
		}
		////Task A
		//test_A();
		////Task B
		//test_ABC();
	}
}

//TaskA
void test_A_init(){
	gpio_enable_gpio_pin(RESPONSE_A);
	gpio_enable_gpio_pin(TEST_A);
	gpio_configure_pin (RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_LOW);
	gpio_configure_pin (TEST_A, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
}

void test_A(){
	while(!gpio_get_pin_value(TEST_A) == 0){}
	gpio_set_pin_low(RESPONSE_A);
	busy_delay_us(5);
	gpio_set_pin_high(RESPONSE_A);
}

//TaskB
void test_ABC_init(){
	gpio_enable_gpio_pin(RESPONSE_A);
	gpio_enable_gpio_pin(RESPONSE_B);
	gpio_enable_gpio_pin(RESPONSE_C);
	gpio_enable_gpio_pin(TEST_A);
	gpio_enable_gpio_pin(TEST_B);
	gpio_enable_gpio_pin(TEST_C);
	gpio_configure_pin (RESPONSE_A, GPIO_DIR_OUTPUT);
	gpio_configure_pin (RESPONSE_B, GPIO_DIR_OUTPUT);
	gpio_configure_pin (RESPONSE_C, GPIO_DIR_OUTPUT);
	gpio_configure_pin (TEST_A, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
	gpio_configure_pin (TEST_B, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
	gpio_configure_pin (TEST_C, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
}

void test_ABC(){
	while (1)
	{
		if(gpio_get_pin_value(TEST_A) == 0)
		gpio_set_pin_low(RESPONSE_A);
		busy_delay_us(5);
		gpio_set_pin_high(RESPONSE_A);
		
		if(gpio_get_pin_value(TEST_B) == 0)
		gpio_set_pin_low(RESPONSE_B);
		busy_delay_us(5);
		gpio_set_pin_high(RESPONSE_B);
		
		if(gpio_get_pin_value(TEST_C) == 0)
		gpio_set_pin_low(RESPONSE_C);
		busy_delay_us(5);
		gpio_set_pin_high(RESPONSE_C);

	}
}

//TaskC
void test_ABC_interrupts_init(){
	/*
	gpio_enable_gpio_pin(RESPONSE_A);
	gpio_enable_gpio_pin(RESPONSE_B);
	gpio_enable_gpio_pin(RESPONSE_C);
	gpio_enable_gpio_pin(TEST_A);
	gpio_enable_gpio_pin(TEST_B);
	gpio_enable_gpio_pin(TEST_C);*/
	gpio_configure_pin (RESPONSE_A, GPIO_DIR_OUTPUT);
	gpio_configure_pin (RESPONSE_B, GPIO_DIR_OUTPUT);
	gpio_configure_pin (RESPONSE_C, GPIO_DIR_OUTPUT);
	gpio_configure_pin (TEST_A, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
	gpio_configure_pin (TEST_B, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
	gpio_configure_pin (TEST_C, GPIO_DIR_INPUT | GPIO_INIT_HIGH);
	gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
	gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);
}
