/**
* @Author kokirika
* @Name IAP
* @Date 2022-12-23
**/

#include "IAP.h"

/*!
 * 从指定地址开始写入指定长度的数据
 * @param appxaddr:应用程序的起始地址
 * @param appbuf:应用程序CODE.
 * @param appsize:应用程序大小(字节)
 * @return
 */
void IAP::write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t appsize)
{
    uint32_t t;
    uint16_t i=0;
    uint32_t temp;
    uint32_t fwaddr=appxaddr;//当前写入的地址
    uint8_t *dfu=appbuf;
    for(t=0;t<appsize;t+=4)
    {
        temp=(u32)dfu[3]<<24;
        temp|=(u32)dfu[2]<<16;
        temp|=(u32)dfu[1]<<8;
        temp|=(u32)dfu[0];
        dfu+=4;//偏移4个字节
        iapbuf[i++]=temp;
        if(i==512)
        {
            i=0;
            STM_FLASH::Write(fwaddr,iapbuf,512);
            fwaddr+=2048;//偏移2048  512*4=2048
        }
    }
    if(i)STM_FLASH::Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.
}
/*!
 * 从指定地址开始写入指定长度的数据
 * @param appxaddr:用户代码起始地址
 * @return
 */
void IAP::load_app(uint32_t appxaddr)
{
    if(((*(volatile uint32_t *)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
    {
        jump2app=(iapfun)*(volatile uint32_t *)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)
        MSR_MSP(*(volatile uint32_t *)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
        jump2app();									//跳转到APP.
    }
}


