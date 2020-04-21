#include <stdio.h>
#include <conio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
//Necessary to allow the mutex to stall indefinitly
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
// The tasks as described in the comments at the top of this file.
static SemaphoreHandle_t printLock;
static SemaphoreHandle_t chrisAwakeLock;
static SemaphoreHandle_t queueLock;
static QueueHandle_t OHqueue;
static const int queueSize = 5;
static TaskHandle_t chrisHandle;
static int chrisAwake = 0;
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
			concurrency_safe_print(name);
			concurrency_safe_print(" went from going about life to being helped by Chris\n");
		}
		else {
			xSemaphoreTake(queueLock, portMAX_DELAY);
			if (uxQueueSpacesAvailable(OHqueue) > 0) {
				TaskHandle_t currHandle = xTaskGetCurrentTaskHandle();
				xQueueSend(OHqueue, &(currHandle), 0);
				wasHelped = 1;
			}
			xSemaphoreGive(queueLock);
		}
		xSemaphoreGive(chrisAwakeLock);
		
		if (wasHelped) {
			// Suspend thread if waiting in queue
			if (wasHelped == 1) {
				concurrency_safe_print(name);
				concurrency_safe_print(" went from going about life to waiting in OH\n");
				vTaskSuspend(xTaskGetCurrentTaskHandle());
				concurrency_safe_print(name);
				concurrency_safe_print(" went from waiting in OH to being helped by Chris\n");
			}
			concurrency_safe_print(name);
			concurrency_safe_print(" went from being helped by Chris to going about life\n");
			// Wait if helped
			int delay_time = rand() % 5 + 3;
			vTaskDelay(delay_time * second);
		}
		else {
			concurrency_safe_print(name);
			concurrency_safe_print(" went from going about life to going about life\n");
			// Wait if not helped
			int delay_time = rand() % 5 + 1;
			vTaskDelay(delay_time * second);
		}
	}
}

static void chris() {

	chrisHandle = xTaskGetCurrentTaskHandle();

	while (1) {
		int queueEmpty = 0;
		xSemaphoreTake(queueLock, portMAX_DELAY);
		if (uxQueueSpacesAvailable(OHqueue) - queueSize <= 0) {
			concurrency_safe_print("QUEUE EMPTY\n");
			queueEmpty = 1;
		}
		else {
			// Create Temp TaskHandle
			TaskHandle_t temp = NULL;
			// Load student from queue into buffer
			xQueueReceive(OHqueue, &(temp), 0);
			// Wake up Student
			vTaskResume(temp);
		}
		xSemaphoreGive(queueLock);
		if (queueEmpty) {
			xSemaphoreTake(chrisAwakeLock, portMAX_DELAY);
			chrisAwake = 0;
			concurrency_safe_print("Chris went from helping students to playing Sudoku\n");
			xSemaphoreGive(chrisAwakeLock);
			vTaskSuspend(chrisHandle);
			concurrency_safe_print("Chris went from playing Sudoku to helping students\n");
		}
	}
}

static void chrisTask(void *pvParameters)
{
	(void)pvParameters;
	chris();
}

static void shriyash(void *pvParameters)
{
	(void)pvParameters;
	student("Shriyash");
}

static void richard(void *pvParameters)
{
	(void)pvParameters;
	student("Richard");
}

static void lakshay(void *pvParameters)
{
	(void)pvParameters;
	student("Lakshay");
}

void main_blinky(void)
{
	printLock = xSemaphoreCreateMutex();
	chrisAwakeLock = xSemaphoreCreateMutex();
	queueLock = xSemaphoreCreateMutex();
	OHqueue = xQueueCreate(queueSize, sizeof(TaskHandle_t));

	if ((printLock != NULL) && (chrisAwakeLock != NULL) && (queueLock != NULL) && (OHqueue != NULL))
	{
		xTaskCreate(chrisTask, "Chris", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
		xTaskCreate(shriyash, "Shriyash", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
		xTaskCreate(richard, "Richard", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
		xTaskCreate(lakshay, "Lakshay", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
		vTaskStartScheduler();
	}
	printf("ERROR: Should not get here\r\n");
	for (;; );
}