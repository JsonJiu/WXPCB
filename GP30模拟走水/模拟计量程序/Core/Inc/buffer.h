#include "main.h"
#ifndef __BUFFER_H
#define __BUFFER_H



typedef struct buffer{
	uint32_t w;//写位置
	uint32_t r;//读位置
	uint32_t len;//环形缓冲区长度
	uint32_t next_w;//下一个写位置
	uint8_t *CircleBuf;//环形缓冲区
	
}circle_buffer,*pCircle_Buf;

void Circle_Buffer_Init(pCircle_Buf buffer,uint32_t len,uint8_t *buf);//环形缓冲区初始化

int Circle_Read(pCircle_Buf buffer,uint8_t *pVal);//环形缓冲区读函数

int Circle_Write(pCircle_Buf buffer,uint8_t val);//环形缓冲区写函数


#endif


