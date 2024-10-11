/**
  ******************************************************************************
  * @file    main.c
  * @author  Vladimir Leonidov
  * @version V1.0.0
  * @date    06.12.2015
  * @brief   Лабораторная работа №2 - "Введение в прерывания"
  *			 Отладочная плата: STM32F10C-EVAL
  *
  *			 - Реализован обработчик прерывания от базвого таймера TIM6.
  *			   Прерывание срабатывает раз в секунду.
  *			 - Реализован обработчик прерывания по спаду импульса на выводе EXTI9.
  *			   К данному выводу подключена кнопка.
  *
  ******************************************************************************
  */

#include "main.h"

/**
  * @brief  Подпрограмма обработчика прерывания
  *			по переполнению тайемера TIM6
  * @param  None
  * @retval None
  */
void TIM6_IRQHandler(void)
{
	TIM6->SR &= ~TIM_SR_UIF;			//Сброс флага переполнения
	
	LED2_SWAP();
}

/**
  * @brief  Подпрограмма обработчика прерывания
  *			по внешнему импульсу на выводах (EXTI5-EXTI9)
  *			Нас интересует EXTI9, там висит кнопка.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
	// Т.к. этот обработчик вызывается если произшло одно из прерываний EXTI5-EXI9,
	// нужно проверить, кто из них его вызвал.
	if (EXTI->PR & EXTI_PR_PR9) 		// нас интересует EXTI9
	{
		EXTI->PR |= EXTI_PR_PR9;		//Сбрасываем флаг прерывания установкой бита в '1'
		delay(1000000);					//Задержка для защиты от дребезга контактов
		
		LED3_SWAP();
		EXTI->PR |= EXTI_PR_PR9;		//Сбрасываем флаг прерывания установкой бита в '1'
	}
}

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
	GPIOD->CRL |= GPIO_CRL_MODE4_1;			//LD4, выход 2МГц
	GPIOD->CRL |= GPIO_CRL_MODE3_1;			//LD3, выход 2МГц
	GPIOD->CRH |= GPIO_CRH_MODE13_1;		//LD2, выход 2МГц
	GPIOD->CRL |= GPIO_CRL_MODE7_1;			//LD1, выход 2МГц	
}

/**
  * @brief  Инициализация прерывания от кнопки (PB9)
  * @param  None
  * @retval None
  */
void initButton(void)
{
	//Включить тактирование порта GPIOB и альтернативных функций
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN | RCC_APB2ENR_AFIOEN;
	
	/* Настраиваем PB9 на вход, альтернативная функция */
	// Сбрасываем биты конфигурации порта...
	GPIOB->CRH &= ~(GPIO_CRH_MODE9 | GPIO_CRH_CNF9);	
	//...и выставляем так, как нам нужно
	GPIOB->CRH |= GPIO_CRH_CNF9_1;			//Вход с подтяжкой
	GPIOB->BSRR = GPIO_BSRR_BS9;			//Подтяжка к Vdd
	
	/* Настройка самого прерывания */
	
	// Настройка альтернативных фукнций портов.
	// Настройки портов с 8 по 11 хранятся в регистре AFIO_EXTICR3.
	// Регистры объединены в массив AFIO->EXTICR, нумерация массива начинается с нулевого элемента.
	// Поэтому настройки AFIO_EXTICR3 хранятся в AFIO->EXTICR[2]
	AFIO->EXTICR[2] |= AFIO_EXTICR3_EXTI9_PB;
	
	EXTI->FTSR |= EXTI_FTSR_TR9;			//Прерывание по спаду импульса (при нажатии на кнопку)
	EXTI->IMR |= EXTI_IMR_MR9;				//Выставляем маску - EXTI9
	
	NVIC_EnableIRQ(EXTI9_5_IRQn);			//Разрешаем прерывание
	NVIC_SetPriority(EXTI9_5_IRQn, 0);		//Выставляем приоритет
}

/**
  * @brief  Инициализация таймера TIM6
  * @param  None
  * @retval None
  */
void initTIM6(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;		//Включить тактирование TIM6
	
	//Частота APB1 для таймеров = APB1Clk * 2 = 36МГц * 2 = 72МГц
	TIM6->PSC = 36000-1;					//Предделитель частоты (72МГц/36000 = 2кГц)
	TIM6->ARR = 2000-1;						//Модуль счёта таймера (2кГц/2000 = 1с)
	TIM6->DIER |= TIM_DIER_UIE;				//Разрешить прерывание по переполнению таймера
	TIM6->CR1 |= TIM_CR1_CEN;				//Включить таймер
	
	NVIC_EnableIRQ(TIM6_IRQn);				//Рарзрешить прерывание от TIM6
	NVIC_SetPriority(TIM6_IRQn, 1);			//Выставляем приоритет
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
	initButton();
	initTIM6();
	
	/*Основной цикл*/
	while(true) {
		LED1_ON();							//включить первый светодиод
		delay(DELAY_VAL);					//вызов подпрограммы задержки
		LED1_OFF();							//выключить первый светодиод
		delay(DELAY_VAL);					//вызов подпрограммы задержки		
	}
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
