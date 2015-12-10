/**
  ******************************************************************************
  * @file    mcu_config.c
  * @author  Vladimir Leonidov
  * @version V1.0.0
  * @date    07.12.2015
  * @brief   Подпрограммы инициализации и настройки периферии микроконтроллера.
  *
  ******************************************************************************
  */

#include "mcu_config.h"

/* Массив кодов ЦАП для синусоиды. Сгенерировано скриптом из папки ..\WaveGenerator */
uint16_t DAC_Data[64] = {2048, 2244, 2438, 2629, 2814, 2991, 3159, 3315, 3460, 3590, 
						3704, 3803, 3884, 3946, 3990, 4014, 4019, 4005, 3971, 3917, 
						3845, 3756, 3649, 3526, 3389, 3239, 3076, 2903, 2722, 2534, 
						2341, 2146, 1949, 1754, 1561, 1373, 1192, 1019, 856, 706, 
						569, 446, 339, 250, 178, 124, 90, 76, 81, 105, 
						149, 211, 292, 391, 505, 635, 780, 936, 1104, 1281, 
						1466, 1657, 1851, 2047};

/**
  * @brief  Инициализация портов ввода-вывода
  * @param  None
  * @retval None
  */
void initPorts(void)
{
	//Включить тактирование порта GPIOD
	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	
	// Сбрасываем биты конфигурации портов...
	GPIOD->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);
	GPIOD->CRL &= ~(GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
	GPIOD->CRL &= ~(GPIO_CRL_MODE7 | GPIO_CRL_CNF7);
	GPIOD->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
	
	//...и выставляем так, как нам нужно
	GPIOD->CRL |= GPIO_CRL_MODE4_1;		//LD4, выход 2МГц
	GPIOD->CRL |= GPIO_CRL_MODE3_1;		//LD3, выход 2МГц
	GPIOD->CRH |= GPIO_CRH_MODE13_1;	//LD2, выход 2МГц
	GPIOD->CRL |= GPIO_CRL_MODE7_1;		//LD1, выход 2МГц	
}

/**
  * @brief  Инициализация модуля USART2
  * @param  None
  * @retval None
  */
void initUART2(void)
{
	/*UART2 Remapped - PD5-TX, PD6-RX	*/
	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;				//включить тактирование GPIOD
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;				//включить тактирование альтернативных ф-ций портов
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;			//включить тактирование UART2
	
	GPIOD->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);		//PD5 на выход
	GPIOD->CRL |= (GPIO_CRL_MODE5_1 | GPIO_CRL_CNF5_1);
	
	GPIOD->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);		//PD6 - вход
	GPIOD->CRL |= GPIO_CRL_CNF6_0;
	
	AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;			//REMAP USART2 -> PD5,PD6
	
	/*****************************************
	Скорость передачи данных - 115200
	Частота шины APB1 - 36МГц
	
	1. USARTDIV = 36000000/(16*115200) = 19.5
	2. 19 = 0x13
	3. 16*0.5 = 8
	4. Итого 0x138
	*****************************************/
	USART2->BRR = 0x138;
	
	USART2->CR2 = 0;
	USART2->CR1 = 0;
	USART2->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
	USART2->CR1 |= USART_CR1_RXNEIE;				//разрешить прерывание по приему байта данных
	
	NVIC_EnableIRQ(USART2_IRQn);					//разрешить прерывание от модуля USART2
}

/**
  * @brief  Инициализация DMA для передачи данных по USART2
  * @param  None
  * @retval None
  */
void initUART2_DMA(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;				//разрешить такт. DMA
	
	DMA1_Channel7->CPAR = (uint32_t)&USART2->DR;	//указатель на регистр данных USART2
	
	DMA1_Channel7->CCR = 0;
	DMA1_Channel7->CCR |= DMA_CCR7_DIR;				//направление - из памяти в устройство
	DMA1_Channel7->CCR |= DMA_CCR7_MINC;			//инкремент указателя в памяти	
	USART2->CR3 |= USART_CR3_DMAT;					//настроить USART2 на работу с DMA
}

/**
  * @brief  Инициализация таймера TIM6 для работы с DAC+DMA
  * @param  None
  * @retval None
  */
void initTIM6(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;				//Включить тактирование TIM6
	
	//Частота APB1 для таймеров = APB1Clk * 2 = 36МГц * 2 = 72МГц
	TIM6->PSC = 72-1;								//Предделитель частоты (72МГц/72 = 1МГц)
	TIM6->ARR = 100-1;								//Модуль счёта таймера (1МГц/100 = 10кГц)
	TIM6->DIER |= TIM_DIER_UDE;						//Разрешить запрос DMA по переполнению таймера
	TIM6->CR2 |= TIM_CR2_MMS_1;						//TRGO будет срабатывать по переполнению..
													//...для функции генератора шума и треугольного сигнала
	TIM6->CR1 |= TIM_CR1_CEN;						//Запускаем TIM6 для формирования запросов DMA
}

/**
  * @brief  Инициализация модуля DMA, канал 3 для работы с ЦАП
  * @param  None
  * @retval None
  */
void initDAC1_DMA(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMA2EN;				//Включить тактирование DMA
	
	DMA2_Channel3->CCR = 0;							//Очищаем регистр конфигурации
	
	DMA2_Channel3->CPAR = (uint32_t)&DAC->DHR12R1;	//Указатель на 12-разрядынй регистр ЦАП
	DMA2_Channel3->CMAR = (uint32_t)DAC_Data;		//Указатель на массив значений ЦАПа
	DMA2_Channel3->CCR |= DMA_CCR3_DIR;				//Направление передачи - из памяти в периферию 
	
	//Кол-во элементов в массиве:
	DMA2_Channel3->CNDTR = sizeof(DAC_Data)/		//Общий объём массива в памяти
						   sizeof(DAC_Data[0]);		//разделить на размер одного элемента
	
	DMA2_Channel3->CCR |= DMA_CCR3_MINC;			//Включить инкремент указателя в памяти
	DMA2_Channel3->CCR |= DMA_CCR3_PSIZE_0;			//Размер данных в периферии - 16 бит
	DMA2_Channel3->CCR |= DMA_CCR3_MSIZE_0;			//Размер данных в памяти - 16 бит
	DMA2_Channel3->CCR |= DMA_CCR3_CIRC;			//Включить циклический режим
	DMA2_Channel3->CCR |= DMA_CCR3_PL;				//Приоритет повыше
}

/**
  * @brief  Инициализация модуля ЦАП
  * @param  None
  * @retval None
  */
void initDAC1(void)
{	
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;				//Включить тактирование альт. функций
	RCC->APB1ENR |= RCC_APB1ENR_DACEN;             	//Включить тактирование DAC
	
	DAC->CR |= 0;									//Очистить регистр конфигурации
	DAC->CR |= DAC_CR_DMAEN1;                 		//Разрешить работу ЦАП с DMA
	DAC->CR |= DAC_CR_MAMP1;						//выбираем максимальную амплитуду
													//для функции генератора шума и треугольника
	DAC->CR |= DAC_CR_EN1;							//Включить ЦАП1 (PA4)
}
