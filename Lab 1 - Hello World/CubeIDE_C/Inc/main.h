#ifndef MAIN_H_
#define MAIN_H_

#include "stm32f1xx.h"

// Величина задержки между вкл/выкл светодиодов
#define DELAY_VAL	100000

/* Управление светодиодами */
#define	LED_ON()	GPIOA->BSRR = GPIO_BSRR_BS5
#define	LED_OFF()	GPIOA->BSRR = GPIO_BSRR_BR5

/* Прототипы функций */
void delay(uint32_t takts);

#endif /* MAIN_H_ */
