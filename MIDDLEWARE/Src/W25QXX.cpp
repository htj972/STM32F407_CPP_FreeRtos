/**
* @Author kokirika
* @Name W25QXX
* @Date 2022-10-18
**/

#include "W25QXX.h"
#include "delay.h"

#define     W25X_WriteStatusReg		0x01
#define     W25X_PageProgram		0x02
#define     W25X_ReadData			0x03
#define     W25X_WriteDisable		0x04
#define     W25X_ReadStatusReg		0x05
#define     W25X_WriteEnable		0x06
#define     W25X_FastReadData		0x0B
#define     W25X_SectorErase		0x20
#define     W25X_FastReadDual		0x3B
#define     W25X_ManufactDeviceID	0x90
#define     W25X_JedecDeviceID		0x9F
#define     W25X_ReleasePowerDown	0xAB
#define     W25X_DeviceID			0xAB
#define     W25X_PowerDown			0xB9
#define     W25X_ChipErase			0xC7
#define     W25X_BlockErase			0xD8


W25QXX::W25QXX(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx, uint16_t BaudRate) {
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->CSPin.init(PORTx,Pinx,GPIO_Mode_OUT);
}

W25QXX::W25QXX(SPI *SPIx, uint8_t CSpin, uint16_t BaudRate) {
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->CSPin.init(CSpin,GPIO_Mode_OUT);
}

void W25QXX::init() {
    if((this->spix!= nullptr)&&(this->CSPin.Pinx!=0xff))
    {
        this->CSPin.set_output(HIGH);
        this->spix->SetSpeed(this->BaudRatex);
        W25QXX_TYPE=this->ReadID();	//读取FLASH ID.
    }
}

void W25QXX::init(SPI *SPIx, GPIO_TypeDef *PORTx, uint32_t Pinx, uint16_t BaudRate) {
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->CSPin.init(PORTx,Pinx,GPIO_Mode_OUT);
    this->CSPin.set_output(HIGH);
    this->spix->SetSpeed(BaudRate);
    W25QXX_TYPE=this->ReadID();	//读取FLASH ID.
}

void W25QXX::init(SPI *SPIx,uint8_t CSpin,uint16_t BaudRate)
{
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->CSPin.init(CSpin,GPIO_Mode_OUT);
    this->CSPin.set_output(HIGH);
    this->spix->SetSpeed(BaudRate);
    W25QXX_TYPE=this->ReadID();	//读取FLASH ID.
}

uint16_t W25QXX::ReadID() const
{
    uint16_t Temp = 0;
    this->CSPin.set_output(LOW);
    this->spix->ReadWriteDATA(W25X_ManufactDeviceID);//发送读取ID命令
    this->spix->ReadWriteDATA(0x00);
    this->spix->ReadWriteDATA(0x00);
    this->spix->ReadWriteDATA(0x00);

    Temp|=this->spix->ReadWriteDATA(0xFF)<<8;
    Temp|=this->spix->ReadWriteDATA(0xFF);
    this->CSPin.set_output(HIGH);
    return Temp;
}

//读取W25QXX的状态寄存器
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:默认0,状态寄存器保护位,配合WP使用
//TB,BP2,BP1,BP0:FLASH区域写保护设置
//WEL:写使能锁定
//BUSY:忙标记位(1,忙;0,空闲)
//默认:0x00
uint8_t W25QXX::ReadSR() const
{
    this->CSPin.set_output(LOW);         //使能器件
    this->spix->ReadWriteDATA(W25X_ReadStatusReg);//发送读取状态寄存器命令
    uint8_t byte=this->spix->ReadWriteDATA(0Xff);//读取一个字节
    this->CSPin.set_output(HIGH);        //取消片选
    return byte;
}
//写W25QXX状态寄存器
//只有SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)可以写!!!
void W25QXX::Write_SR(uint8_t sr) const
{
    this->CSPin.set_output(LOW);         //使能器件
    this->spix->ReadWriteDATA(W25X_WriteStatusReg);//发送写取状态寄存器命令
    this->spix->ReadWriteDATA(sr);       //写入一个字节
    this->CSPin.set_output(HIGH);       //取消片选
}
//W25QXX写使能
//将WEL置位
void W25QXX::Write_Enable() const
{
    this->CSPin.set_output(LOW);         //使能器件
    this->spix->ReadWriteDATA(W25X_WriteEnable);//发送写使能
    this->CSPin.set_output(HIGH);        //取消片选
}
//W25QXX写禁止
//将WEL清零
void W25QXX::Write_Disable() const
{
    this->CSPin.set_output(LOW);         //使能器件
    this->spix->ReadWriteDATA(W25X_WriteDisable);//发送写禁止指令
    this->CSPin.set_output(HIGH);        //取消片选
}

