#ifndef SYS_H
#define SYS_H
#include "stm32f4xx.h"
//#include <iostream>

//0,不支持ucos
//1,支持ucos

#define ON	1
#define	OFF	0

#define HIGH	1
#define	LOW		0

#define SYSTEM_SUPPORT_OS		ON		//定义系统文件夹是否支持FREE_RTOS

//关闭所有中断
#define INTX_DISABLE()  __ASM volatile("cpsid i")
//开启所有中断
#define INTX_ENABLE() __ASM volatile("cpsie i")
//采用如下方法实现执行汇编指令WFI
#define WFI_SET()     __ASM volatile("wfi")
	 
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).M4同M3类似,只是寄存器地址变了.
//IO口操作宏定义
#define BITBAND(addr, bitnum) (((addr) & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+20) //0x40020014
#define GPIOB_ODR_Addr    (GPIOB_BASE+20) //0x40020414 
#define GPIOC_ODR_Addr    (GPIOC_BASE+20) //0x40020814 
#define GPIOD_ODR_Addr    (GPIOD_BASE+20) //0x40020C14 
#define GPIOE_ODR_Addr    (GPIOE_BASE+20) //0x40021014 
#define GPIOF_ODR_Addr    (GPIOF_BASE+20) //0x40021414    
#define GPIOG_ODR_Addr    (GPIOG_BASE+20) //0x40021814   
#define GPIOH_ODR_Addr    (GPIOH_BASE+20) //0x40021C14    
#define GPIOI_ODR_Addr    (GPIOI_BASE+20) //0x40022014     

#define GPIOA_IDR_Addr    (GPIOA_BASE+16) //0x40020010 
#define GPIOB_IDR_Addr    (GPIOB_BASE+16) //0x40020410 
#define GPIOC_IDR_Addr    (GPIOC_BASE+16) //0x40020810 
#define GPIOD_IDR_Addr    (GPIOD_BASE+16) //0x40020C10 
#define GPIOE_IDR_Addr    (GPIOE_BASE+16) //0x40021010 
#define GPIOF_IDR_Addr    (GPIOF_BASE+16) //0x40021410 
#define GPIOG_IDR_Addr    (GPIOG_BASE+16) //0x40021810 
#define GPIOH_IDR_Addr    (GPIOH_BASE+16) //0x40021C10 
#define GPIOI_IDR_Addr    (GPIOI_BASE+16) //0x40022010 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

#define PHout(n)   BIT_ADDR(GPIOH_ODR_Addr,n)  //输出 
#define PHin(n)    BIT_ADDR(GPIOH_IDR_Addr,n)  //输入

#define PIout(n)   BIT_ADDR(GPIOI_ODR_Addr,n)  //输出 
#define PIin(n)    BIT_ADDR(GPIOI_IDR_Addr,n)  //输入


#define Pin_0				GPIO_Pin_0
#define Pin_1				GPIO_Pin_1
#define Pin_2				GPIO_Pin_2
#define Pin_3				GPIO_Pin_3
#define Pin_4				GPIO_Pin_4
#define Pin_5				GPIO_Pin_5
#define Pin_6				GPIO_Pin_6
#define Pin_7				GPIO_Pin_7
#define Pin_8				GPIO_Pin_8
#define Pin_9				GPIO_Pin_9
#define Pin_10			    GPIO_Pin_10
#define Pin_11			    GPIO_Pin_11
#define Pin_12			    GPIO_Pin_12
#define Pin_13			    GPIO_Pin_13
#define Pin_14			    GPIO_Pin_14
#define Pin_15			    GPIO_Pin_15

