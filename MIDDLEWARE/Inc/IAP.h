/**
* @Author kokirika
* @Name IAP
* @Date 2022-12-23
**/

#ifndef KOKIRIKA_IAP_H
#define KOKIRIKA_IAP_H

#include "STM_FLASH.h"

#define FLASH_APP1_ADDR		0x8000000  	//��һ��Ӧ�ó�����ʼ��ַ(�����FLASH)

class IAP {
protected:
    typedef  void (*iapfun)();//����һ���������͵Ĳ���.
private:
    iapfun jump2app;
    uint32_t iapbuf[512]; 	//2K�ֽڻ���
public:
    void load_app(uint32_t appxaddr);			//��ת��APP����ִ��
    void write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t applen);	//��ָ����ַ��ʼ,д��bin

};


#endif //KOKIRIKA_IAP_H
