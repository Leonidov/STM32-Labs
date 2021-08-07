#include "main.h"

/**
  * @brief  Основная программа
  * @param  None
  * @retval None
  */
int main(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;	//включить тактирование GPIOA

	//очистка полей
	GPIOA->CRL &= ~(GPIO_CRL_CNF5 | GPIO_CRL_MODE5);
	//и конфигурация
	GPIOA->CRL |= GPIO_CRL_MODE5_1;		//PA5 (LD3), выход 2МГц

	//Бесконечный цикл
	while(1) {
		LED_ON();						//включить первый светодиод
		delay(DELAY_VAL);				//вызов подпрограммы задержки
		LED_OFF();						//выключить первый светодиод
		delay(DELAY_VAL);				//вызов подпрограммы задержки
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
