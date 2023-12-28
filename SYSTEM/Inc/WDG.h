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

//�������Ź�/////////////////////////
//prer:��Ƶ��:0~7(ֻ�е���λ��Ч!)
//��Ƶ����=4*2^prer.���ֵֻ��256!  4/8/16/32/64/128/256/256
//rlr:��װ��ֵ:��12λ��Ч,���ֵֻ��4095
//Tout=((4*2^prer)*rlr)/32 (ms) 4ms~131s
#define IWDG_prer	4
#define IWDG_rlr	1000
void IWDG_Init(void); //1*1000ms
void IWDG_Feed(void);

//���ڿ��Ź�////////////////////////////
//����WWDG������������ֵ,Ĭ��Ϊ���.
#define WWDG_OUT_CNT	0x7f			//װ��ֵ
#define WWDG_WAT_CNT	0x5f			//����ֵ  Twwdg=(4096*8*(0X7F-0X5F))/36000 =29.18ms
#define WWDG_prer			WWDG_Prescaler_8			//��Ƶֵ
/*
WWDG_Prescaler_1	5ms
WWDG_Prescaler_2	10ms
WWDG_Prescaler_4	30ms
WWDG_Prescaler_8	60ms
*/
//�ж�ʱ��	Twwdg=(4096*8*(0X7F-0X40+1))/36000 = 58.25ms
//tr   :T[6:0],������ֵ 0~0x7f	7λ��Ч
//wr   :W[6:0],����ֵ 	0~0x7f	7λ��Ч		0x7f>WWDG_WAT_CNT>0x40
//fprer:��Ƶϵ����WDGTB��,��[8:7]λ��Ч  00 80 100 180  WWDG_Prescaler_8
//Twwdg=(4096*2^WDGTB*(T[5:0]+1))
void WWDG_Init(void);//��ʼ��WWDG
void WWDG_Feed(void);       //����WWDG�ļ�����
void WWDG_NVIC_Init(void);

#define MY_IWDG	 ON
#define MY_WWDG	 OFF

void WDG_Init(void);
void Feed_Dog(void);

#if __cplusplus
}
#endif

#endif //KOKIRIKA_WDG_H
