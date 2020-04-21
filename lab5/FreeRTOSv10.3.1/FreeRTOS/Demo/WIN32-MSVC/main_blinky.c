#include <stdio.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
//Necessary to allow the mutex to stall indefinitly
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelay                      1
// The tasks as described in the comments at the top of this file.
static SemaphoreHandle_t printLock;
static SemaphoreHandle_t chrisAwakeLock;
static SemaphoreHandle_t queueLock;
static QueueHandle_t OHqueue;
static const int queueSize = 5;
static TaskHandle_t chrisHandle;
static int chrisAwake;
const TickType_t second = 1000 / portTICK_PERIOD_MS;

static void concurrency_safe_print(char* strToPrint) {
	xSemaphoreTake(printLock, portMAX_DELAY);
	printf("%s", strToPrint);
	xSemaphoreGive(printLock);
}

static void student(char* name) {
	while (1) {
		int wasHelped = 0;
		xSemaphoreTake(chrisAwakeLock, portMAX_DELAY);
		if (!chrisAwake) {
			chrisAwake = 1;
			vTaskResume(chrisHandle);
			wasHelped = 2; // Make sure the student helped by Chris does not sleep eternally
		}
		else {
			xSemaphoreTake(queueLock, portMAX_DELAY);
			if (uxQueueSpacesAvailable(OHqueue) > 0) {
				xQueueSend(OHqueue, xTaskGetCurrentTaskHandle(), 0);
				wasHelped = 1;
			}
			xSemaphoreGive(queueLock);
		}
		xSemaphoreGive(chrisAwakeLock);
		
		if (wasHelped) {
			// Suspend thread if waiting in queue
			if (wasHelped == 1) {
				vTaskSuspend(xTaskGetCurrentTaskHandle());
			}

			// Wait if helped
			int delay_time = rand() % 5 + 3;
			vTaskDelay(delay_time * second);
		}
		else {
			// Wait if not helped
			int delay_time = rand() % 5 + 1;
			vTaskDelay(delay_time * second);
		}
	}
}

static void chris() {
	while (1) {
		int queueEmpty = 0;
		xSemaphoreTake(queueLock, portMAX_DELAY);
		if (uxQueueSpacesAvailable(OHqueue) == queueSize) {
			queueEmpty = 1;
		}
		else {
			// Create Temp TaskHandle
			TaskHandle_t* temp;
			// Load student from queue into buffer
			temp = xQueueReceive(OHqueue, temp, 0);
			// Wake up Student
			vTaskResume(temp);
		}
		xSemaphoreGive(queueLock);
		if (queueEmpty) {
			xSemaphoreTake(chrisAwakeLock, portMAX_DELAY);
			chrisAwake = 0;
			xSemaphoreGive(chrisAwakeLock);
			vTaskSuspend(chrisHandle);
		}
	}
}

void main_blinky(void)
{
	accountMutex = xSemaphoreCreateMutex();
	if (accountMutex != NULL)
	{
		xTaskCreate(withdraw100Task,		/* The function that implements the task. */
			"withdraw100Task", 	/* The text name assigned to the task - for debug only as it is not used by the kernel. */
			configMINIMAL_STACK_SIZE, 	/* The size of the stack to allocate to the task. */
			NULL, 			/* The parameter passed to the task - not used in this simple case. */
			tskIDLE_PRIORITY + 1,	/* The priority assigned to the task. */
			NULL);			/* The task handle is not required, so NULL is passed. */

		xTaskCreate(deposite200Task, "deposite200Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
		vTaskStartScheduler();
	}
	printf("ERROR: Should not get here\r\n");
	for (;; );
}
static void deposite200Task(void *pvParameters)
{
	(void)pvParameters;
	deposit(200);
}

static void withdraw100Task(void *pvParameters)
{
	(void)pvParameters;
	withdraw(100);
}
