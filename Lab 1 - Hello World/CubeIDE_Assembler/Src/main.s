.syntax unified
.cpu cortex-m3
.fpu softvfp
.thumb

#include "stm32f10x.s"

	.section .text.main
  	.global main
  	.type main, %function
main:
	ldr		r0, =PERIPH_BB_BASE+ \
				(RCC_APB2ENR-PERIPH_BASE)*32 + \
				2*4
										@ вычисляем адрес для BitBanding 2-го бита регистра RCC_APB2ENR
	mov		r1, #1						@ включаем тактирование порта A (во 2-й бит RCC_APB2ENR пишем '1`)
	str 	r1, [r0]					@ загружаем это значение

	ldr		r0, =GPIOA_CRL				@ адрес порта
	mov		r1, #0x03					@ 4-битная маска настроек для Output mode 50mHz, Push-Pull ("0011")
	ldr		r2, [r0]					@ считать порт
    bfi		r2, r1, #20, #4    			@ скопировать биты маски в позицию PIN5
    str		r2, [r0]					@ загрузить результат в регистр настройки порта

    ldr		r0, =GPIOA_BSRR				@ адрес порта выходных сигналов

loop:									@ Бесконечный цикл
	ldr 	r1, =GPIO_BSRR_BS5			@ устанавливаем вывод в '1'
	str 	r1, [r0]					@ загружаем в порт

	bl		delay						@ задержка

	ldr		r1, =GPIO_BSRR_BR5			@ сбрасываем в '0'
	str 	r1, [r0]					@ загружаем в порт

	bl		delay						@ задержка

	b 		loop						@ возвращаемся к началу цикла
	.size main, .-main

	.section .text.delay
  	.global delay
  	.type delay, %function
delay:									@ Подпрограмма задержки
	push	{r0}						@ Загружаем в стек R0, т.к. его значение будем менять
	ldr		r0, =0xFFFF					@ псевдоинструкция Thumb (загрузить константу в регистр)
delay_loop:
	subs	r0, #1						@ SUB с установкой флагов результата
	it 		NE
	bne		delay_loop					@ переход, если Z==0 (результат вычитания не равен нулю)
	pop		{r0}						@ Выгружаем из стека R0
	bx		lr							@ выход из подпрограммы (переход к адресу в регистре LR - вершина стека)
	.size delay, .-delay


