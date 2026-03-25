#include "main.h"
#ifndef __PROCESS__H
#define __PROCESS__H


void Water_Speed(void);
void Data_Process(void);
void Water_Status(void);
void Get_Buffer_Init(void);
uint8_t Data_Verify(void);

/*一般配置3k  12pF*/
#define xibanya 9000  //10pF 3k
#define shate_yijie 7000
#define shate_erjie 7000
#define shate_NB 13500
#define bilu 9000
#define xiaokoujing 13500
#define yidaibiao 13500 //一代表配2.2k，33pF  脉宽UP:57 脉宽DOWN:56 幅值UP:242 幅值DOWN：251  脉宽要求45~55  幅值需大于200

/*
国外的计量一致，都使用13500
配2.2k，33pF
还未验证，目前西班牙和秘鲁巴西验证一致
*/
#define abroad 13500


#endif

