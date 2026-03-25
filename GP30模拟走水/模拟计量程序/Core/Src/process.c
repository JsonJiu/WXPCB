#include "process.h"
#include "buffer.h"
#include "capture.h"




extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim6;

static volatile uint32_t g_Tx_sign = 0;//定义发送完成标志位，必须定义volatile类型，否则改变量会被编译器优化
static volatile uint32_t g_Rx_sign = 0;//定义发送完成标志位，必须定义volatile类型，否则改变量会被编译器优化

static circle_buffer Rxbuffer;//定义环形缓冲区结构体变量
static uint8_t Getbuffer[100];//定义环形缓冲区
static uint8_t g_recvchar;//定义存储接收数据的字节变量

uint8_t getdata;
uint8_t getbuffer[100];//定义一个缓冲区用于保存数据，可用此来控制外设
int i = 0;
uint8_t GetOrder[100];

uint8_t start_receive_flag = 0;//置1即开始接收下发指令
uint8_t receive_sucess = 0;//置1表示接收完成
uint8_t Sum_Receive = 0x00;//和校验的数据累加值
uint8_t Len_Receive = 0;//数据帧长度
uint8_t Verify_Flag = 0;//校验合格标志位
uint8_t flag1 = 0;//检测到帧头标志位
uint8_t cnt = 0;//进入中断次数计数
uint8_t count = 0;//检测到帧头时的计数值
uint8_t status = 0x00;  //水的状态 0——无水状态   1——静水状态   2——走水状态
uint8_t speed = 0; //水流大小

uint8_t Back_Order[100] = {0};//返回指令

void Get_Buffer_Init(void)//初始化
{
	Circle_Buffer_Init(&Rxbuffer,100,Getbuffer);//环形缓冲区初始化
	HAL_UART_Receive_IT(&huart1,&g_recvchar,1);//使能串口DMA中断
}

int UART_GetChar(uint8_t *pVal)//得到一个字节
{
	return Circle_Read(&Rxbuffer,pVal);//读取字节并返回值
}

void Wait_Tx_Complete(void)//等待发送完成函数
{
	while(g_Tx_sign == 0);
	g_Tx_sign = 0;
}

void Wait_Rx_Complete(void)//等待接收完成函数
{
	while(g_Rx_sign == 0);
	g_Rx_sign = 0;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)//发送完成回调函数
{
	g_Tx_sign = 1;//标志位置1表示发送完成
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef*UartHandle)
{
	  g_Rx_sign = 1;
		Circle_Write(&Rxbuffer,g_recvchar);//将接收到的数据逐字节写入环形缓冲区
		UART_GetChar(&getdata);//写完直接读出来
		cnt++;//主要作用在于判断帧头和下发指令是否连续
		if(getdata == 0x68 && flag1 == 0)
		{
			flag1 = 1;
			count = cnt;
		}
		if(flag1 == 1 && getdata == 0xAA && cnt == count+1)//判断0x68帧头接收下一个字节是否为0xAA
		{
			start_receive_flag = 1;//如果检测到0x68和0xAA连续，则开始存储数据串标志位置1
			count = 0;
			i = 1;
		}
		if(start_receive_flag == 1)//如果开始存储数据串标志位置1，则开始存储数据串
		{
			GetOrder[0] = 0x68;
			GetOrder[i] = getdata;
			i++;
		}
		if(i == 3)
		{
			Len_Receive = GetOrder[2];//获取数据串长度
		}
		if(getdata == 0x16 && i == Len_Receive)//如果获取的数据串长度和接收到帧尾时的实时数据串长度一致，则开始校验数据串
		{
			cnt = 0;
			flag1 = 0;
			start_receive_flag = 0;
			i = 0;
			if(Data_Verify() == 1) //校验正确
			{
				Verify_Flag = 1;
			}
			else if(Data_Verify() == 0)//校验错误
			{
				Verify_Flag = 0;
			}
		}
		if(i > Len_Receive)//如果实时数据串长度大于获取的数据串长度，则初始化
		{
			flag1 = 0;
			cnt = 0;
		}
    HAL_UART_Receive_IT(&huart1,&g_recvchar, 1); //每接收一个数据，就打开一次串口中断接收，否则只会接收一个数据就停止接收
}

uint8_t Data_Verify(void)//进行数据校验
{
		Len_Receive = GetOrder[2];
		for(int j = 0; j<Len_Receive-2; j++) //进行和校验，累加校验位前面的所有字节
		{
			Sum_Receive = Sum_Receive + GetOrder[j];
		}
		if(Sum_Receive == GetOrder[Len_Receive - 2])
		{
			Sum_Receive = 0;
			return 1; //校验正确返回1
		}
		else 
		{
			Sum_Receive = 0;
			return 0; //校验错误返回0
		}
}

void Back_Order_Process(void)
{
	for(int j = 0;j<Len_Receive;j++)
		{
			Back_Order[j] = GetOrder[j];
		}
		Back_Order[1] = 0xAB;
		Back_Order[Len_Receive - 2] = GetOrder[Len_Receive - 2] + 1;
		HAL_UART_Transmit_IT(&huart1,Back_Order,Len_Receive);
		Wait_Tx_Complete();
}

void Data_Process(void)
{
	if(Verify_Flag == 1)
	{
		status = GetOrder[3];
		speed = GetOrder[4];
		Back_Order_Process();
		Verify_Flag = 0;
		Water_Status();
	}
}

void Water_Status(void)
{
		switch(status)
		{
			case 0x00:					//无水状态，不回应
				HAL_NVIC_DisableIRQ(EXTI2_IRQn);
				HAL_NVIC_DisableIRQ(EXTI10_IRQn);
				HAL_NVIC_DisableIRQ(EXTI5_IRQn);
				HAL_NVIC_DisableIRQ(EXTI6_IRQn);
				break;
			case 0x01:					//静水状态，△t = 0；
				__HAL_TIM_SET_AUTORELOAD(&htim6,yidaibiao+10);  //西班牙——(xibanya-25)  秘鲁——(bilu+20)  一代表——(yidaibiao+10)
				HAL_NVIC_EnableIRQ(EXTI2_IRQn);
				HAL_NVIC_EnableIRQ(EXTI10_IRQn);
				HAL_NVIC_EnableIRQ(EXTI5_IRQn);
				HAL_NVIC_EnableIRQ(EXTI6_IRQn);
				break;
			case 0x02:					//走水状态，△t != 0；
				Water_Speed();
				HAL_NVIC_EnableIRQ(EXTI2_IRQn);
				HAL_NVIC_EnableIRQ(EXTI10_IRQn);
				HAL_NVIC_EnableIRQ(EXTI5_IRQn);
				HAL_NVIC_EnableIRQ(EXTI6_IRQn);
				break;
			default:
				break;
		}
}

void Water_Speed(void)
{
	switch(speed)
	{
		case 0x01:
			__HAL_TIM_SET_AUTORELOAD(&htim6,yidaibiao+10); 
			break;
		case 0x02:
			__HAL_TIM_SET_AUTORELOAD(&htim6,yidaibiao+20);  
			break;
		case 0x03:
			__HAL_TIM_SET_AUTORELOAD(&htim6,yidaibiao+30);  
			break;
		case 0x04:
			__HAL_TIM_SET_AUTORELOAD(&htim6,yidaibiao+40);
			break;
		default:
			break;
	}
}

