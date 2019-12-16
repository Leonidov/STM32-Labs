/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 128 ];
osStaticThreadDef_t defaultTaskControlBlock;
osThreadId_t Blink1TaskHandle;
uint32_t Blink1TaskBuffer[ 128 ];
osStaticThreadDef_t Blink1TaskControlBlock;
osThreadId_t Blink2TaskHandle;
uint32_t Blink2TaskBuffer[ 128 ];
osStaticThreadDef_t Blink2TaskControlBlock;
osThreadId_t ButtonTaskHandle;
uint32_t ButtonTaskBuffer[ 128 ];
osStaticThreadDef_t ButtonTaskControlBlock;
osThreadId_t LED3TaskHandle;
uint32_t LED3TaskBuffer[ 128 ];
osStaticThreadDef_t LED3TaskControlBlock;
osSemaphoreId_t ButtonPressedSemHandle;
osStaticSemaphoreDef_t ButtonPressedSemControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void StartBlink1Task(void *argument);
void StartBlink2Task(void *argument);
void StartButtonTask(void *argument);
void StartLED3Task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */
osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of ButtonPressedSem */
  const osSemaphoreAttr_t ButtonPressedSem_attributes = {
    .name = "ButtonPressedSem",
    .cb_mem = &ButtonPressedSemControlBlock,
    .cb_size = sizeof(ButtonPressedSemControlBlock),
  };
  ButtonPressedSemHandle = osSemaphoreNew(1, 1, &ButtonPressedSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_mem = &defaultTaskBuffer[0],
    .stack_size = sizeof(defaultTaskBuffer),
    .cb_mem = &defaultTaskControlBlock,
    .cb_size = sizeof(defaultTaskControlBlock),
    .priority = (osPriority_t) osPriorityNormal,
  };
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* definition and creation of Blink1Task */
  const osThreadAttr_t Blink1Task_attributes = {
    .name = "Blink1Task",
    .stack_mem = &Blink1TaskBuffer[0],
    .stack_size = sizeof(Blink1TaskBuffer),
    .cb_mem = &Blink1TaskControlBlock,
    .cb_size = sizeof(Blink1TaskControlBlock),
    .priority = (osPriority_t) osPriorityLow,
  };
  Blink1TaskHandle = osThreadNew(StartBlink1Task, NULL, &Blink1Task_attributes);

  /* definition and creation of Blink2Task */
  const osThreadAttr_t Blink2Task_attributes = {
    .name = "Blink2Task",
    .stack_mem = &Blink2TaskBuffer[0],
    .stack_size = sizeof(Blink2TaskBuffer),
    .cb_mem = &Blink2TaskControlBlock,
    .cb_size = sizeof(Blink2TaskControlBlock),
    .priority = (osPriority_t) osPriorityLow,
  };
  Blink2TaskHandle = osThreadNew(StartBlink2Task, NULL, &Blink2Task_attributes);

  /* definition and creation of ButtonTask */
  const osThreadAttr_t ButtonTask_attributes = {
    .name = "ButtonTask",
    .stack_mem = &ButtonTaskBuffer[0],
    .stack_size = sizeof(ButtonTaskBuffer),
    .cb_mem = &ButtonTaskControlBlock,
    .cb_size = sizeof(ButtonTaskControlBlock),
    .priority = (osPriority_t) osPriorityLow,
  };
  ButtonTaskHandle = osThreadNew(StartButtonTask, NULL, &ButtonTask_attributes);

  /* definition and creation of LED3Task */
  const osThreadAttr_t LED3Task_attributes = {
    .name = "LED3Task",
    .stack_mem = &LED3TaskBuffer[0],
    .stack_size = sizeof(LED3TaskBuffer),
    .cb_mem = &LED3TaskControlBlock,
    .cb_size = sizeof(LED3TaskControlBlock),
    .priority = (osPriority_t) osPriorityLow,
  };
  LED3TaskHandle = osThreadNew(StartLED3Task, NULL, &LED3Task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartBlink1Task */
/**
* @brief Function implementing the Blink1Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBlink1Task */
void StartBlink1Task(void *argument)
{
  /* USER CODE BEGIN StartBlink1Task */
  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_TogglePin(LED0_GPIO_Port,LED0_Pin);
    osDelay(1000);
  }
  /* USER CODE END StartBlink1Task */
}

/* USER CODE BEGIN Header_StartBlink2Task */
/**
* @brief Function implementing the Blink2Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartBlink2Task */
void StartBlink2Task(void *argument)
{
  /* USER CODE BEGIN StartBlink2Task */
  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
    osDelay(2000);
  }
  /* USER CODE END StartBlink2Task */
}

/* USER CODE BEGIN Header_StartButtonTask */
/**
* @brief Function implementing the ButtonTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartButtonTask */
void StartButtonTask(void *argument)
{
  /* USER CODE BEGIN StartButtonTask */
  /* Infinite loop */
  for(;;)
  {
	if (HAL_GPIO_ReadPin(BUTTON_GPIO_Port,BUTTON_Pin))
	{
		osSemaphoreRelease(ButtonPressedSemHandle);
	}
    osDelay(100);
  }
  /* USER CODE END StartButtonTask */
}

/* USER CODE BEGIN Header_StartLED3Task */
/**
* @brief Function implementing the LED3Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLED3Task */
void StartLED3Task(void *argument)
{
  /* USER CODE BEGIN StartLED3Task */
  /* Infinite loop */
  for(;;)
  {
	  if (osSemaphoreAcquire(ButtonPressedSemHandle, osWaitForever) == osOK)
		 HAL_GPIO_TogglePin(LED3_GPIO_Port,LED3_Pin);
	  osThreadYield();
  }
  /* USER CODE END StartLED3Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
