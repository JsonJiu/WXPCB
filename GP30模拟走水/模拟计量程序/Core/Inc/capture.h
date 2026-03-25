#include "main.h"

#ifndef __CAPTURE__H
#define __CAPTURE__H

#define Input_A 1
#define Input_B 2
#define Output_A 3
#define Output_B 4
#define Capture_A HAL_GPIO_ReadPin(INPUT_A_GPIO_Port,INPUT_A_Pin)
#define Capture_B HAL_GPIO_ReadPin(INPUT_B_GPIO_Port,INPUT_B_Pin)
#define Capture_C HAL_GPIO_ReadPin(INPUT_C_GPIO_Port,INPUT_C_Pin)
#define Capture_D HAL_GPIO_ReadPin(INPUT_D_GPIO_Port,INPUT_D_Pin)
void Capture_Detect(void);
void GPIO_Mode(uint32_t IO);
void Water_Disable(void);

enum mode
{
	jiliang_channel1 = 1,
	jiliang_channel2
};


#endif