typedef enum
{
    GPIOA0   = 0x00,
    GPIOA1   = 0x01,
    GPIOA2   = 0x02,
    GPIOA3   = 0x03,
    GPIOA4   = 0x04,
    GPIOA5   = 0x05,
    GPIOA6   = 0x06,
    GPIOA7   = 0x07,
    GPIOA8   = 0x08,
    GPIOA9   = 0x09,
    GPIOA10  = 0x0a,
    GPIOA11  = 0x0b,
    GPIOA12  = 0x0c,
    GPIOA13  = 0x0d,
    GPIOA14  = 0x0e,
    GPIOA15  = 0x0f,

    GPIOB0   = 0x10,
    GPIOB1   = 0x11,
    GPIOB2   = 0x12,
    GPIOB3   = 0x13,
    GPIOB4   = 0x14,
    GPIOB5   = 0x15,
    GPIOB6   = 0x16,
    GPIOB7   = 0x17,
    GPIOB8   = 0x18,
    GPIOB9   = 0x19,
    GPIOB10  = 0x1a,
    GPIOB11  = 0x1b,
    GPIOB12  = 0x1c,
    GPIOB13  = 0x1d,
    GPIOB14  = 0x1e,
    GPIOB15  = 0x1f,

    GPIOC0   = 0x20,
    GPIOC1   = 0x21,
    GPIOC2   = 0x22,
    GPIOC3   = 0x23,
    GPIOC4   = 0x24,
    GPIOC5   = 0x25,
    GPIOC6   = 0x26,
    GPIOC7   = 0x27,
    GPIOC8   = 0x28,
    GPIOC9   = 0x29,
    GPIOC10  = 0x2a,
    GPIOC11  = 0x2b,
    GPIOC12  = 0x2c,
    GPIOC13  = 0x2d,
    GPIOC14  = 0x2e,
    GPIOC15  = 0x2f,

    GPIOD0   = 0x30,
    GPIOD1   = 0x31,
    GPIOD2   = 0x32,
    GPIOD3   = 0x33,
    GPIOD4   = 0x34,
    GPIOD5   = 0x35,
    GPIOD6   = 0x36,
    GPIOD7   = 0x37,
    GPIOD8   = 0x38,
    GPIOD9   = 0x39,
    GPIOD10  = 0x3a,
    GPIOD11  = 0x3b,
    GPIOD12  = 0x3c,
    GPIOD13  = 0x3d,
    GPIOD14  = 0x3e,
    GPIOD15  = 0x3f,

    GPIOE0   = 0x40,
    GPIOE1   = 0x41,
    GPIOE2   = 0x42,
    GPIOE3   = 0x43,
    GPIOE4   = 0x44,
    GPIOE5   = 0x45,
    GPIOE6   = 0x46,
    GPIOE7   = 0x47,
    GPIOE8   = 0x48,
    GPIOE9   = 0x49,
    GPIOE10  = 0x4a,
    GPIOE11  = 0x4b,
    GPIOE12  = 0x4c,
    GPIOE13  = 0x4d,
    GPIOE14  = 0x4e,
    GPIOE15  = 0x4f,

    GPIOF0   = 0x50,
    GPIOF1   = 0x51,
    GPIOF2   = 0x52,
    GPIOF3   = 0x53,
    GPIOF4   = 0x54,
    GPIOF5   = 0x55,
    GPIOF6   = 0x56,
    GPIOF7   = 0x57,
    GPIOF8   = 0x58,
    GPIOF9   = 0x59,
    GPIOF10  = 0x5a,
    GPIOF11  = 0x5b,
    GPIOF12  = 0x5c,
    GPIOF13  = 0x5d,
    GPIOF14  = 0x5e,
    GPIOF15  = 0x5f,

    GPIOG0   = 0x60,
    GPIOG1   = 0x61,
    GPIOG2   = 0x62,
    GPIOG3   = 0x63,
    GPIOG4   = 0x64,
    GPIOG5   = 0x65,
    GPIOG6   = 0x66,
    GPIOG7   = 0x67,
    GPIOG8   = 0x68,
    GPIOG9   = 0x69,
    GPIOG10  = 0x6a,
    GPIOG11  = 0x6b,
    GPIOG12  = 0x6c,
    GPIOG13  = 0x6d,
    GPIOG14  = 0x6e,
    GPIOG15  = 0x6f,

    GPIOH0   = 0x70,
    GPIOH1   = 0x71,
    GPIOH2   = 0x72,
    GPIOH3   = 0x73,
    GPIOH4   = 0x74,
    GPIOH5   = 0x75,
    GPIOH6   = 0x76,
    GPIOH7   = 0x77,
    GPIOH8   = 0x78,
    GPIOH9   = 0x79,
    GPIOH10  = 0x7a,
    GPIOH11  = 0x7b,
    GPIOH12  = 0x7c,
    GPIOH13  = 0x7d,
    GPIOH14  = 0x7e,
    GPIOH15  = 0x7f,
}GPIO_Pin;

//以下为汇编函数
void MSR_MSP(u32 addr);	//设置堆栈地址

extern void run_void(void);								//空指令函数   初始函数指针使用
extern void run_void_data(u8 data);						//空指令传入函数

#endif











