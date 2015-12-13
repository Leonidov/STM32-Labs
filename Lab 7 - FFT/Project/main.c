/**
  ******************************************************************************
  * @file    main.c
  * @author  Vladimir Leonidov
  * @version V1.0.0
  * @date    12.12.2015
  * @brief   Лабораторная работа №7 - "Быстрое преобразование Фурье"
  *			 Отладочная плата: STM32F10C-EVAL
  *
  *			 Реализована периодическая дискретизация сигнала с частотой 100кГц с использованием связки АЦП+DMA.
  *			 Для хранения дискретов выделен буфер ADC_Buffer из 512 элементов.
  *			 Алгоритм работы:
  *				1. Производится заполнение первой половины буфера АЦП ADC_Buffer
  *				   (256 из 512 элементов), после чего генерируется прерывание от DMA (`DMA_ISR_HTIF1`).
  *				2. Производится ДПФ от первой половины буфера АЦП,
  *				   в это время заполняется вторая половина буфера.
  *				3. После заполнения всего буфера из 512 отсчётов DMA генерирует второе прерывание (`DMA_ISR_TCIF1`).
  *				4. Производится ДПФ от второй половины буфера АЦП, в это время заполняется первая половина буфера.
  *				5. Пункты 1-4 повторяются.
  *
  ******************************************************************************
  */

#include "main.h"
#include "mcu_config.h"

uint32_t ADC_Buffer[SAMPLES*2];						//Буфер АЦП (в 2 раза больше, чем кол-во отсчётов)
int32_t	FFT_Result[SAMPLES];						//Буфер результатов БПФ
bool TopOfBuffer;									//Показывает, какая часть буфера АЦП сейчас актуальна
													// true - верхняя; false - нижняя

char RxBuffer[RX_BUFF_SIZE];						//Буфер приёма USART
char TxBuffer[TX_BUFF_SIZE];						//Буфер передачи USART
bool ComReceived;									//Флаг приёма команды

/**
  * @brief  Обработчик прерывания от DMA1
  * @param  None
  * @retval None
  */
void DMA1_Channel1_IRQHandler(void)
{
	if (DMA1->ISR & DMA_ISR_TCIF1)					//Прерывание по заполнению всего буфера
	{
		DMA1->IFCR |= DMA_ISR_TCIF1;				//Сбрасываем флаг прерывания
		
		//Производим БПФ от верхней половины буфера
		cr4_fft_256_stm32(FFT_Result, &ADC_Buffer[SAMPLES], SAMPLES);	
		TopOfBuffer = true;
	}
	else if (DMA1->ISR & DMA_ISR_HTIF1)				//Прерывание по заполнению половины буфера
	{
		DMA1->IFCR |= DMA_ISR_HTIF1;				//Сбрасываем флаг прерывания
		
		//Поизводим БПФ от нижней половины буфера
		cr4_fft_256_stm32(FFT_Result, ADC_Buffer, SAMPLES);		
		TopOfBuffer = false;
	}
}

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
	uint16_t i;
	int16_t Real, Imag;
	int32_t Res;
	float A;
	char str[255];

	memset(&TxBuffer[0], 0, sizeof(TxBuffer));			//Очистка буфера передачи
	
	/* Обработчик команд */
	if (strncmp(RxBuffer, "*IDN?", 5) == 0)				//Это команда "*IDN?"
	{
		strcpy(TxBuffer, "Lab 7 - FFT");				//Она самая, возвращаем строку идентификации	
	}
	else if (strncmp(RxBuffer, "ADC?", 4) == 0)			//Считать массив отсчётов во временной области
	{
		DMA1_Channel1->CCR &= ~DMA_CCR1_EN; 			//Временно выключаем АЦПирование
		for (i = 0; i < SAMPLES; i++)
		{
			if (TopOfBuffer)							//Если актуальная верхняя часть буфера
				sprintf(str, "%x", ADC_Buffer[i+SAMPLES]); //Передаём начиная с середины
			else
				sprintf(str, "%x", ADC_Buffer[i]);		//Если нет - первые 256 элементов		
			if (i != SAMPLES-1) strcat(str, ",");		//Если это не последний элемент - добавляем запятую
			strcat(TxBuffer, str);						//Добавляем элемент в конец буфера передачи
		}		
		DMA1_Channel1->CCR |= DMA_CCR1_EN;				//Включаем преобразования
	}
	else if (strncmp(RxBuffer, "FFT?", 4) == 0)			//Считать массив результатов БПФ
	{
		DMA1_Channel1->CCR &= ~DMA_CCR1_EN;				//Временно выключаем АЦПирование
		for (i = 0; i < SAMPLES; i++)
		{
			/* Результаты БПФ записываются в массив из 32-разрядных целочисленных переменных:
			 * - 16 старших разрядов - это мнимая часть
			 * - 16 младших разрядов - это действительная часть
			 */
			Imag = (int16_t)(FFT_Result[i] >> 16);		//Вытаскиваем мнимую часть
			Real = (int16_t)(FFT_Result[i] & 0xFFFF);	//Вытаскиваем действительную часть
			
			// Считаем амплитуду, как в школе учили
			A = sqrt(Imag*Imag + Real*Real)/SAMPLES;
			Res = A*10000;								//Нормируем результат
			
			sprintf(str, "%x", Res);					//Добавляем результат в конец буфера передачи
			if (i != SAMPLES-1) strcat(str, ",");		//Если это не последний элемент, ставим запятую
			strcat(TxBuffer, str);
		}
		DMA1_Channel1->CCR |= DMA_CCR1_EN;				//Включаем преобразования
	}
	else
	  strcpy(TxBuffer, "Invalid Command");				//Если мы не знаем, чего от нас хотят, ругаемся в ответ
	
	txStrWithDMA(TxBuffer);								//Отправляем буефер передачи
	
	memset(RxBuffer, 0, RX_BUFF_SIZE);					//Очищаем буфер приёма
	ComReceived = false;								//Сбрасываем флаг приёма строки	
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
	initADC1();
	initADC1_DMA();
	initTIM3();
	
	/*Основной цикл*/
	while(true)
	{
		LED4_ON();
		
		if (ComReceived)			//Ждём приема строки
			ExecuteCommand();
		
		LED4_OFF();
	}
}

