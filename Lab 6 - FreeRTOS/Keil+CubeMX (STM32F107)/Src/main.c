/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2015 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId ScanUserKeysHandle;
osThreadId TaskLEDSHandle;
osThreadId TaskLEDBlinkHandle;
osThreadId ScanWakeUpKeyHandle;
osMessageQId PressedBtnHandle;
osSemaphoreId WakeUpKeyPressedHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void StartDefaultTask(void const * argument);
void StartScanUserKeys(void const * argument);
void StartTaskLEDS(void const * argument);
void StartLEDBlink(void const * argument);
void StartScanWakeUpKey(void const * argument);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of WakeUpKeyPressed */
  osSemaphoreDef(WakeUpKeyPressed);
  WakeUpKeyPressedHandle = osSemaphoreCreate(osSemaphore(WakeUpKeyPressed), 5);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of ScanUserKeys */
  osThreadDef(ScanUserKeys, StartScanUserKeys, osPriorityNormal, 0, 128);
  ScanUserKeysHandle = osThreadCreate(osThread(ScanUserKeys), NULL);

  /* definition and creation of TaskLEDS */
  osThreadDef(TaskLEDS, StartTaskLEDS, osPriorityNormal, 0, 128);
  TaskLEDSHandle = osThreadCreate(osThread(TaskLEDS), NULL);

  /* definition and creation of TaskLEDBlink */
  osThreadDef(TaskLEDBlink, StartLEDBlink, osPriorityNormal, 0, 128);
  TaskLEDBlinkHandle = osThreadCreate(osThread(TaskLEDBlink), NULL);

  /* definition and creation of ScanWakeUpKey */
  osThreadDef(ScanWakeUpKey, StartScanWakeUpKey, osPriorityNormal, 0, 128);
  ScanWakeUpKeyHandle = osThreadCreate(osThread(ScanWakeUpKey), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of PressedBtn */
  osMessageQDef(PressedBtn, 16, uint8_t);
  PressedBtnHandle = osMessageCreate(osMessageQ(PressedBtn), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV5;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

  HAL_RCC_EnableCSS();

  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  __HAL_RCC_PLLI2S_ENABLE();

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PD13 PD3 PD4 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_4);
	  
	osDelay(100);
  }
  /* USER CODE END 5 */ 
}

/* StartScanUserKeys function */
void StartScanUserKeys(void const * argument)
{
  /* USER CODE BEGIN StartScanUserKeys */
  /* Infinite loop */
	for(;;)
	{
		if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_9) == GPIO_PIN_RESET)
		{
			//Если нажата кнопка UserKey, пишем в очередь 1
			osMessagePut(PressedBtnHandle, 1, 10);
		}
		else if (HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) == GPIO_PIN_RESET)
		{
			//Если нажата кнопка Tamper, пишем в очередь 2
			osMessagePut(PressedBtnHandle, 2, 10);		
		}
		osDelay(100);
	}
  /* USER CODE END StartScanUserKeys */
}

/* StartTaskLEDS function */
void StartTaskLEDS(void const * argument)
{
  /* USER CODE BEGIN StartTaskLEDS */
  /* Infinite loop */

	uint32_t BtnCode;
	osEvent QueueEvent;
	
	for(;;)
	{
		QueueEvent = osMessageGet(PressedBtnHandle,10);	//Читаем сообщение ОС
		if (QueueEvent.status == osEventMessage)		//Если прочитали
		{ 
			BtnCode = (uint32_t)QueueEvent.value.p;		//Читаем содержимое очереди
			switch (BtnCode)							//Выполняем действие
			{											//в зависимости от содержимого очереди
				case 1:
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_7);
					break;
				case 2:
					HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13);
					break;
				default:
					break;
			}
		}
		osDelay(1);
	}
  /* USER CODE END StartTaskLEDS */
}

/* StartLEDBlink function */
void StartLEDBlink(void const * argument)
{
  /* USER CODE BEGIN StartLEDBlink */
  /* Infinite loop */
  for(;;)
  {
	//Ждём семафор WakeUpKeyPressed
	xSemaphoreTake(WakeUpKeyPressedHandle, osWaitForever);
	HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_3);
    osDelay(500);
  }
  /* USER CODE END StartLEDBlink */
}

/* StartScanWakeUpKey function */
void StartScanWakeUpKey(void const * argument)
{
  /* USER CODE BEGIN StartScanWakeUpKey */
  /* Infinite loop */
  for(;;)
  {
	if (HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0) == GPIO_PIN_SET)
	{
		//Устанавливаем семафор WakeUpKeyPressed
		xSemaphoreGive(WakeUpKeyPressedHandle);		
	}
    osDelay(100);
  }
  /* USER CODE END StartScanWakeUpKey */
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
