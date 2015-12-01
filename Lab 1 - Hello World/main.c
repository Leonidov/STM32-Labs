#include "main.h"

/**
  * @brief  Основная программа
  * @param  None
  * @retval None
  */
int main(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPDEN;	//включить тактирование GPIOD
	
	GPIOD->CRL = 0;						//очистка регистров конфигурации..
	GPIOD->CRH = 0;						//..портов ввода/вывода
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
	uint32_t i;
	
	for (i = 0; i < takts; i++) {};
}
