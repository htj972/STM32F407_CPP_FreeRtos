/**
* @Author kokirika
* @Name IAP
* @Date 2022-12-23
**/

#ifndef KOKIRIKA_IAP_H
#define KOKIRIKA_IAP_H

#include "STM_FLASH.h"

#define FLASH_APP1_ADDR		0x8000000  	//第一个应用程序起始地址(存放在FLASH)

class IAP {
protected:
    typedef  void (*iapfun)();//定义一个函数类型的参数.
private:
    iapfun jump2app;
    uint32_t iapbuf[512]; 	//2K字节缓存
public:
    void load_app(uint32_t appxaddr);			//跳转到APP程序执行
    void write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//在指定地址开始,写入bin

};


#endif //KOKIRIKA_IAP_H
