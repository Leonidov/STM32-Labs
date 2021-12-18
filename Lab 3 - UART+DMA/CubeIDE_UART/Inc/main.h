#ifndef __MAIN_H
#define	__MAIN_H

#include "stm32f1xx.h"
#include "stdbool.h"
#include "string.h"
#include "stdio.h"

// Раскомментировать #define,
// если нужно использовать DMA при передаче по USART
#define USE_DMA

/* Размеры буферов приёма и передачи */
#define	RX_BUFF_SIZE	256
#define TX_BUFF_SIZE	256

#define DELAY_VAL		1000000

/* Управление светодиодами */
#define	LED_ON()		GPIOA->BSRR = GPIO_BSRR_BS5
#define	LED_OFF()		GPIOA->BSRR = GPIO_BSRR_BR5
#define LED_SWAP()		GPIOA->ODR ^= GPIO_ODR_ODR5

/* Прототипы функций */
void initUSART2(void);
void txStr(char *str, bool crlf);
void txStrWithDMA(char *str, bool crlf);
void ExecuteCommand(void);
void initClk(void);
void initPorts(void);
void initButton(void);
void initTIM2(void);
void delay(uint32_t takts);

#endif
