/**
  ******************************************************************************
  * @file    mcu_config.c
  * @author  Vladimir Leonidov
  * @version V1.0.1
  * @date    28.11.2020
  * @brief   Подпрограммы инициализации и настройки периферии микроконтроллера.
  *
  ******************************************************************************
  */

#include "mcu_config.h"

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
	
	DMA1_Channel7->CCR |= DMA_CCR7_DIR;				//направление - из памяти в устройство
	DMA1_Channel7->CCR |= DMA_CCR7_MINC;			//инкремент указателя в памяти	
	USART2->CR3 |= USART_CR3_DMAT;					//настроить USART2 на работу с DMA
}

/**
* @brief  Инициализация АЦП ADC1 на работу с регулярной группой каналов
  *			- к PC4 (ADC12_IN14) подключён потенциометр
  *			- к каналу ADC1_IN16 подключён встроенный термодатчик 
  * @param  None
  * @retval None
  */
void initADC1_Regular(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;					//Включить тактирование порта GPIOC
	
	GPIOC->CRL &= ~(GPIO_CRL_MODE4 | GPIO_CRL_CNF4);	//PC4 на вход
	
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;  				//Включить тактирование АЦП
	
	//Настройка времени преобразования каналов
	ADC1->SMPR1 |= ADC_SMPR1_SMP14;						//Канал 14 - 239.5 тактов
	ADC1->SMPR1 |= ADC_SMPR1_SMP16;						//Канал 16 - 239.5 тактов

	ADC1->CR2 |= ADC_CR2_TSVREFE;						//Подключить термодатчик к каналу ADC1_IN16
	ADC1->CR2 |= ADC_CR2_EXTSEL;       					//Выбрать в качестве источника запуска SWSTART
	ADC1->CR2 |= ADC_CR2_EXTTRIG;      					//Разрешить внешний запуск регулярного канала
	ADC1->CR2 |= ADC_CR2_ADON;         					//Включить АЦП
	
	Delay(5);											//Задержка перед калибровкой
	ADC1->CR2 |= ADC_CR2_CAL;							//Запуск калибровки
	while (ADC1->CR2 & ADC_CR2_CAL){};	 				//Ожидание окончания калибровки
}

/**
  * @brief  Подпрограмма задержки
  * @param  takts - Кол-во тактов
  * @retval None
  */
void Delay(uint32_t takts)
{
	volatile uint32_t i;
	
	for (i = 0; i < takts; i++) {};
}
