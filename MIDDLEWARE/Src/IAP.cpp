/**
* @Author kokirika
* @Name IAP
* @Date 2022-12-23
**/

#include "IAP.h"

/*!
 * ��ָ����ַ��ʼд��ָ�����ȵ�����
 * @param appxaddr:Ӧ�ó������ʼ��ַ
 * @param appbuf:Ӧ�ó���CODE.
 * @param appsize:Ӧ�ó����С(�ֽ�)
 * @return
 */
void IAP::write_appbin(uint32_t appxaddr,uint8_t *appbuf,uint32_t appsize)
{
    uint32_t t;
    uint16_t i=0;
    uint32_t temp;
    uint32_t fwaddr=appxaddr;//��ǰд��ĵ�ַ
    uint8_t *dfu=appbuf;
    for(t=0;t<appsize;t+=4)
    {
        temp=(u32)dfu[3]<<24;
        temp|=(u32)dfu[2]<<16;
        temp|=(u32)dfu[1]<<8;
        temp|=(u32)dfu[0];
        dfu+=4;//ƫ��4���ֽ�
        iapbuf[i++]=temp;
        if(i==512)
        {
            i=0;
            STM_FLASH::Write(fwaddr,iapbuf,512);
            fwaddr+=2048;//ƫ��2048  512*4=2048
        }
    }
    if(i)STM_FLASH::Write(fwaddr,iapbuf,i);//������һЩ�����ֽ�д��ȥ.
}
/*!
 * ��ָ����ַ��ʼд��ָ�����ȵ�����
 * @param appxaddr:�û�������ʼ��ַ
 * @return
 */
void IAP::load_app(uint32_t appxaddr)
{
    if(((*(volatile uint32_t *)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
    {
        jump2app=(iapfun)*(volatile uint32_t *)(appxaddr+4);		//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)
        MSR_MSP(*(volatile uint32_t *)appxaddr);					//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
        jump2app();									//��ת��APP.
    }
}


