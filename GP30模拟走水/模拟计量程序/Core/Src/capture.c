#include "capture.h"
#include "delay.h"
extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
uint32_t countA = 0;
uint32_t countB = 0;
uint32_t countC = 0;
uint32_t countD = 0;
uint8_t jiliang_channel = 0;



void Capture_Detect(void)
{
  if(Capture_A == GPIO_PIN_SET)
	{
		__HAL_TIM_CLEAR_IT(&htim6,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim6);
		delay_us(70);
	}
	if(Capture_B == GPIO_PIN_SET)
	{
		__HAL_TIM_CLEAR_IT(&htim1,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim1);
		delay_us(70);
	}
	if(Capture_C == GPIO_PIN_SET)
	{
		__HAL_TIM_CLEAR_IT(&htim6,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim6);
		delay_us(70);	
	}
	if(Capture_D == GPIO_PIN_SET)
	{
		__HAL_TIM_CLEAR_IT(&htim1,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim1);
		delay_us(70);	
	}
}

void Water_Disable(void)
{
	HAL_NVIC_DisableIRQ(EXTI2_IRQn);
	HAL_NVIC_DisableIRQ(EXTI10_IRQn);
	HAL_NVIC_DisableIRQ(EXTI5_IRQn);
	HAL_NVIC_DisableIRQ(EXTI6_IRQn);
}

void HAL_EXTI_CLEAR_IT(void)
{
		__HAL_GPIO_EXTI_CLEAR_IT(INPUT_A_Pin);
	  __HAL_GPIO_EXTI_CLEAR_IT(INPUT_B_Pin);
		__HAL_GPIO_EXTI_CLEAR_IT(INPUT_C_Pin);
	  __HAL_GPIO_EXTI_CLEAR_IT(INPUT_D_Pin);
}

void HAL_EXTI_ENABLE(void)
{
		HAL_NVIC_EnableIRQ(EXTI2_IRQn);
		HAL_NVIC_EnableIRQ(EXTI10_IRQn);
		HAL_NVIC_EnableIRQ(EXTI5_IRQn);
		HAL_NVIC_EnableIRQ(EXTI6_IRQn);
}

void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim3)
	{
		if(htim -> Channel == HAL_TIM_ACTIVE_CHANNEL_4)
		{
			countB++;
			if(countB == 20)//20
			{
				countB = 0;
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_4);
				HAL_TIM_PWM_Init(&htim3);
				HAL_EXTI_CLEAR_IT();
				HAL_EXTI_ENABLE();
			}
		}
		if(htim -> Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			countA++;
			if(countA == 20)//20
			{
				countA = 0;
				HAL_TIM_PWM_Stop_IT(&htim3,TIM_CHANNEL_3);
				HAL_TIM_PWM_Init(&htim3);
				HAL_EXTI_CLEAR_IT();
				HAL_EXTI_ENABLE();
			}
		}
	}
	else if(htim == &htim2)
	{
		if(htim -> Channel == HAL_TIM_ACTIVE_CHANNEL_1)
		{
			countC++;
			if(countC == 20)//20
			{
				countC = 0;
				HAL_TIM_PWM_Stop_IT(&htim2,TIM_CHANNEL_1);
				HAL_TIM_PWM_Init(&htim2);
				HAL_EXTI_CLEAR_IT();
				HAL_EXTI_ENABLE();
			}
		}
		if(htim -> Channel == HAL_TIM_ACTIVE_CHANNEL_3)
		{
			countD++;
			if(countD == 20)//20
			{
				countD = 0;
				HAL_TIM_PWM_Stop_IT(&htim2,TIM_CHANNEL_3);
				HAL_TIM_PWM_Init(&htim2);
				HAL_EXTI_CLEAR_IT();
				HAL_EXTI_ENABLE();
			}
		}	
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim6)
	{
		HAL_TIM_Base_Stop_IT(&htim6);
		switch(jiliang_channel)
		{
			case jiliang_channel1:
				HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_4); //B模拟发送
				break;
			case jiliang_channel2:
				HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_3); //D模拟发送
				break;
			default:
				break;
		}
	}
	if(htim == &htim1)
	{
		HAL_TIM_Base_Stop_IT(&htim1);
		switch(jiliang_channel)
		{
			case jiliang_channel1:
				HAL_TIM_PWM_Start_IT(&htim3,TIM_CHANNEL_3); //A模拟发送
				break;
			case jiliang_channel2:
				HAL_TIM_PWM_Start_IT(&htim2,TIM_CHANNEL_1); //C模拟发送
				break;
			default:
				break;
		}
	}
}

void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == INPUT_A_Pin)
	{
		jiliang_channel = jiliang_channel1;
		HAL_NVIC_DisableIRQ(EXTI2_IRQn);	
		HAL_NVIC_DisableIRQ(EXTI10_IRQn);	
		__HAL_TIM_CLEAR_IT(&htim6,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim6);
	}
	if(GPIO_Pin == INPUT_B_Pin)
	{
		jiliang_channel = jiliang_channel1;
		HAL_NVIC_DisableIRQ(EXTI10_IRQn);	
		HAL_NVIC_DisableIRQ(EXTI2_IRQn);		
		__HAL_TIM_CLEAR_IT(&htim1,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim1);
	}	

	if(GPIO_Pin == INPUT_C_Pin)
	{
		jiliang_channel = jiliang_channel2;
		HAL_NVIC_DisableIRQ(EXTI5_IRQn);	
		HAL_NVIC_DisableIRQ(EXTI6_IRQn);	
		__HAL_TIM_CLEAR_IT(&htim6,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim6);
	}
	if(GPIO_Pin == INPUT_D_Pin)
	{
		jiliang_channel = jiliang_channel2;
		HAL_NVIC_DisableIRQ(EXTI6_IRQn);	
		HAL_NVIC_DisableIRQ(EXTI5_IRQn);		
		__HAL_TIM_CLEAR_IT(&htim1,TIM_IT_UPDATE);
		HAL_TIM_Base_Start_IT(&htim1);
	}	
}


