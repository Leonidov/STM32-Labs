/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
typedef StaticQueue_t osStaticMessageQDef_t;
typedef StaticSemaphore_t osStaticSemaphoreDef_t;
/* USER CODE BEGIN PTD */

typedef struct {
	char Buf[128];
} QUEUE_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
uint32_t defaultTaskBuffer[ 128 ];
osStaticThreadDef_t defaultTaskControlBlock;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_mem = &defaultTaskBuffer[0],
  .stack_size = sizeof(defaultTaskBuffer),
  .cb_mem = &defaultTaskControlBlock,
  .cb_size = sizeof(defaultTaskControlBlock),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LED1Task */
osThreadId_t LED1TaskHandle;
uint32_t LED1TaskBuffer[ 128 ];
osStaticThreadDef_t LED1TaskControlBlock;
const osThreadAttr_t LED1Task_attributes = {
  .name = "LED1Task",
  .stack_mem = &LED1TaskBuffer[0],
  .stack_size = sizeof(LED1TaskBuffer),
  .cb_mem = &LED1TaskControlBlock,
  .cb_size = sizeof(LED1TaskControlBlock),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for LED2Task */
osThreadId_t LED2TaskHandle;
uint32_t LED2TaskBuffer[ 128 ];
osStaticThreadDef_t LED2TaskControlBlock;
const osThreadAttr_t LED2Task_attributes = {
  .name = "LED2Task",
  .stack_mem = &LED2TaskBuffer[0],
  .stack_size = sizeof(LED2TaskBuffer),
  .cb_mem = &LED2TaskControlBlock,
  .cb_size = sizeof(LED2TaskControlBlock),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ReadBtnTask */
osThreadId_t ReadBtnTaskHandle;
uint32_t ReadBtnTaskBuffer[ 128 ];
osStaticThreadDef_t ReadBtnTaskControlBlock;
const osThreadAttr_t ReadBtnTask_attributes = {
  .name = "ReadBtnTask",
  .stack_mem = &ReadBtnTaskBuffer[0],
  .stack_size = sizeof(ReadBtnTaskBuffer),
  .cb_mem = &ReadBtnTaskControlBlock,
  .cb_size = sizeof(ReadBtnTaskControlBlock),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for LED3Task */
osThreadId_t LED3TaskHandle;
uint32_t LED3TaskBuffer[ 128 ];
osStaticThreadDef_t LED3TaskControlBlock;
const osThreadAttr_t LED3Task_attributes = {
  .name = "LED3Task",
  .stack_mem = &LED3TaskBuffer[0],
  .stack_size = sizeof(LED3TaskBuffer),
  .cb_mem = &LED3TaskControlBlock,
  .cb_size = sizeof(LED3TaskControlBlock),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for ADCTask */
osThreadId_t ADCTaskHandle;
uint32_t ADCTaskBuffer[ 128 ];
osStaticThreadDef_t ADCTaskControlBlock;
const osThreadAttr_t ADCTask_attributes = {
  .name = "ADCTask",
  .stack_mem = &ADCTaskBuffer[0],
  .stack_size = sizeof(ADCTaskBuffer),
  .cb_mem = &ADCTaskControlBlock,
  .cb_size = sizeof(ADCTaskControlBlock),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for UARTTask */
osThreadId_t UARTTaskHandle;
uint32_t UARTTaskBuffer[ 128 ];
osStaticThreadDef_t UARTTaskControlBlock;
const osThreadAttr_t UARTTask_attributes = {
  .name = "UARTTask",
  .stack_mem = &UARTTaskBuffer[0],
  .stack_size = sizeof(UARTTaskBuffer),
  .cb_mem = &UARTTaskControlBlock,
  .cb_size = sizeof(UARTTaskControlBlock),
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for UARTQueue */
osMessageQueueId_t UARTQueueHandle;
uint8_t UARTQueueBuffer[ 10 * sizeof( QUEUE_t ) ];
osStaticMessageQDef_t UARTQueueControlBlock;
const osMessageQueueAttr_t UARTQueue_attributes = {
  .name = "UARTQueue",
  .cb_mem = &UARTQueueControlBlock,
  .cb_size = sizeof(UARTQueueControlBlock),
  .mq_mem = &UARTQueueBuffer,
  .mq_size = sizeof(UARTQueueBuffer)
};
/* Definitions for BtnSem */
osSemaphoreId_t BtnSemHandle;
osStaticSemaphoreDef_t BtnSemControlBlock;
const osSemaphoreAttr_t BtnSem_attributes = {
  .name = "BtnSem",
  .cb_mem = &BtnSemControlBlock,
  .cb_size = sizeof(BtnSemControlBlock),
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM4_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
void StartDefaultTask(void *argument);
void StartLED1Task(void *argument);
void StartLED2Task(void *argument);
void StartReadBtnTask(void *argument);
void StartLED3Task(void *argument);
void StartADCTask(void *argument);
void StartUARTTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */

  HAL_ADC_Start(&hadc1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of BtnSem */
  BtnSemHandle = osSemaphoreNew(1, 0, &BtnSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of UARTQueue */
  UARTQueueHandle = osMessageQueueNew (10, sizeof(QUEUE_t), &UARTQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of LED1Task */
  LED1TaskHandle = osThreadNew(StartLED1Task, NULL, &LED1Task_attributes);

  /* creation of LED2Task */
  LED2TaskHandle = osThreadNew(StartLED2Task, NULL, &LED2Task_attributes);

  /* creation of ReadBtnTask */
  ReadBtnTaskHandle = osThreadNew(StartReadBtnTask, NULL, &ReadBtnTask_attributes);

  /* creation of LED3Task */
  LED3TaskHandle = osThreadNew(StartLED3Task, NULL, &LED3Task_attributes);

  /* creation of ADCTask */
  ADCTaskHandle = osThreadNew(StartADCTask, NULL, &ADCTask_attributes);

  /* creation of UARTTask */
  UARTTaskHandle = osThreadNew(StartUARTTask, NULL, &UARTTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

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

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV5;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.Prediv1Source = RCC_PREDIV1_SOURCE_PLL2;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL2.PLL2State = RCC_PLL2_ON;
  RCC_OscInitStruct.PLL2.PLL2MUL = RCC_PLL2_MUL8;
  RCC_OscInitStruct.PLL2.HSEPrediv2Value = RCC_HSE_PREDIV2_DIV5;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Enables the Clock Security System
  */
  HAL_RCC_EnableCSS();
  /** Configure the Systick interrupt time
  */
  __HAL_RCC_PLLI2S_ENABLE();
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_14;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_1CYCLE_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 300;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 4095;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED1_Pin|LED2_Pin|LED3_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : Btn_Pin */
  GPIO_InitStruct.Pin = Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Btn_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartLED1Task */
/**
* @brief Function implementing the LED1Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLED1Task */
void StartLED1Task(void *argument)
{
  /* USER CODE BEGIN StartLED1Task */
  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
    osDelay(1000);
  }
  /* USER CODE END StartLED1Task */
}

/* USER CODE BEGIN Header_StartLED2Task */
/**
* @brief Function implementing the LED2Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartLED2Task */
void StartLED2Task(void *argument)
{
  /* USER CODE BEGIN StartLED2Task */
  QUEUE_t msg;
  /* Infinite loop */
  for(;;)
  {
	HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);

	strcpy(msg.Buf,"LED2 Blink\r\n\0");
	osMessageQueuePut(UARTQueueHandle, &msg, 0, osWaitForever);

    osDelay(2000);
  }
  /* USER CODE END StartLED2Task */
}

/* USER CODE BEGIN Header_StartReadBtnTask */
/**
* @brief Function implementing the ReadBtnTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartReadBtnTask */
void StartReadBtnTask(void *argument)
{
  /* USER CODE BEGIN StartReadBtnTask */
  QUEUE_t msg;
  /* Infinite loop */
  for(;;)
  {
	if (!HAL_GPIO_ReadPin(Btn_GPIO_Port, Btn_Pin))
	{
		osSemaphoreRelease(BtnSemHandle);
		strcpy(msg.Buf,"Btn Pressed\r\n\0");
		osMessageQueuePut(UARTQueueHandle, &msg, 0, osWaitForever);
	}
    osDelay(500);
  }
  /* USER CODE END StartReadBtnTask */
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
	if (osSemaphoreAcquire(BtnSemHandle, osWaitForever) == osOK)
	{
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
	}
    osDelay(1);
  }
  /* USER CODE END StartLED3Task */
}

/* USER CODE BEGIN Header_StartADCTask */
/**
* @brief Function implementing the ADCTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartADCTask */
void StartADCTask(void *argument)
{
  /* USER CODE BEGIN StartADCTask */
  QUEUE_t msg;
  /* Infinite loop */
  for(;;)
  {
    uint16_t adc_res = HAL_ADC_GetValue(&hadc1);
    __HAL_TIM_SET_COMPARE(&htim4,TIM_CHANNEL_2,adc_res);

    if (adc_res == 0)
    {
    	strcpy(msg.Buf,"MIN ADC\r\n\0");
    	osMessageQueuePut(UARTQueueHandle, &msg, 0, osWaitForever);
    	osDelay(500);
    }
    else if (adc_res > 4000)
    {
    	strcpy(msg.Buf,"MAX ADC\r\n\0");
    	osMessageQueuePut(UARTQueueHandle, &msg, 0, osWaitForever);
    	osDelay(500);
    }

	osDelay(100);
  }
  /* USER CODE END StartADCTask */
}

/* USER CODE BEGIN Header_StartUARTTask */
/**
* @brief Function implementing the UARTTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartUARTTask */
void StartUARTTask(void *argument)
{
  /* USER CODE BEGIN StartUARTTask */
  QUEUE_t msg;
  /* Infinite loop */
  for(;;)
  {
    osMessageQueueGet(UARTQueueHandle, &msg, 0, osWaitForever);
    HAL_UART_Transmit(&huart2, (uint8_t*)msg.Buf, strlen(msg.Buf), osWaitForever);
	osDelay(1);
  }
  /* USER CODE END StartUARTTask */
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
