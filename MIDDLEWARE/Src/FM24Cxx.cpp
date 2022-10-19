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
//检查24CXX是否正常
//这里用了24XX的最后一个地址(255)来存储标志字.
//如果用其他24C系列,这个地址要修改
//返回1:检测失败
//返回0:检测成功
uint8_t FM24Cxx::check()
{
    uint8_t temp;
    temp=this->read(this->EE_TYPE);//避免每次开机都写AT24CXX
    if(temp==0X55)return 2;
    else//排除第一次初始化的情况
    {
        this->write(this->EE_TYPE,0X55);
        temp=this->read(this->EE_TYPE);
        if(temp==0X55)return 1;
    }
    return 0;
}

//在AT24CXX指定地址读出一个数据
//ReadAddr:开始读数的地址
//返回值  :读到的数据
uint8_t FM24Cxx::read(uint32_t Addr)
{
//    OS_ERR err;
//    OSMutexPend (&IIC_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);//请求互斥信号量
    uint8_t temp=0;
    this->IIC->Start();
    if(this->EE_TYPE>FM24Cxx::TYPE::AT24C16)
    {
        this->IIC->Send_Byte(0XA0);//发送写命令
        this->IIC->Wait_Ack();
        this->IIC->Send_Byte(Addr>>8);//发送高地址
    }else this->IIC->Send_Byte(0XA0+((Addr/256)<<1));   //发送器件地址0XA0,写数据
    this->IIC->Wait_Ack();
    this->IIC->Send_Byte(Addr%256);   //发送低地址
    this->IIC->Wait_Ack();
    this->IIC->Start();
    this->IIC->Send_Byte(0XA1);           //进入接收模式
    this->IIC->Wait_Ack();
    temp=this->IIC->Read_Byte(0);
    this->IIC->Stop();//产生一个停止条件
//    OSMutexPost(&IIC_MUTEX,OS_OPT_POST_NONE,&err);			//释放互斥信号量
    return temp;
}

void FM24Cxx::read(uint32_t Addr, uint8_t *data) {
    *data=this->read(Addr);
}
//在AT24CXX里面的指定地址开始读出指定个数的数据
//ReadAddr :开始读出的地址 对24c02为0~255
//pBuffer  :数据数组首地址
//NumToRead:要读出数据的个数
void FM24Cxx::read(uint32_t Addr,uint8_t *pBuffer,uint16_t Num)
{
    while(Num)
    {
        *pBuffer++=this->read(Addr++);
        Num--;
    }
}

//在AT24CXX指定地址写入一个数据
//WriteAddr  :写入数据的目的地址
//DataToWrite:要写入的数据
uint16_t FM24Cxx::write(uint32_t Addr,uint8_t Data)
{
//    OS_ERR err;
//    OSMutexPend (&IIC_MUTEX,0,OS_OPT_PEND_BLOCKING,0,&err);//请求互斥信号量
    this->IIC->Start();
    if(this->EE_TYPE>FM24Cxx::TYPE::AT24C16)
    {
        this->IIC->Send_Byte(0XA0);	    //发送写命令
        this->IIC->Wait_Ack();
        this->IIC->Send_Byte(Addr>>8);//发送高地址
    }else
    {
        this->IIC->Send_Byte(0XA0+((Addr/256)<<1));   //发送器件地址0XA0,写数据
    }
    this->IIC->Wait_Ack();
    this->IIC->Send_Byte(Addr%256);   //发送低地址
    this->IIC->Wait_Ack();
    this->IIC->Send_Byte(Data);     //发送字节
    this->IIC->Wait_Ack();
    this->IIC->Stop();//产生一个停止条件
    //delay_ms(10);
//    OSMutexPost(&IIC_MUTEX,OS_OPT_POST_NONE,&err);			//释放互斥信号量
    return 1;
}

//在AT24CXX里面的指定地址开始写入指定个数的数据
//WriteAddr :开始写入的地址 对24c02为0~255
//pBuffer   :数据数组首地址
//NumToWrite:要写入数据的个数
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

