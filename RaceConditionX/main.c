/*
 * main.c
 *
 * Created: 20.03.2018 18:32:07
 * Author : chaos
 */ 

//#include <avr/io.h>
#include "avr_compiler.h"
#include "pmic_driver.h"
#include "TC_driver.h"
#include "clksys_driver.h"
#include "sleepConfig.h"
#include "port_driver.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "stack_macros.h"

#include "mem_check.h"

#include "init.h"
#include "utils.h"
#include "errorHandler.h"
#include "NHD0420Driver.h"


extern void vApplicationIdleHook( void );
void vDisplayTask(void *pvParameters);
void vButtonHandler1(void *pvParameters);
void vButtonHandler2(void *pvParameters);
void vButtonHandler3(void *pvParameters);
void vButtonHandler4(void *pvParameters);

QueueHandle_t	buttonqueue;
uint32_t buttonData = 0;			//P-Resource

void vApplicationIdleHook( void )
{	
	
}

int main(void)
{

	vInitClock();
	vInitDisplay();
	
	buttonqueue = xQueueCreate(10, sizeof(uint8_t));

	xTaskCreate( vDisplayTask, (const char *) "dTask", configMINIMAL_STACK_SIZE+10, NULL, 1, NULL);
	xTaskCreate( vButtonHandler1, (const char *) "bHandler", configMINIMAL_STACK_SIZE+10, NULL, 1, NULL);
	xTaskCreate( vButtonHandler2, (const char *) "bHandler", configMINIMAL_STACK_SIZE+10, NULL, 1, NULL);
	xTaskCreate( vButtonHandler3, (const char *) "bHandler", configMINIMAL_STACK_SIZE+10, NULL, 1, NULL);
	xTaskCreate( vButtonHandler4, (const char *) "bHandler", configMINIMAL_STACK_SIZE+10, NULL, 1, NULL);

	vDisplayClear();
	vTaskStartScheduler();
	return 0;
}

void vDisplayTask(void *pvParameters){
	uint32_t b1 = 0, b2 = 0, b3 = 0, b4 = 0;
	uint8_t local = 0;
	for(;;) {
		while(uxQueueMessagesWaiting(buttonqueue) > 0){
		if(xQueueReceive(buttonqueue, &local, portMAX_DELAY) == pdTRUE) { //Lock A-Resource to get access to P-Resource
			switch(local) {
				case 1:
					b1++;
				break;
				case 2:
					b2++;
				break;
				case 3:
					b3++;
				break;
				case 4:
					b4++;
				break;
				}
				buttonData = 0;
		}
		vDisplayWriteStringAtPos(0,0,"B1: %d", b1);
		vDisplayWriteStringAtPos(1,0,"B2: %d", b2);
		vDisplayWriteStringAtPos(2,0,"B3: %d", b3);
		vDisplayWriteStringAtPos(3,0,"B4: %d", b4);
		}
		vTaskDelay(100 / portTICK_RATE_MS);
	}
} //Alle 100ms Aufgerufen

void vButtonHandler1(void *pvParameters) { //Buttonhandler to debounce Button and send to DisplayTask
	PORTF.DIRCLR = 0x10;
	uint8_t value = 1;
	uint8_t * p_value = &value;
	
	for(;;) {
		if((PORTF.IN & 0x10) == 0x00) {
				while((PORTF.IN & 0x10) == 0x00) { //crude debouncing
					vTaskDelay(10);
				}
				xQueueSend(buttonqueue, p_value, 10);
		}
		vTaskDelay(50 / portTICK_RATE_MS);
	}
}
void vButtonHandler2(void *pvParameters) {
	PORTF.DIRCLR = 0x20;
	uint8_t value = 2;
	uint8_t * p_value = &value;
		
	for(;;) {
		if((PORTF.IN & 0x20) == 0x00) {
				while((PORTF.IN & 0x20) == 0x00) {
					vTaskDelay(10);
				}
				xQueueSend(buttonqueue, p_value, 10);
		}
		vTaskDelay(50 / portTICK_RATE_MS);
	}
}
void vButtonHandler3(void *pvParameters) {
	PORTF.DIRCLR = 0x40;
	uint8_t value = 3;
	uint8_t * p_value = &value;
	for(;;) {
		if((PORTF.IN & 0x40) == 0x00) {
				while((PORTF.IN & 0x40) == 0x00) {
					vTaskDelay(10);
				}
				xQueueSend(buttonqueue, p_value, 10);
		}
		vTaskDelay(50 / portTICK_RATE_MS);
	}
}
void vButtonHandler4(void *pvParameters) {
	PORTF.DIRCLR = 0x80;
	uint8_t value = 4;
	uint8_t * p_value = &value;
	for(;;) {
		if((PORTF.IN & 0x80) == 0x00) {
				while((PORTF.IN & 0x80) == 0x00) {
					vTaskDelay(10);
				}
				xQueueSend(buttonqueue, p_value, 10);
		}
		vTaskDelay(50 / portTICK_RATE_MS);
	}
}