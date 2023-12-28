/**
* @Author kokirika
* @Name WDG
* @Date 2023-09-11
*/
#include "WDG.h"


//0x7F,0X5F,WWDG_Prescaler_8
//IWDG_Init(4,500); //与分频数为64,重载值为500,溢出时间为1s
void IWDG_Init()
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);  //使能对寄存器IWDG_PR和IWDG_RLR的写操作
    IWDG_SetPrescaler(IWDG_prer);  //设置IWDG预分频值:设置IWDG预分频值为64
    IWDG_SetReload(IWDG_rlr);  //设置IWDG重装载值
    IWDG_ReloadCounter();  //按照IWDG重装载寄存器的值重装载IWDG计数器
    IWDG_Enable();  //使能IWDG
}
//喂独立看门狗
void IWDG_Feed(void)
{
    IWDG_ReloadCounter();	//重载计数值
}


void WWDG_Init()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);  //   WWDG时钟使能
    WWDG_SetPrescaler(WWDG_prer);////设置IWDG预分频值
    WWDG_SetWindowValue(WWDG_WAT_CNT);//设置窗口值
    WWDG_Enable(WWDG_OUT_CNT);	 //使能看门狗 ,	设置 counter .
    WWDG_ClearFlag();//清除提前唤醒中断标志位
    WWDG_NVIC_Init();//初始化窗口看门狗 NVIC
    WWDG_EnableIT(); //开启窗口看门狗中断
}

//窗口看门狗中断服务程序
void WWDG_NVIC_Init()
{
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;    //WWDG中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //抢占2，子优先级3，组2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	 //抢占2，子优先级3，组2
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_Init(&NVIC_InitStructure);//NVIC初始化
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

//重设置WWDG计数器的值
void WWDG_Feed()
{
    WWDG_Enable(WWDG_OUT_CNT);//使能看门狗 ,	设置 counter .
    WWDG_ClearFlag();	  //清除提前唤醒中断标志位
}

void WWDG_IRQHandler(void)
{
    INTX_DISABLE();									//关闭所有中断
#if	POWER_DOWN_SAVE ==	ON
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, DISABLE);		//关闭窗口看门狗时钟
//    Power_Down_Save();
#endif
    WWDG_ClearFlag();	  //清除提前唤醒中断标志位
#if	POWER_DOWN_SAVE ==	ON
    NVIC_SystemReset();																				//手动重启
#endif
}
