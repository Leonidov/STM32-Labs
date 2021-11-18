#ifndef __MAIN_H
#define	__MAIN_H

#include "stm32f10x.h" 
#include "stdbool.h"
#include "string.h"
#include "stdio.h"

/* Размеры буферов приёма и передачи */
#define	RX_BUFF_SIZE	256
#define TX_BUFF_SIZE	256

/* Прототипы функций */
void txStr(char *str);
void txStrWithDMA(char *str);
void delay(uint32_t takts);

#endif
