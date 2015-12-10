#ifndef __MCU_CONFIG_H
#define	__MCU_CONFIG_H

#include "stm32f10x.h" 

/* Параметры ЦАП */
//Величина опорного напряжения
#define	V_REF		3.3
//Полная шкала ЦАП
#define	DAC_FS		0xFFF

/* Управление светодиодами */
#define	LED1_ON()	GPIOD->BSRR |= GPIO_BSRR_BS7
#define	LED1_OFF()	GPIOD->BSRR |= GPIO_BSRR_BR7

#define	LED2_ON()	GPIOD->BSRR |= GPIO_BSRR_BS13
#define	LED2_OFF()	GPIOD->BSRR |= GPIO_BSRR_BR13

#define	LED3_ON()	GPIOD->BSRR |= GPIO_BSRR_BS3
#define	LED3_OFF()	GPIOD->BSRR |= GPIO_BSRR_BR3

#define	LED4_ON()	GPIOD->BSRR |= GPIO_BSRR_BS4
#define	LED4_OFF()	GPIOD->BSRR |= GPIO_BSRR_BR4

/* Прототипы функций */
void initPorts(void);
void initUART2(void);
void initUART2_DMA(void);
void initTIM6(void);
void initDAC1(void);
void initDAC1_DMA(void);

#endif
