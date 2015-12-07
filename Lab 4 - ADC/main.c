/**
  ******************************************************************************
  * @file    main.c
  * @author  Vladimir Leonidov
  * @version V1.0.0
  * @date    07.12.2015
  * @brief   Лабораторная работа №4 - "АЦП"
  *			 Отладочная плата: STM32F10C-EVAL
  *
  *			 Реализована работа с регулярной группой каналов АЦП ADC1:
  *			 	- ADC1_IN14 - подключен к потенциометру
  *				- ADC1_IN16 - внутренний канал, подключен к встроенному термодатчику
  *			 Реализован простейший обработчик SCPI-подобных команд
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
  * @brief  Чтение результата АЦПирования
  * @param  n - номер канала
  * @retval Результат измерения в "попугаях" шкалы АЦП:
  * 		4095 - 3.3В
  *			   0 - 0.0В
  */
uint16_t Read_ADC(uint8_t n)
{ 
	ADC1->SQR3 = n;									//Записываем номер канала в регистр SQR3
	ADC1->SR &= ~ADC_SR_EOC;						//Сбрасываем флаг окончания преобразования
	ADC1->CR2 |= ADC_CR2_SWSTART;					//Запускаем преобразование в регулярном канале   
	while(!(ADC1->SR & ADC_SR_EOC)){};				//Ждем окончания преобразования
	return ADC1->DR;								//Читаем результат
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
	uint16_t adc_res;	//Сюда будем считывать резултаты измерения АЦП
	float F;			//Временная переменная для хранения чисел с плавающей точкой
	char str[255];		//Временная строка

	memset(&TxBuffer[0],0,sizeof(TxBuffer));		//Очистка буфера передачи
	
	/* Обработчик команд */
	if (strncmp(RxBuffer,"*IDN?",5)==0)				//Это команда "*IDN?"
	{
		strcpy(TxBuffer,"Lab 4 - ADC");				//Она самая, возвращаем строку идентификации	
	}
	else if (strncmp(RxBuffer,"LED1",4)==0)			//Это команда управления светодиодом?
	{
		// Дальше должен следовать пробел и команда "ON" или "OFF"
		// Удаляем "LED1 " (с пробелом)
		memmove(RxBuffer, RxBuffer+5, strlen(RxBuffer)-5+1);

		// Теперь буфер должен начинаться с "ON" или "OFF"
		if (strncmp(RxBuffer,"ON",3)==0)			//Проверяем, далее следует "ON"?
		{
			LED1_ON();								//Да, включаем светодиод...
			strcpy(TxBuffer,"OK");					//...и отправляем обратно "OK"
		}
		else if (strncmp(RxBuffer,"OFF",4)==0)		//Может быть там всё-таки "OFF"?
		{
			LED1_OFF();								//Так и есть!
			strcpy(TxBuffer,"OK");					//Выключаем этот чёртов светодиод
		}
		else
			strcpy(TxBuffer,"Invalid Parameter");	//Нам шлют непонятные символы, пишем ошибку в ответ	
	}
	else if (strncmp(RxBuffer,"ADC?",4)==0)			//Команда измерения напряжения на потенциометре
	{
		adc_res = Read_ADC(14);						//Измеряем напряжение (у нас 14 канал)
		F = adc_res*V_REF/ADC_FS;					//Преобразуем из "попугаев" в Вольты
		sprintf(str, "%1.2f", F);					//Преобразуем результат в строку, два знака после запятой
		strcpy(TxBuffer, str);						//Копируем результат в буфер передачи	
	}
	else if (strncmp(RxBuffer,"TEMPER?",7)==0)		//Команда измерения температуры процессора?
	{
		adc_res = Read_ADC(16);						//Да, выбираем канал 16 (к нему подключён внутреннийтермодатчик)
		F = adc_res/(float)ADC_FS*V_REF;			//Преобразуем "попугаи" в Вольты
		F = (V25 - F)/AVG_SLOPE + 25.0;				//Преобразуем из Вольт в градусы Цельсия
		sprintf(str, "%1.1f", F);					//Преобразуем результат в строку, один знак после запятой
		strcpy(TxBuffer, str);						//Копируем результат в буфер передачи
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
	initADC1_Regular();
	
	/*Основной цикл*/
	while(true)
	{
		LED4_ON();
		
		if (ComReceived)			//Ждём приема строки
			ExecuteCommand();
		
		LED4_OFF();
	}
}

