#include "main.h"
#include "delay.h"
void delay_us(int us)
{

	uint32_t told = SysTick->VAL;//SYSTICK的计数值  systick的频率是250Mhz
	uint32_t tnow;
	
	uint32_t load = 249999;
	
	/* LOAD+1个时钟对应1ms
	 * n us对应 n*(load+1)/1000个时钟
   */
	uint32_t ticks = us*(load+1)/1000;
	uint32_t cnt = 0;
	while (1)
	{
		tnow = SysTick->VAL;
		if (told >= tnow)
			cnt += told - tnow;
		else
			cnt += told + load + 1 - tnow;
			
		told = tnow;
		if (cnt >= ticks)
			break;
	}	
}


void delay_ms(int ms)
{
	for (int i = 0; i < ms; i++)
		delay_us(1000);
}