//等待空闲
void W25QXX::Wait_Busy() const
{
    uint16_t wait_times_t=this->wait_times;
    while(wait_times_t){// 等待BUSY位清空
        if((this->ReadSR()&0x01)==0x01)
            break;
        wait_times_t-=10;
        if(wait_times_t<10)
            break;
        delay_ms(10);
    }
}
//擦除整个芯片
//等待时间超长...
void W25QXX::Erase_Chip() const
{
    this->Write_Enable();                 //设置写使能
    this->Wait_Busy();
    this->CSPin.set_output(LOW);    //使能器件
    this->spix->ReadWriteDATA(W25X_ChipErase);//发送片擦除命令
    this->CSPin.set_output(HIGH);   //取消片选
    this->Wait_Busy();   				  //等待芯片擦除结束
}
//擦除一个扇区
//Dst_Addr:扇区地址 根据实际容量设置
//擦除一个山区的最少时间:150ms
void W25QXX::Erase_Sector(uint32_t Dst_Addr) const
{
    Dst_Addr*=4096;
    this->Write_Enable();                  //SET WEL
    this->Wait_Busy();
    this->CSPin.set_output(LOW);    //使能器件
    this->spix->ReadWriteDATA(W25X_SectorErase);      //发送扇区擦除指令
    this->spix->ReadWriteDATA((uint8_t)((Dst_Addr)>>16));  //发送24bit地址
    this->spix->ReadWriteDATA((uint8_t)((Dst_Addr)>>8));
    this->spix->ReadWriteDATA((uint8_t)Dst_Addr);
    this->CSPin.set_output(HIGH);   //取消片选
    this->Wait_Busy();   				  //等待擦除完成
}
//进入掉电模式
void W25QXX::PowerDown() const
{
    this->CSPin.set_output(LOW);    //使能器件
    this->spix->ReadWriteDATA(W25X_PowerDown);        //发送掉电命令
    this->CSPin.set_output(HIGH);   //取消片选
    delay_us(3);                     //等待TPD
}
//唤醒
void W25QXX::WAKEUP() const
{
    this->CSPin.set_output(LOW);    //使能器件
    this->spix->ReadWriteDATA(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB
    this->CSPin.set_output(HIGH);   //取消片选
    delay_us(3);                     //等待TRES1
}

uint16_t W25QXX::GetID() const {
    return this->W25QXX_TYPE;
}
//SPI在一页(0~65535)内写入少于256个字节的数据
//在指定地址开始写入最大256字节的数据
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大256),该数不应该超过该页的剩余字节数!!!
void W25QXX::Write_Page(uint32_t Addr, uint8_t* pBuffer, uint16_t NumByte) const
{
    this->Write_Enable();                  //SET WEL
    this->CSPin.set_output(LOW);     //使能器件
    this->spix->ReadWriteDATA(W25X_PageProgram);//发送写页命令
    this->spix->ReadWriteDATA((uint8_t)((Addr)>>16)); //发送24bit地址
    this->spix->ReadWriteDATA((uint8_t)((Addr)>>8));
    this->spix->ReadWriteDATA((uint8_t)Addr);
    for(uint16_t ii=0;ii<NumByte;ii++)
        this->spix->ReadWriteDATA(pBuffer[ii]);//循环写数
    this->CSPin.set_output(HIGH);    //取消片选
    this->Wait_Busy();					   //等待写入结束
}
//无检验写SPI FLASH
//必须确保所写的地址范围内的数据全部为0XFF,否则在非0XFF处写入的数据将失败!
//具有自动换页功能
//在指定地址开始写入指定长度的数据,但是要确保地址不越界!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
//CHECK OK
void W25QXX::Write_NoCheck(uint32_t Addr,uint8_t* pBuffer,uint16_t NumByte) const
{
    uint16_t pageremain=256-Addr%256; //单页剩余的字节数
    if(NumByte<=pageremain)pageremain=NumByte;//不大于256个字节
    while(true)
    {
        this->Write_Page(Addr,pBuffer,pageremain);
        if(NumByte==pageremain)break;//写入结束了
        else //NumByteToWrite>pageremain
        {
            pBuffer+=pageremain;
            Addr+=pageremain;

            NumByte-=pageremain;		   //减去已经写入了的字节数
            if(NumByte>256)pageremain=256; //一次可以写入256个字节
            else pageremain=NumByte; 	   //不够256个字节了
        }
    }
}
//写SPI FLASH
//在指定地址开始写入指定长度的数据
//该函数带擦除操作!
//pBuffer:数据存储区
//WriteAddr:开始写入的地址(24bit)
//NumByteToWrite:要写入的字节数(最大65535)
uint16_t W25QXX::write(uint32_t Addr, uint8_t *pBuffer, uint16_t NumByte) {
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    secpos=Addr/4096;//扇区地址
    secoff=Addr%4096;//在扇区内的偏移
    secremain=4096-secoff;//扇区剩余空间大小
    if(NumByte<=secremain)secremain=NumByte;//不大于4096个字节
    while(true)
    {
        //this->read(secpos*4096,(uint8_t*)this->get_data.data(),4096);//读出整个扇区的内容
        this->readstr(secpos*4096,&this->get_data,4096);
        for(i=0;i<secremain;i++)//校验数据
        {
            if(this->get_data[secoff+i]!=0XFF)break;//需要擦除
        }
        if(i<secremain)//需要擦除
        {
            this->Erase_Sector(secpos);//擦除这个扇区
            for(i=0;i<secremain;i++)	   //复制
            {
                this->get_data[i+secoff]=pBuffer[i];
            }
            this->Write_NoCheck(secpos*4096,(uint8_t*)this->get_data.data(),4096);//写入整个扇区

        }else this->Write_NoCheck(Addr,pBuffer,secremain);//写已经擦除了的,直接写入扇区剩余区间.
        if(NumByte==secremain)break;//写入结束了
        else//写入未结束
        {
            secpos++;//扇区地址增1
            secoff=0;//偏移位置为0

            pBuffer+=secremain;  //指针偏移
            Addr+=secremain;//写地址偏移
            NumByte-=secremain;				//字节数递减
            if(NumByte>4096)secremain=4096;	//下一个扇区还是写不完
            else secremain=NumByte;			//下一个扇区可以写完了
        }
    }
    return NumByte;
}

uint16_t W25QXX::write(uint32_t addr, uint8_t data) {
    return this->write(addr,&data,1);
}
//读取SPI FLASH
//在指定地址开始读取指定长度的数据
//pBuffer:数据存储区
//ReadAddr:开始读取的地址(24bit)
//NumByteToRead:要读取的字节数(最大65535)
void W25QXX::read(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte) {
    this->CSPin.set_output(LOW);     //使能器件
    this->spix->ReadWriteDATA(W25X_ReadData);     //发送读取命令
    this->spix->ReadWriteDATA((u8)((Addr)>>16));  //发送24bit地址
    this->spix->ReadWriteDATA((u8)((Addr)>>8));
    this->spix->ReadWriteDATA((u8)Addr);
    for(uint16_t i=0;i<NumByte;i++)
    {
        pBuffer[i]=this->spix->ReadWriteDATA(0XFF);//循环读数
    }
    this->CSPin.set_output(HIGH);     //取消片选
}

uint8_t W25QXX::read(uint32_t addr) {
    uint8_t data_t;
    this->read(addr,&data_t,1);
    return data_t;
}

void W25QXX::read(uint32_t addr, uint8_t *data) {
    this->read(addr,data,1);
}



