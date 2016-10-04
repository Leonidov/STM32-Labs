	PRESERVE8							; 8-битное выравнивание стека
	THUMB								; Режим Thumb (AUL) инструкций

	GET	config.s						; include-файлы
	GET	stm32f10x.s	

	AREA RESET, CODE, READONLY

	; Таблица векторов прерываний
	DCD STACK_TOP						; Указатель на вершину стека
	DCD Reset_Handler					; Вектор сброса

	ENTRY								; Точка входа в программу

Reset_Handler	PROC					; Вектор сброса
	EXPORT  Reset_Handler				; Делаем Reset_Handler видимым вне этого файла

main									; Основная подпрограмма
	MOV32	R0, PERIPH_BB_BASE + \
			RCC_APB2ENR * 32 + \
			5 * 4						; вычисляем адрес для BitBanding 5-го бита регистра RCC_APB2ENR
										; BitAddress = BitBandBase + (RegAddr * 32) + BitNumber * 4
	MOV		R1, #1						; включаем тактирование порта D (в 5-й бит RCC_APB2ENR пишем '1`)
	STR 	R1, [R0]					; загружаем это значение
	
	MOV32	R0, GPIOD_CRL				; адрес порта
	MOV		R1, #0x03					; 4-битная маска настроек для Output mode 50mHz, Push-Pull ("0011")
	LDR		R2, [R0]					; считать порт
    BFI		R2, R1, #28, #4    			; скопировать биты маски в позицию PIN7
    STR		R2, [R0]					; загрузить результат в регистр настройки порта

loop									; Бесконечный цикл
    MOV32	R0, GPIOD_BSRR				; адрес порта выходных сигналов

	MOV 	R1, #(PIN7)					; устанавливаем вывод в '1'
	STR 	R1, [R0]					; загружаем в порт
	
	BL		delay						; задержка
	
	MOV		R1, #(PIN7 << 16)			; сбрасываем в '0'
	STR 	R1, [R0]					; загружаем в порт
	
	BL		delay						; задержка

	B 		loop						; возвращаемся к началу цикла
	
	ENDP

delay		PROC						; Подпрограмма задержки
	PUSH	{R0}						; Загружаем в стек R0, т.к. его значение будем менять
	LDR		R0, =DELAY_VAL				; псевдоинструкция Thumb (загрузить константу в регистр)
delay_loop
	SUBS	R0, #1						; SUB с установкой флагов результата
	IT 		NE
	BNE		delay_loop					; переход, если Z==0 (результат вычитания не равен нулю)
	POP		{R0}						; Выгружаем из стека R0
	BX		LR							; выход из подпрограммы (переход к адресу в регистре LR - вершина стека)
	ENDP
	
    END
