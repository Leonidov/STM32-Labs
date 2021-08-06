#include "main.h"                // Device header#include "main.h"

/**
  * @brief  Основная программа
  * @param  None
  * @retval None
  */
int main(void)
{
	GPIO_InitTypeDef MyPortCfg;		//структура конфигурации порта
	
	//Включить тактирование GPIOD
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD,ENABLE);
	
	//Заполняем поля структуры MyPortCfg
	MyPortCfg.GPIO_Mode = GPIO_Mode_Out_PP;	//Push-Pull
	MyPortCfg.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_13;
	MyPortCfg.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOD,&MyPortCfg);			//Применить настройки порта
	
	//Бесконечный цикл
	while(1) {
		GPIO_SetBits(GPIOD,GPIO_Pin_7);
		delay(DELAY_VAL);
		GPIO_ResetBits(GPIOD,GPIO_Pin_7);
		GPIO_SetBits(GPIOD,GPIO_Pin_13);
		delay(DELAY_VAL);
		GPIO_ResetBits(GPIOD,GPIO_Pin_13);
		GPIO_SetBits(GPIOD,GPIO_Pin_3);
		delay(DELAY_VAL);
		GPIO_ResetBits(GPIOD,GPIO_Pin_3);
		GPIO_SetBits(GPIOD,GPIO_Pin_4);
		delay(DELAY_VAL);
		GPIO_ResetBits(GPIOD,GPIO_Pin_4);			
	}
}

/**
  * @brief  Подпрограмма задержки
  * @param  takts - Кол-во тактов
  * @retval None
  */
void delay(uint32_t takts)
{
	uint32_t i;
	
	for (i = 0; i < takts; i++) {};
}
