/**
  ******************************************************************************
  * @file    main.c
  * @author  Vladimir Leonidov
  * @version V1.0.1
  * @date    28.11.2020
  * @brief   Лабораторная работа №5 - "ЦАП"
  *			 Отладочная плата: STM32F10C-EVAL
  *
  *			 Реализованы режимы работы ЦАП:
  *				- установка постоянного напряжения;
  *				- генерирование синусоидального сигнала (с помощью модуля DMA);
  *				- генерирование шума;
  *				- генерирование треугольного сигнала.
  *
  ******************************************************************************
  */

#include "main.h"
#include "mcu_config.h"

char RxBuffer[RX_BUFF_SIZE];						//Буфер приёма USART
char TxBuffer[TX_BUFF_SIZE];						//Буфер передачи USART
bool ComReceived;									//Флаг приёма команды

/**
  * @brief  Обработчик прерывания от USART2
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	char tmp;

	if ((USART2->SR & USART_SR_RXNE)!=0)			//Прерывание по приёму данных?
	{
		tmp=USART2->DR;
		if (tmp==0x0D)								//Если это символ конца строки
		{
			ComReceived=true;						//- выставляем флаг приёма строки
			return;									//- и выходим
		}
		RxBuffer[strlen(RxBuffer)] = tmp;			//Иначе добавляем его в буфер
	}
}

/**
  * @brief  Передача строки по USART2 с помощью DMA
  * @param  str - Указатель на строку
  * @retval None
  */
void txStrWithDMA(char *str)
{
	strcat(str,"\r");								//добавляем символ конца строки

	DMA1_Channel7->CCR &= ~DMA_CCR7_EN;				//выключаем DMA
	DMA1_Channel7->CMAR = (uint32_t)str;			//указатель на строку, которую нужно передать
	DMA1_Channel7->CNDTR = strlen(str);				//длина строки
	DMA1->IFCR |= DMA_IFCR_CTCIF7;					//сброс флага окончания обмена
	DMA1_Channel7->CCR |= DMA_CCR7_EN;    			//включить DMA
}

/**
  * @brief  Обработчик команд
  * @param  None
  * @retval None
  */
void ExecuteCommand(void)
{
	float F;			//Временная переменная для хранения чисел с плавающей точкой
	uint16_t d;			//Временная переменная для хранения целых чисел

	memset(&TxBuffer[0],0,sizeof(TxBuffer));		//Очистка буфера передачи
	
	/* Обработчик команд */
	if (strncmp(RxBuffer,"*IDN?",5)==0)				//Это команда "*IDN?"
	{
		strcpy(TxBuffer,"Lab 5 - DAC");				//Она самая, возвращаем строку идентификации	
	}
	else if (strncmp(RxBuffer,"U",1)==0)			//Команда установки постояннного напряжения
	{	
		sscanf(RxBuffer,"%*s %f", &F);				//преобразуем строку в float
		
		if ((0 <= F) && (F <= V_REF))
		{
			DAC->CR &= ~DAC_CR_WAVE1;				//выключить генерирование спец. сигналов
			DAC->CR &= ~DAC_CR_TEN1;				//выключить триггер ЦАП
			
			DMA2_Channel3->CCR &= ~DMA_CCR3_EN;		//выключаем DMA
			
			DAC->DHR12R1 = F*DAC_FS/V_REF;			//переводим Вольты в "попугаи" ЦАПа
		
			strcpy(TxBuffer,"OK");					//Отправляем обратно "ОК"			
		}
		else
			strcpy(TxBuffer,"Out of Range");		//если значение больше, чем опорное, возвращаем ошибку
	}
	else if (strncmp(RxBuffer,"SIN",3)==0)			//Команда генерирования синусоиды
	{	
		DAC->CR &= ~DAC_CR_WAVE1;					//выключить генерирование спец. сигналов
		DAC->CR &= ~DAC_CR_TEN1;					//выключить триггер ЦАП
		
		sscanf(RxBuffer,"%*s %hu", &d);				//преобразуем число в принятой команде в uint16_t
		
		TIM6->ARR = d;								//изменяем период таймера TIM6
		
		DMA2_Channel3->CCR |= DMA_CCR3_EN;			//Включить DMA
		
		strcpy(TxBuffer,"OK");						//Отправляем обратно "ОК"
	}
	else if (strncmp(RxBuffer,"NOISE",5)==0)		//Команда генерирования шума
	{	
		DMA2_Channel3->CCR &= ~DMA_CCR3_EN;			//Выключить DMA
		
		DAC->CR &= ~DAC_CR_WAVE1;					//очищаем биты генерирования спец. сигналов
		DAC->CR |= DAC_CR_WAVE1_0;					//включить генерирование шума
		DAC->CR |= DAC_CR_TEN1;						//включить триггер ЦАП1
		
		sscanf(RxBuffer,"%*s %hu", &d);				//преобразуем число в принятой команде в uint16_t
		
		TIM6->ARR = d;								//изменяем период таймера TIM6
		
		strcpy(TxBuffer,"OK");						//Отправляем обратно "ОК"
	}
	else if (strncmp(RxBuffer,"TRIANGLE",8)==0)		//Команда генерирования треугольного сигнала
	{		
		DMA2_Channel3->CCR &= ~DMA_CCR3_EN;			//Выключить DMA
		
		DAC->DHR12R1 = 0;							//Начальное значение для "треугольника" = 0В
		
		DAC->CR &= ~DAC_CR_WAVE1;					//очищаем биты генерирования спец. сигналов
		DAC->CR |= DAC_CR_WAVE1_1;					//включить генерирование треугольного сигнала
		DAC->CR |= DAC_CR_TEN1;						//включить триггер ЦАП1
		
		sscanf(RxBuffer,"%*s %hu", &d);				//преобразуем число в принятой команде в uint16_t
		
		TIM6->ARR = d;								//изменяем период таймера TIM6
		
		strcpy(TxBuffer,"OK");						//Отправляем обратно "ОК"
	}
	else
	  strcpy(TxBuffer,"Invalid Command");			//Если мы не знаем, чего от нас хотят, ругаемся в ответ
	
	txStrWithDMA(TxBuffer);							//Отправляем буефер передачи
	
	memset(RxBuffer,0,RX_BUFF_SIZE);				//Очищаем буфер приёма
	ComReceived = false;							//Сбрасываем флаг приёма строки	
}

/**
  * @brief  Основная программа
  * @param  None
  * @retval None
  */
int main(void)
{
	/*Инициализации всякие*/
	initPorts();
	initUART2();
	initUART2_DMA();
	initDAC1();
	initDAC1_DMA();
	initTIM6();
	
	/*Основной цикл*/
	while(true)
	{
		LED4_ON();
		
		if (ComReceived)			//Ждём приема строки
			ExecuteCommand();
		
		LED4_OFF();
	}
}
