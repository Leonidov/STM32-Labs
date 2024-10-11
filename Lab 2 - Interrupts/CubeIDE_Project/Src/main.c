/**
  ******************************************************************************
  * @file    main.c
  * @author  Vladimir Leonidov
  * @version V1.0.0
  * @date    09.10.2021
  * @brief   Лабораторная работа №2 - "Введение в прерывания"
  *			 Отладочная плата: STM32F103 Nucleo-64
  *
  *			 - Реализован обработчик прерывания от базвого таймера TIM2.
  *			   Прерывание срабатывает раз в секунду.
  *			 - Реализован обработчик прерывания по спаду импульса на выводе PC13.
  *			   К данному выводу подключена кнопка.
  *
  ******************************************************************************
  */

#include "main.h"

/**
  * @brief  Подпрограмма обработчика прерывания
  *			по переполнению тайемера TIM2
  * @param  None
  * @retval None
  */
void TIM2_IRQHandler(void)
{
	TIM2->SR &= ~TIM_SR_UIF;			//Сброс флага переполнения

	LED_SWAP();
}

/**
  * @brief  Подпрограмма обработчика прерывания
  *			по внешнему импульсу на выводах (EXTI15-EXTI10)
  *			Нас интересует EXTI13, там висит кнопка.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
	// Т.к. этот обработчик вызывается, если произшло одно из прерываний EXTI15-EXI10,
	// нужно проверить, кто из них его вызвал.
	if (EXTI->PR & EXTI_PR_PR13) 		// нас интересует EXTI13
	{
		EXTI->PR |= EXTI_PR_PR13;
		delay(10000);					//Задержка для защиты от дребезга контактов
		TIM2->CR1 ^= TIM_CR1_CEN;		//Инвертируем состояние таймера
	}
}

/**
  * @brief  Инициализация портов ввода-вывода
  * @param  None
  * @retval None
  */
void initPorts(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	//включить тактирование GPIOA

	//очистка полей
	GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
	//и конфигурация
	GPIOA->CRL |= GPIO_CRL_MODE5_1;		//PA5, выход 2МГц
}

/**
  * @brief  Инициализация прерывания от кнопки (PC13)
  * @param  None
  * @retval None
  */
void initButton(void)
{
	//Включить тактирование порта GPIOC и альтернативных функций
	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN | RCC_APB2ENR_AFIOEN;

	/* Настраиваем PC13 на вход, альтернативная функция */
	// Сбрасываем биты конфигурации порта...
	GPIOC->CRH &= ~(GPIO_CRH_MODE13 | GPIO_CRH_CNF13);
	//...и выставляем так, как нам нужно
	GPIOC->CRH |= GPIO_CRH_CNF13_1;			//Вход с подтяжкой
	GPIOC->BSRR = GPIO_BSRR_BS13;			//Подтяжка к Vdd

	/* Настройка самого прерывания */

	// Настройка альтернативных фукнций портов.
	// Настройки портов с 12 по 15 хранятся в регистре AFIO_EXTICR4.
	// Регистры объединены в массив AFIO->EXTICR, нумерация массива начинается с нулевого элемента.
	// Поэтому настройки AFIO_EXTICR4 хранятся в AFIO->EXTICR[3]
	AFIO->EXTICR[3] |= AFIO_EXTICR4_EXTI13_PC;

	EXTI->FTSR |= EXTI_FTSR_TR13;			//Прерывание по спаду импульса (при нажатии на кнопку)
	EXTI->IMR |= EXTI_IMR_MR13;				//Выставляем маску - EXTI13

	NVIC_EnableIRQ(EXTI15_10_IRQn);			//Разрешаем прерывание
	NVIC_SetPriority(EXTI15_10_IRQn, 0);	//Выставляем приоритет
}

/**
  * @brief  Инициализация таймера TIM6
  * @param  None
  * @retval None
  */
void initTIM2(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;		//Включить тактирование TIM6

	//Частота APB1 для таймеров = APB1Clk * 2 = 32МГц * 2 = 64МГц
	TIM2->PSC = 64000-1;					//Предделитель частоты (64МГц/64000 = 1кГц)
	TIM2->ARR = 1000-1;						//Модуль счёта таймера (1кГц/1000 = 1с)
	TIM2->DIER |= TIM_DIER_UIE;				//Разрешить прерывание по переполнению таймера
	TIM2->CR1 |= TIM_CR1_CEN;				//Включить таймер

	NVIC_EnableIRQ(TIM2_IRQn);				//Рарзрешить прерывание от TIM2
	NVIC_SetPriority(TIM2_IRQn, 1);			//Выставляем приоритет
}


/**
  * @brief  Инициализация систем тактирования:
  * 		Источник тактирования: HSI
  * 		Частота: 64МГц
  * @param  None
  * @retval None
  */
void initClk(void)
{
	// Enable HSI
	RCC->CR |= RCC_CR_HSION;
	while(!(RCC->CR & RCC_CR_HSIRDY)){};

	// Enable Prefetch Buffer
	FLASH->ACR |= FLASH_ACR_PRFTBE;

	// Flash 2 wait state
	FLASH->ACR &= ~FLASH_ACR_LATENCY;
	FLASH->ACR |= FLASH_ACR_LATENCY_2;

	// HCLK = SYSCLK
	RCC->CFGR |= RCC_CFGR_HPRE_DIV1;

	// PCLK2 = HCLK
	RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;

	// PCLK1 = HCLK
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;

	// PLL configuration: PLLCLK = HSI/2 * 16 = 64 MHz
	RCC->CFGR &= ~RCC_CFGR_PLLSRC;
	RCC->CFGR |= RCC_CFGR_PLLMULL16;

	// Enable PLL
	RCC->CR |= RCC_CR_PLLON;

	// Wait till PLL is ready
	while((RCC->CR & RCC_CR_PLLRDY) == 0) {};

	// Select PLL as system clock source
	RCC->CFGR &= ~RCC_CFGR_SW;
	RCC->CFGR |= RCC_CFGR_SW_PLL;

	// Wait till PLL is used as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){};
}

/**
  * @brief  Основная программа
  * @param  None
  * @retval None
  */
int main(void)
{
	/*Инициализации всякие*/
	initClk();
	initPorts();
	initButton();
	initTIM2();

	/*Основной цикл*/
	while(true){};
}

/**
  * @brief  Подпрограмма задержки
  * @param  takts - Кол-во тактов
  * @retval None
  */
void delay(uint32_t takts)
{
	for (uint32_t i = 0; i < takts; i++) {};
}
