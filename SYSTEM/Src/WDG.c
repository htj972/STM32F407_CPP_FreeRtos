/**
* @Author kokirika
* @Name WDG
* @Date 2023-09-11
*/
#include "WDG.h"


//0x7F,0X5F,WWDG_Prescaler_8
//IWDG_Init(4,500); //���Ƶ��Ϊ64,����ֵΪ500,���ʱ��Ϊ1s
void IWDG_Init()
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //ʹ�ܶԼĴ���IWDG_PR��IWDG_RLR��д����
    IWDG_SetPrescaler(IWDG_prer);  //����IWDGԤ��Ƶֵ:����IWDGԤ��ƵֵΪ64
    IWDG_SetReload(IWDG_rlr);  //����IWDG��װ��ֵ
    IWDG_ReloadCounter();  //����IWDG��װ�ؼĴ�����ֵ��װ��IWDG������
    IWDG_Enable();  //ʹ��IWDG
}
//ι�������Ź�
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();	//���ؼ���ֵ
}


void WWDG_Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);  //   WWDGʱ��ʹ��
    WWDG_SetPrescaler(WWDG_prer);////����IWDGԤ��Ƶֵ
    WWDG_SetWindowValue(WWDG_WAT_CNT);//���ô���ֵ
    WWDG_Enable(WWDG_OUT_CNT);	 //ʹ�ܿ��Ź� ,	���� counter .
    WWDG_ClearFlag();//�����ǰ�����жϱ�־λ
    WWDG_NVIC_Init();//��ʼ�����ڿ��Ź� NVIC
    WWDG_EnableIT(); //�������ڿ��Ź��ж�
}

//���ڿ��Ź��жϷ������
void WWDG_NVIC_Init()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;    //WWDG�ж�
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //��ռ2�������ȼ�3����2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	 //��ռ2�������ȼ�3����2
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);//NVIC��ʼ��
}

void WDG_Init(void)
{
#if MY_IWDG	== ON
    IWDG_Init();
#endif
#if MY_WWDG	== ON
    WWDG_Init();
#endif
}

void Feed_Dog(void)
{
#if MY_IWDG	== ON
    IWDG_Feed();
#endif
#if MY_WWDG	== ON
    WWDG_Feed();
#endif
}

//������WWDG��������ֵ
void WWDG_Feed()
{
    WWDG_Enable(WWDG_OUT_CNT);//ʹ�ܿ��Ź� ,	���� counter .
    WWDG_ClearFlag();	  //�����ǰ�����жϱ�־λ
}

void WWDG_IRQHandler(void)
{
    INTX_DISABLE();									//�ر������ж�
#if	POWER_DOWN_SAVE ==	ON
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, DISABLE);		//�رմ��ڿ��Ź�ʱ��
//    Power_Down_Save();
#endif
    WWDG_ClearFlag();	  //�����ǰ�����жϱ�־λ
#if	POWER_DOWN_SAVE ==	ON
    NVIC_SystemReset();																				//�ֶ�����
#endif
}
