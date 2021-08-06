/**
  ******************************************************************************
  * @file    main.c
  * @author  Vladimir Leonidov
  * @version V1.0.0
  * @date    06.12.2015
  * @brief   Лабораторная работа №1 - "Hello World!"
  *			 Отладочная плата: STM32F10C-EVAL
  *
  *			 - Реализовано поочерёдное мигание четырьмя светодиодами
  *
  ******************************************************************************
  */

#include "main.h"

/**
  * @brief  Основная программа
  * @param  None
  * @retval None
  */
int main(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	//включить тактирование GPIOD
	
	//очистка полей
	GPIOD->CRL &= ~(GPIO_CRL_CNF3 | GPIO_CRL_MODE3);
	GPIOD->CRL &= ~(GPIO_CRL_CNF4 | GPIO_CRL_MODE4);
	GPIOD->CRL &= ~(GPIO_CRL_CNF7 | GPIO_CRL_MODE7);
	GPIOD->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
	
	//и конфигурация
	GPIOD->CRL |= GPIO_CRL_MODE4_1;		//LD4, выход 2МГц
	GPIOD->CRL |= GPIO_CRL_MODE3_1;		//LD3, выход 2МГц
	GPIOD->CRH |= GPIO_CRH_MODE13_1;	//LD2, выход 2МГц
	GPIOD->CRL |= GPIO_CRL_MODE7_1;		//LD1, выход 2МГц
	
	//Бесконечный цикл
	while(1) {
		LED1_ON();						//включить первый светодиод
		delay(DELAY_VAL);				//вызов подпрограммы задержки
		LED1_OFF();						//выключить первый светодиод
		LED2_ON();						//включить второй светодиод
		delay(DELAY_VAL);				//...
		LED2_OFF();
		LED3_ON();
		delay(DELAY_VAL);
		LED3_OFF();
		LED4_ON();
		delay(DELAY_VAL);
		LED4_OFF();			
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
