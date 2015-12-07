**Проекты Keil uVision 5**

**МК: STM32F107VC (отладочная плата STM3210C-EVAL)**

# Лабораторная работа №1 (Lab 1 - Hello World)
## Hello World!

Проект "Hello World!", реализует миганием светодиодами на отладочной плате.

В папке `StdPeriphLib Version` находится версия исходников с использованием библиотеки **StdPeriphLib**.

# Лабораторная работа №2 (Lab 2 - Interrupts)
## Введение в прерывания

Реализовано два обработчика прерываний:
- по переполненю таймера **TIM6** (раз в секунду меняется состояние светодиода)
- по внешнему импульсу на линии **EXTI9** (к **PB9** подключена кнопка, по нажатию меняется состояние светодиода)

# Лабораторная работа №3 (Lab 3 - UART+DMA)
## Модуль USART, DMA

Демонстрирует работу с модулем USART и два подхода к передаче данных:
- через процессор (`#define USE_DMA` должен быть закомментирован)
- через модуль DMA (`#define USE_DMA` должен быть раскомментирован)

В папке `Waveforms` лежат осциллограммы для двух варинтов передачи данных.
- [USART without DMA.png](https://github.com/Leonidov/STM32-Labs/blob/master/Lab%203%20-%20UART+DMA/Waveforms/USART%20without%20DMA.png?raw=true) - передача по USART осуществляется через процессор.
- [USART with DMA.png](https://github.com/Leonidov/STM32-Labs/blob/master/Lab%203%20-%20UART+DMA/Waveforms/USART%20with%20DMA.png?raw=true) - передача по USART осуществляется через модуль DMA.

На двух картинках:
- **Канал А (синий)** - сигнал на выводе **PD7** (светодиод **LED1**)
- **Канал B (красный)** - сигнал на выводе **PD5** (USART2 TX Remapped)

# Лабораторная работа №4 (Lab 4 - ADC)
## Работа с модулем АЦП

Реализована работа с регулярной группой каналов АЦП ADC1:
- **ADC1_IN14** - подключен к потенциометру
- **ADC1_IN16** - внутренний канал, подключен к встроенному термодатчику

Реализован простейший обработчик SCPI-подобных команд:
- __*IDN?__ - считать идентификатор устройства. В ответ МК вернёт строку `Lab 4 - ADC`.
- **LED1 ON|OFF** - включить/выключить светодиод **LED1**. В случае успешного выполнения команды МК вернёт строку `OK`. В случае неверного параметра (если это не `ON` или `OFF`) - `Invalid Parameter`.
- **ADC?** - измерить напряжение на входе **ADC1_IN14**. В ответ МК вернёт значение напряжения в Вольтах.
- **TEMPER?** - измерить температуру микроконтроллера. В ответ МК вернёт значение температуры в градусах Цельсия.

В случае получения неизвестной команды МК вернёт сообщение об ошибке `Invalid Command`.

# Лабораторная работа №6 (Lab 6 - FreeRTOS)
## Введение в операционные системы реального времени

Проект сгенерирован с использованием STM32CubeMX.

Данный проект демонстрирует:
- Реализацию многозадачности
- Работу с очередью
- Работу с семафорами

#### Очереди:
- `PressedBtn` - содержит номера нажатых кнопок (`1` для **LED1** и `2` для **LED2**).

#### Семафоры:
- `WakeUpKeyPressed` - счётный семафор, инкрементируется с каждым нажатием кнопки **Wakeup**.

#### Задачи:
- `DefaultTask` - просто мигает светодиодом **LED4**.
- `ScanUserKeys` - в цикле считывает состояние кнопок **UserKey** и **Tamper**, передаёт номер нажатой кнопки в очередь `PressedBtn`:
  - `1` - нажата кнопка  **UserKey**;
  - `2` - нажата кнопка  **Tamper**;
- `TaskLEDS` - принимает из очереди `PressedBtn` номер нажатой кнопки, в зависимости от этого инвертирует состояние светодиода **LED1** (если это **UserKey**) или **LED2** (если это **Tamper**).
- `ScanWakeUpKey` - выставляет семафор `WakeUpKeyPressed` в случае, если произошло нажатие на кнопку **Wakeup**.
- `LEDBlink` - ожидает семафор `WakeUpKeyPressed` и, по его установке, инвертирует состояние светодиода **LED3**.