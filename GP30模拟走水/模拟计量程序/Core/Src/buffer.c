#include "buffer.h"


void Circle_Buffer_Init(pCircle_Buf buffer,uint32_t len,uint8_t *buf)//环形缓冲区初始化
{
	buffer->w = 0;
	buffer->r = 0;
	buffer->len = len;
	buffer->CircleBuf = buf;
}

int Circle_Read(pCircle_Buf buffer,uint8_t *pVal)//环形缓冲区读函数
{
	if(buffer->r != buffer->w)//如果读的位置不等于写的位置，即环形缓冲区非空，可读
	{
		*pVal = buffer->CircleBuf[buffer->r];//读取当前读位置的数据
		buffer->r++;
		if(buffer->r == buffer->len)//如果读位置到达环形缓冲区的长度
		{
			buffer->r = 0;//从头开始读
		}
		return 0;//正确返回0
	}
	else
	{
		return -1;//错误返回-1
	}
}

int Circle_Write(pCircle_Buf buffer,uint8_t val)//环形缓冲区写函数
{
	buffer->next_w = buffer->w + 1;
	if(buffer->next_w  == buffer->len)//如果下一个写位置等于环形缓冲区的长度
		buffer->next_w = 0;//下一个写位置到达最初的头位置
	if(buffer->next_w != buffer->r)//如果下一个写位置不等于读位置，即环形缓冲区未满，可写
	{
		buffer->CircleBuf[buffer->w] = val;//将数据写入当前写位置
		buffer->w = buffer->next_w;
		return 0;//正确返回0
	}
	else
	{
		return -1;//正确返回-1
	}
}



