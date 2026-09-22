    GET     stm32f10x.s

    EXPORT  __main
    EXPORT  delay

    AREA    |.text|, CODE, READONLY
    THUMB

__main    PROC
	MOV32	R0, PERIPH_BB_BASE + \
			(RCC_APB2ENR-PERIPH_BASE) * 32 + \
			2 * 4						; вычисляем адрес для BitBanding 5-го бита регистра RCC_APB2ENR
										; BitAddress = BitBandBase + (RegAddr * 32) + BitNumber * 4
	MOV		R1, #1						; включаем тактирование порта D (в 5-й бит RCC_APB2ENR пишем '1`)
	STR 	R1, [R0]					; загружаем это значение
	
	MOV32	R0, GPIOA_CRL				; адрес порта
	MOV		R1, #2_0011					; 4-битная маска настроек для Output mode 50mHz, Push-Pull ("0011")
	LDR		R2, [R0]					; считать порт
    BFI		R2, R1, #20, #4    			; скопировать биты маски в позицию PIN7
    STR		R2, [R0]					; загрузить результат в регистр настройки порта

    MOV32	R0, GPIOA_BSRR				; адрес порта выходных сигналов

loop									; Бесконечный цикл
	MOV32 	R1, #GPIO_BSRR_BS5					; устанавливаем вывод в '1'
	STR 	R1, [R0]					; загружаем в порт
	
	BL		delay						; задержка
	
	MOV32	R1, #GPIO_BSRR_BR5			; сбрасываем в '0'
	STR 	R1, [R0]					; загружаем в порт
	
	BL		delay						; задержка

	B 		loop						; возвращаемся к началу цикла
	
	ENDP

delay		PROC						; Подпрограмма задержки
	PUSH	{R0}						; Загружаем в стек R0, т.к. его значение будем менять
	LDR		R0, =0xFFFFF    			; псевдоинструкция Thumb (загрузить константу в регистр)
delay_loop
	SUBS	R0, #1						; SUB с установкой флагов результата
	IT 		NE
	BNE		delay_loop					; переход, если Z==0 (результат вычитания не равен нулю)
	POP		{R0}						; Выгружаем из стека R0
	BX		LR							; выход из подпрограммы (переход к адресу в регистре LR - вершина стека)
	ENDP

    END