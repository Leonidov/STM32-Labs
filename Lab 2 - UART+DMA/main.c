#include "main.h"

char RxBuffer[RX_BUFF_SIZE];					//Буфер приёма USART
char TxBuffer[TX_BUFF_SIZE];					//Буфер передачи USART
bool ComReceived;								//Флаг приёма команды

/**
  * @brief  Обработчик прерывания от USART2
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
	char tmp;

	if ((USART2->SR & USART_SR_RXNE)!=0)		//Прерывание по приёму данных?
	{
		tmp=USART2->DR;
		if (tmp==0x0D)							//Если это символ конца строки
		{
			ComReceived=true;					//- выставляем флаг приёма строки
			return;								//- и выходим
		}
		RxBuffer[strlen(RxBuffer)] = tmp;		//Иначе добавляем его в буфер
	}
}

/**
  * @brief  Инициализация портов ввода-вывода
  * @param  None
  * @retval None
  */
void initPorts(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;
	
	GPIOD->CRL = 0;
	GPIOD->CRH = 0;
	GPIOD->CRL |= GPIO_CRL_MODE4_1;				//LD4, выход 2МГц
	GPIOD->CRL |= GPIO_CRL_MODE3_1;				//LD3, выход 2МГц
	GPIOD->CRH |= GPIO_CRH_MODE13_1;			//LD2, выход 2МГц
	GPIOD->CRL |= GPIO_CRL_MODE7_1;				//LD1, выход 2МГц	
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
void initDMA(void)
{
	RCC->AHBENR |= RCC_AHBENR_DMA1EN;				//разрешить такт. DMA
	
	DMA1_Channel7->CPAR = (uint32_t)&USART2->DR;	//указатель на регистр данных USART2
	
	DMA1_Channel7->CCR = 0;
	DMA1_Channel7->CCR |= DMA_CCR7_DIR;				//направление - из памяти в устройство
	DMA1_Channel7->CCR |= DMA_CCR7_MINC;			//инкремент указателя в памяти	
	USART2->CR3 |= USART_CR3_DMAT;					//настроить USART2 на работу с DMA
}

/**
  * @brief  Передача строки по USART2 без DMA
  * @param  Указатель на строку
  * @retval None
  */
void txStr(char *str)
{
	uint16_t i;
	
	strcat(str,"\r");								//добавляем символ конца строки
	
	for (i = 0; i < strlen(str); i++)
	{
		USART2->DR = str[i];						//передаём байт данных
		while ((USART2->SR & USART_SR_TC)==0) {};	//ждём окончания передачи
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
	memmove(TxBuffer,RxBuffer,RX_BUFF_SIZE);		//RxBuffer -> TxBuffer
	
	// Передача принятой строки обратно одним из двух способов
	#ifdef USE_DMA
		txStrWithDMA(TxBuffer);
	#else
		txStr(TxBuffer);
	#endif
	
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
	initDMA();

	/*Основной цикл*/
	while(true)
	{
		LED1_ON();
		
		if (ComReceived)			//Ждём приема строки
			ExecuteCommand();
		
		LED1_OFF();
	}
}

/**
  * @brief  Подпрограмма задержки
  * @param  takts - Кол-во тактов
  * @retval None
  */
void delay(uint32_t takts)
{
	uint32_t i;
	
	for (i = 0; i < takts; i++) {};
}
