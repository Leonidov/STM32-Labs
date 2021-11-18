/**
  ******************************************************************************
  * @file    mcu_config.c
  * @author  Vladimir Leonidov
  * @version V1.0.0
  * @date    12.12.2015
  * @brief   Подпрограммы инициализации и настройки периферии микроконтроллера.
  *
  ******************************************************************************
  */

#include "mcu_config.h"
#include "main.h"

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
	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;			//включить тактирование GPIOD
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;			//включить тактирование альтернативных ф-ций портов
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;		//включить тактирование UART2
	
	GPIOD->CRL &= ~(GPIO_CRL_MODE5 | GPIO_CRL_CNF5);		//PD5 на выход
	GPIOD->CRL |= (GPIO_CRL_MODE5_1 | GPIO_CRL_CNF5_1);
	
	GPIOD->CRL &= ~(GPIO_CRL_MODE6 | GPIO_CRL_CNF6);		//PD6 - вход
	GPIOD->CRL |= GPIO_CRL_CNF6_0;
	
	AFIO->MAPR |= AFIO_MAPR_USART2_REMAP;		//REMAP USART2 -> PD5,PD6
	
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
* @brief  Инициализация АЦП ADC1 на работу с регулярной группой каналов
  *			- PB0 (ADC12_IN8) - вход на BNC разъёме
  * @param  None
  * @retval None
  */
void initADC1(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;					//Включить тактирование порта GPIOB
	
	GPIOB->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);	//PB0 на вход
	
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;  				//Включить тактирование АЦП

	ADC1->CR1 = 0;										//Обнуляем конфигурационные регистры
	ADC1->CR2 = 0;										//...
	ADC1->SQR1 = 0;										//...
	ADC1->SQR2 = 0;										//...
	ADC1->SQR3 = 8;										//Выбираем канал 8
	
	//Настройка времени преобразования каналов
	ADC1->SMPR2 |= ADC_SMPR2_SMP8;						//Канал 8 - 239.5 тактов					//Канал 14 - 239.5 тактов

	ADC1->CR2 |= ADC_CR2_EXTSEL_2;       				//Выбрать в качестве источника запуска TIM3 TRGO
	ADC1->CR2 |= ADC_CR2_EXTTRIG;      					//Разрешить внешний запуск регулярного канала
	ADC1->CR2 |= ADC_CR2_ADON;         					//Включить АЦП
	Delay(5);											//Задержка перед калибровкой
	ADC1->CR2 |= ADC_CR2_CAL;							//Запуск калибровки
	while (!(ADC1->CR2 & ADC_CR2_CAL)){};	 			//Ожидание окончания калибровки
}

/**
  * @brief  Инициализация DMA для работы с АЦП
  * @param  None
  * @retval None
  */
void initADC1_DMA(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;				//Разрешить тактирование DMA
	
	DMA1_Channel1->CPAR = (uint32_t)&ADC1->DR;		//Указатель на периферию - регистр данных АЦП
	DMA1_Channel1->CMAR = (uint32_t)ADC_Buffer;
	DMA1_Channel1->CNDTR = SAMPLES*2;
	
	DMA1_Channel1->CCR = 0;							//Очищаем регистр конфигурации
	
	DMA1_Channel1->CCR |= DMA_CCR1_CIRC; 			//Включаем циклический режим
	DMA1_Channel1->CCR |= DMA_CCR1_PSIZE_0;			//Разрядность данных в устройстве - 16 бит
	DMA1_Channel1->CCR |= DMA_CCR1_MSIZE;			//Разрядность данных в памяти - 32 бита
	DMA1_Channel1->CCR |= DMA_CCR1_MINC;			//Включить инкремент указателя в памяти
	DMA1_Channel1->CCR |= DMA_CCR1_TCIE; 			//Разрешить прерывание от заполнения всего буфера	
	DMA1_Channel1->CCR |= DMA_CCR1_HTIE;			//Разрешить прерывание от заполнения половины буфера
	
	ADC1->CR2 |= ADC_CR2_DMA;						//Разрешить работу АЦП с DMA
	
	DMA1_Channel1->CCR |= DMA_CCR1_EN;				//включить DMA
	
	NVIC_EnableIRQ(DMA1_Channel1_IRQn);				//Разрешаем прерывание по DMA
	NVIC_SetPriority(DMA1_Channel1_IRQn, 4);		//Ставим приоритет пониже, иначе может мешать USARTу
}

/**
  * @brief  Инициализация таймера TIM3 для запуска АЦП 
  * @param  None
  * @retval None
  */
void initTIM3(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;				//включить тактирование таймера TIM3

	TIM3->PSC = 0;									//Предделитель
	TIM3->ARR = 720;								//Модуль счёта - 720, частота переполнения - 100кГц
	
	TIM3->CR1 = 0;									//Очищаем регистр конфигурации таймера CR1
	TIM3->CR2 = 0;									//Очищаем регистр конфигурации таймера CR2
	TIM3->CR2 |= TIM_CR2_MMS_1;						//TRGO выставляется по переполнению таймера
	TIM3->CR1 |= TIM_CR1_CEN;						//Включить тактирование таймера
}

/**
  * @brief  Подпрограмма задержки
  * @param  takts - Кол-во тактов
  * @retval None
  */
void Delay(uint32_t takts)
{
	uint32_t i;
	
	for (i = 0; i < takts; i++) {};
}
