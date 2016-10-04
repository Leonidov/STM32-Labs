	PRESERVE8							; 8-битное выравнивание стека
	THUMB								; Режим Thumb (AUL) инструкций

	GET	config.s						; include-файлы
	GET	stm32f10x.s	

	AREA RESET, CODE, READONLY

	; Таблица векторов прерываний
	DCD STACK_TOP						; Указатель на вершину стека
	DCD Reset_Handler					; Вектор сброса

	ENTRY								; Точка входа в программу

; Начало программы
Reset_Handler	PROC
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
	LDR		R3, [R0]					; считать порт
    BFI		R3, R1, #28, #4    			; скопировать биты маски в позицию PIN7
    STR		R3, [R0]					; загрузить результат в регистр настройки порта

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
	LDR		R7, =DELAY_VAL				; псевдоинструкция Thumb (загрузить константу в регистр)
delay_loop
	SUBS	R7, #1						; SUB с установкой флагов результата
	IT 		NE
	BNE		delay_loop					; переход, если Z==0 (результат вычитания не равен нулю)
	BX		LR							; выход из подпрограммы (переход к адресу в регистре LR - вершина стека)
	ENDP
	
    END
