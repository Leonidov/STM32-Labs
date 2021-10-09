#ifndef __MAIN_H
#define	__MAIN_H

#include "stm32f10x.h" 
#include "stdbool.h"

#define DELAY_VAL		1000000

/* Управление светодиодами */
#define	LED1_ON()		GPIOD->BSRR = GPIO_BSRR_BS7
#define	LED1_OFF()		GPIOD->BSRR = GPIO_BSRR_BR7
#define LED1_SWAP()		GPIOD->ODR ^= GPIO_ODR_ODR7


#define	LED2_ON()		GPIOD->BSRR = GPIO_BSRR_BS13
#define	LED2_OFF()		GPIOD->BSRR = GPIO_BSRR_BR13	
#define LED2_SWAP()		GPIOD->ODR ^= GPIO_ODR_ODR13

#define	LED3_ON()		GPIOD->BSRR = GPIO_BSRR_BS3
#define	LED3_OFF()		GPIOD->BSRR = GPIO_BSRR_BR3
#define LED3_SWAP()		GPIOD->ODR ^= GPIO_ODR_ODR3

#define	LED4_ON()		GPIOD->BSRR = GPIO_BSRR_BS4
#define	LED4_OFF()		GPIOD->BSRR = GPIO_BSRR_BR4
#define LED4_SWAP()		GPIOD->ODR ^= GPIO_ODR_ODR4

/* Прототипы функций */
void initPorts(void);
void initButton(void);
void initTIM6(void);
void delay(uint32_t takts);

#endif
