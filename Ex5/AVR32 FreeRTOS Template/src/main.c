#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gpio.h>
#include <power_clocks_lib.h>
#include <sysclk.h>
#include <compiler.h>
#include <board.h>
#include <stdio_usb.h>


#define CONFIG_USART_IF (AVR32_USART2)

#include "FreeRTOS.h"
#include "task.h"

#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


void busy_delay_ms(int delay){
    for(; delay != 0; delay--){
        for(int i = 0; i < 2108; i++){
            asm volatile ("" ::: "memory");
        }
    }
}

void busy_delay_short(void){
    for(int i = 0; i < 10; i++){
        asm volatile ("" ::: "memory");
    }
}

void init(){
	board_init();
	
    gpio_configure_pin(TEST_A, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_B, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_C, GPIO_DIR_INPUT);
    gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
	
	stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
	    setbuf(stdout, NULL);
	    setbuf(stdin,  NULL);
    #endif
}

static void taskFn(void* args){
	const portTickType delay = 1000 / portTICK_RATE_MS;
	
    int iter = 0;

	while(1){
		gpio_toggle_pin(LED0_GPIO);
		printf("tick %d\n", iter++);
		
		vTaskDelay(delay);
	}
}

// Task A

static void blink_led0(void* args){
	const portTickType delay = 200 / portTICK_RATE_MS;
	while(1){
		gpio_toggle_pin(LED0_GPIO);
		vTaskDelay(delay);
	}
}

static void blink_led1(void* args){
	const portTickType delay = 500 / portTICK_RATE_MS;
	while(1){
		gpio_toggle_pin(LED1_GPIO);
		vTaskDelay(delay);
	}
}

static void task_A(void* args){
	xTaskCreate(blink_led0, "LED0_TASK", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(blink_led1, "LED1_TASK", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
}

// Task B
struct responseTaskArgs {
	struct {
		uint32_t test;
		uint32_t response;
	} pin;
	// other args ...
};

static void responseTask_B(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	
	while(1){
		if(gpio_pin_is_low(a.pin.test)){
			gpio_set_pin_low(a.pin.response);
			//vTaskDelay(1);
			busy_delay_short();
			gpio_set_pin_high(a.pin.response);
		}
	}
}


static void task_B(void* args){
	xTaskCreate(responseTask_B, "Test_A", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask_B, "Test_B", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B}}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask_B, "Test_C", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C}}), tskIDLE_PRIORITY + 1, NULL);
}
// Task C
static void responseTask_C(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	
	while(1){
		//printf("PIN: %d \n\r", a.pin.test);
		//printf("PIN C !!: %d \n\r", TEST_C);
		if(gpio_pin_is_low(a.pin.test)){
			if (a.pin.test == TEST_C){
				busy_delay_ms(3);
			}
			
			gpio_set_pin_low(a.pin.response);
			vTaskDelay(1);
			gpio_set_pin_high(a.pin.response);
		}
	}
}


static void task_C(void* args){
	xTaskCreate(responseTask_C, "Test_A", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask_C, "Test_B", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B}}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask_C, "Test_C", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C}}), tskIDLE_PRIORITY + 1, NULL);
}

// Task D
static void responseTask_D(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	
	while(1){
		if(gpio_pin_is_low(a.pin.test)){
			if (a.pin.test == TEST_C){
				busy_delay_ms(3);
			}
			
			gpio_set_pin_low(a.pin.response);
			vTaskDelay(1);
			gpio_set_pin_high(a.pin.response);
		} else {
			vTaskDelay(1);
		}
	}
}

static void task_D(void* args){
	xTaskCreate(responseTask_D, "Test_A", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask_D, "Test_B", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B}}), tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(responseTask_D, "Test_C", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C}}), tskIDLE_PRIORITY + 1, NULL);
}

// Task E
static void task_E(void* args){
	xTaskCreate(responseTask_D, "Test_A", 1024, (&(struct responseTaskArgs){{TEST_A, RESPONSE_A}}), tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate(responseTask_D, "Test_B", 1024, (&(struct responseTaskArgs){{TEST_B, RESPONSE_B}}), tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(responseTask_D, "Test_C", 1024, (&(struct responseTaskArgs){{TEST_C, RESPONSE_C}}), tskIDLE_PRIORITY + 1, NULL);
}


int main(){
	init();
        
	//xTaskCreate(taskFn, "", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
	task_E(NULL);
	
	// Start the scheduler, anything after this will not run.
	vTaskStartScheduler();
    
}

