/**
* @Author kokirika
* @Name FM24Cxx
* @Date 2022-10-19
**/

#include "FM24Cxx.h"

FM24Cxx::FM24Cxx(Software_IIC *IICx,uint16_t TYPE) {
    this->IIC=IICx;
    this->EE_TYPE=TYPE;
    //this->init();
}

bool FM24Cxx::init(Software_IIC *IICx, uint16_t TYPE) {
    this->IIC=IICx;
    this->EE_TYPE=TYPE;
    return this->init();
}

bool FM24Cxx::init() {
    if(this->EE_TYPE)
    {
        if(this->check()>0)
            return true;
        else
            return false;
    }
    return false;
}
//���24CXX�Ƿ�����
//��������24XX�����һ����ַ(255)���洢��־��.
//���������24Cϵ��,�����ַҪ�޸�
//����1:���ʧ��
//����0:���ɹ�
uint8_t FM24Cxx::check()
{
    uint8_t temp;
    temp=this->read(this->EE_TYPE);//����ÿ�ο�����дAT24CXX
    if(temp==0X55)return 2;
    else//�ų���һ�γ�ʼ�������
    {
        this->write(this->EE_TYPE,0X55);
        temp=this->read(this->EE_TYPE);
        if(temp==0X55)return 1;
    }
    return 0;
}

//��AT24CXXָ����ַ����һ������
//ReadAddr:��ʼ�����ĵ�ַ
//����ֵ  :����������
uint8_t FM24Cxx::read(uint32_t Addr)
{
//    OS_ERR err;
//    OSMutexPend (&IIC_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);//���󻥳��ź���
    uint8_t temp=0;
    this->IIC->Start();
    if(this->EE_TYPE>FM24Cxx::TYPE::AT24C16)
    {
        this->IIC->Send_Byte(0XA0);//����д����
        this->IIC->Wait_Ack();
        this->IIC->Send_Byte(Addr>>8);//���͸ߵ�ַ
    }else this->IIC->Send_Byte(0XA0+((Addr/256)<<1));   //����������ַ0XA0,д����
    this->IIC->Wait_Ack();
    this->IIC->Send_Byte(Addr%256);   //���͵͵�ַ
    this->IIC->Wait_Ack();
    this->IIC->Start();
    this->IIC->Send_Byte(0XA1);           //�������ģʽ
    this->IIC->Wait_Ack();
    temp=this->IIC->Read_Byte(0);
    this->IIC->Stop();//����һ��ֹͣ����
//    OSMutexPost(&IIC_MUTEX,OS_OPT_POST_NONE,&err);			//�ͷŻ����ź���
    return temp;
}

void FM24Cxx::read(uint32_t Addr, uint8_t *data) {
    *data=this->read(Addr);
}
//��AT24CXX�����ָ����ַ��ʼ����ָ������������
//ReadAddr :��ʼ�����ĵ�ַ ��24c02Ϊ0~255
//pBuffer  :���������׵�ַ
//NumToRead:Ҫ�������ݵĸ���
void FM24Cxx::read(uint32_t Addr,uint8_t *pBuffer,uint16_t Num)
{
    while(Num)
    {
        *pBuffer++=this->read(Addr++);
        Num--;
    }
}

//��AT24CXXָ����ַд��һ������
//WriteAddr  :д�����ݵ�Ŀ�ĵ�ַ
//DataToWrite:Ҫд�������
uint16_t FM24Cxx::write(uint32_t Addr,uint8_t Data)
{
//    OS_ERR err;
//    OSMutexPend (&IIC_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);//���󻥳��ź���
    this->IIC->Start();
    if(this->EE_TYPE>FM24Cxx::TYPE::AT24C16)
    {
        this->IIC->Send_Byte(0XA0);	    //����д����
        this->IIC->Wait_Ack();
        this->IIC->Send_Byte(Addr>>8);//���͸ߵ�ַ
    }else
    {
        this->IIC->Send_Byte(0XA0+((Addr/256)<<1));   //����������ַ0XA0,д����
    }
    this->IIC->Wait_Ack();
    this->IIC->Send_Byte(Addr%256);   //���͵͵�ַ
    this->IIC->Wait_Ack();
    this->IIC->Send_Byte(Data);     //�����ֽ�
    this->IIC->Wait_Ack();
    this->IIC->Stop();//����һ��ֹͣ����
    //delay_ms(10);
//    OSMutexPost(&IIC_MUTEX,OS_OPT_POST_NONE,&err);			//�ͷŻ����ź���
    return 1;
}

//��AT24CXX�����ָ����ַ��ʼд��ָ������������
//WriteAddr :��ʼд��ĵ�ַ ��24c02Ϊ0~255
//pBuffer   :���������׵�ַ
//NumToWrite:Ҫд�����ݵĸ���
uint16_t FM24Cxx::write(uint32_t Addr,uint8_t *pBuffer,uint16_t Num)
{
    while(Num--)
    {
        this->write(Addr,*pBuffer);
        Addr++;
        pBuffer++;
    }
    return Num;
}

