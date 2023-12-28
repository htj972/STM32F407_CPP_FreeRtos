/**
* @Author kokirika
* @Name WDG
* @Date 2023-09-11
*/
#ifndef KOKIRIKA_WDG_H
#define KOKIRIKA_WDG_H

#include "sys.h"

#if __cplusplus
extern "C"{
#endif

//独立看门狗/////////////////////////
//prer:分频数:0~7(只有第三位有效!)
//分频因子=4*2^prer.最大值只能256!  4/8/16/32/64/128/256/256
//rlr:重装载值:低12位有效,最大值只能4095
//Tout=((4*2^prer)*rlr)/32 (ms) 4ms~131s
#define IWDG_prer	4
#define IWDG_rlr	1000
void IWDG_Init(void); //1*1000ms
void IWDG_Feed(void);

//窗口看门狗////////////////////////////
//保存WWDG计数器的设置值,默认为最大.
#define WWDG_OUT_CNT	0x7f			//装载值
#define WWDG_WAT_CNT	0x5f			//窗口值  Twwdg=(4096*8*(0X7F-0X5F))/36000 =29.18ms
#define WWDG_prer			WWDG_Prescaler_8			//分频值
/*
WWDG_Prescaler_1	5ms
WWDG_Prescaler_2	10ms
WWDG_Prescaler_4	30ms
WWDG_Prescaler_8	60ms
*/
//中断时间	Twwdg=(4096*8*(0X7F-0X40+1))/36000 = 58.25ms
//tr   :T[6:0],计数器值 0~0x7f	7位有效
//wr   :W[6:0],窗口值 	0~0x7f	7位有效		0x7f>WWDG_WAT_CNT>0x40
//fprer:分频系数（WDGTB）,仅[8:7]位有效  00 80 100 180  WWDG_Prescaler_8
//Twwdg=(4096*2^WDGTB*(T[5:0]+1))
void WWDG_Init(void);//初始化WWDG
void WWDG_Feed(void);       //设置WWDG的计数器
void WWDG_NVIC_Init(void);

#define MY_IWDG	 ON
#define MY_WWDG	 OFF

void WDG_Init(void);
void Feed_Dog(void);

#if __cplusplus
}
#endif

#endif //KOKIRIKA_WDG_H
