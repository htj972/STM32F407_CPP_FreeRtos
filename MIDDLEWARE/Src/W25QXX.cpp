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
        W25QXX_TYPE=this->ReadID();	//��ȡFLASH ID.
    }
}

void W25QXX::init(SPI *SPIx, GPIO_TypeDef *PORTx, uint32_t Pinx, uint16_t BaudRate) {
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->CSPin.init(PORTx,Pinx,GPIO_Mode_OUT);
    this->CSPin.set_output(HIGH);
    this->spix->SetSpeed(BaudRate);
    W25QXX_TYPE=this->ReadID();	//��ȡFLASH ID.
}

void W25QXX::init(SPI *SPIx,uint8_t CSpin,uint16_t BaudRate)
{
    this->BaudRatex=BaudRate;
    this->spix=SPIx;
    this->CSPin.init(CSpin,GPIO_Mode_OUT);
    this->CSPin.set_output(HIGH);
    this->spix->SetSpeed(BaudRate);
    W25QXX_TYPE=this->ReadID();	//��ȡFLASH ID.
}

uint16_t W25QXX::ReadID() const
{
    uint16_t Temp = 0;
    this->CSPin.set_output(LOW);
    this->spix->ReadWriteDATA(W25X_ManufactDeviceID);//���Ͷ�ȡID����
    this->spix->ReadWriteDATA(0x00);
    this->spix->ReadWriteDATA(0x00);
    this->spix->ReadWriteDATA(0x00);

    Temp|=this->spix->ReadWriteDATA(0xFF)<<8;
    Temp|=this->spix->ReadWriteDATA(0xFF);
    this->CSPin.set_output(HIGH);
    return Temp;
}

//��ȡW25QXX��״̬�Ĵ���
//BIT7  6   5   4   3   2   1   0
//SPR   RV  TB BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
uint8_t W25QXX::ReadSR() const
{
    this->CSPin.set_output(LOW);         //ʹ������
    this->spix->ReadWriteDATA(W25X_ReadStatusReg);//���Ͷ�ȡ״̬�Ĵ�������
    uint8_t byte=this->spix->ReadWriteDATA(0Xff);//��ȡһ���ֽ�
    this->CSPin.set_output(HIGH);        //ȡ��Ƭѡ
    return byte;
}
//дW25QXX״̬�Ĵ���
//ֻ��SPR,TB,BP2,BP1,BP0(bit 7,5,4,3,2)����д!!!
void W25QXX::Write_SR(uint8_t sr) const
{
    this->CSPin.set_output(LOW);         //ʹ������
    this->spix->ReadWriteDATA(W25X_WriteStatusReg);//����дȡ״̬�Ĵ�������
    this->spix->ReadWriteDATA(sr);       //д��һ���ֽ�
    this->CSPin.set_output(HIGH);       //ȡ��Ƭѡ
}
//W25QXXдʹ��
//��WEL��λ
void W25QXX::Write_Enable() const
{
    this->CSPin.set_output(LOW);         //ʹ������
    this->spix->ReadWriteDATA(W25X_WriteEnable);//����дʹ��
    this->CSPin.set_output(HIGH);        //ȡ��Ƭѡ
}
//W25QXXд��ֹ
//��WEL����
void W25QXX::Write_Disable() const
{
    this->CSPin.set_output(LOW);         //ʹ������
    this->spix->ReadWriteDATA(W25X_WriteDisable);//����д��ָֹ��
    this->CSPin.set_output(HIGH);        //ȡ��Ƭѡ
}

//�ȴ�����
void W25QXX::Wait_Busy() const
{
    uint16_t wait_times_t=this->wait_times;
    while(wait_times_t){// �ȴ�BUSYλ���
        if((this->ReadSR()&0x01)==0x01)
            break;
        wait_times_t-=10;
        if(wait_times_t<10)
            break;
        delay_ms(10);
    }
}
//��������оƬ
//�ȴ�ʱ�䳬��...
void W25QXX::Erase_Chip() const
{
    this->Write_Enable();                 //����дʹ��
    this->Wait_Busy();
    this->CSPin.set_output(LOW);    //ʹ������
    this->spix->ReadWriteDATA(W25X_ChipErase);//����Ƭ��������
    this->CSPin.set_output(HIGH);   //ȡ��Ƭѡ
    this->Wait_Busy();   				  //�ȴ�оƬ��������
}
//����һ������
//Dst_Addr:������ַ ����ʵ����������
//����һ��ɽ��������ʱ��:150ms
void W25QXX::Erase_Sector(uint32_t Dst_Addr) const
{
    Dst_Addr*=4096;
    this->Write_Enable();                  //SET WEL
    this->Wait_Busy();
    this->CSPin.set_output(LOW);    //ʹ������
    this->spix->ReadWriteDATA(W25X_SectorErase);      //������������ָ��
    this->spix->ReadWriteDATA((uint8_t)((Dst_Addr)>>16));  //����24bit��ַ
    this->spix->ReadWriteDATA((uint8_t)((Dst_Addr)>>8));
    this->spix->ReadWriteDATA((uint8_t)Dst_Addr);
    this->CSPin.set_output(HIGH);   //ȡ��Ƭѡ
    this->Wait_Busy();   				  //�ȴ��������
}
//�������ģʽ
void W25QXX::PowerDown() const
{
    this->CSPin.set_output(LOW);    //ʹ������
    this->spix->ReadWriteDATA(W25X_PowerDown);        //���͵�������
    this->CSPin.set_output(HIGH);   //ȡ��Ƭѡ
    delay_us(3);                     //�ȴ�TPD
}
//����
void W25QXX::WAKEUP() const
{
    this->CSPin.set_output(LOW);    //ʹ������
    this->spix->ReadWriteDATA(W25X_ReleasePowerDown);   //  send W25X_PowerDown command 0xAB
    this->CSPin.set_output(HIGH);   //ȡ��Ƭѡ
    delay_us(3);                     //�ȴ�TRES1
}

uint16_t W25QXX::GetID() const {
    return this->W25QXX_TYPE;
}
//SPI��һҳ(0~65535)��д������256���ֽڵ�����
//��ָ����ַ��ʼд�����256�ֽڵ�����
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���256),������Ӧ�ó�����ҳ��ʣ���ֽ���!!!
void W25QXX::Write_Page(uint32_t Addr, uint8_t* pBuffer, uint16_t NumByte) const
{
    this->Write_Enable();                  //SET WEL
    this->CSPin.set_output(LOW);     //ʹ������
    this->spix->ReadWriteDATA(W25X_PageProgram);//����дҳ����
    this->spix->ReadWriteDATA((uint8_t)((Addr)>>16)); //����24bit��ַ
    this->spix->ReadWriteDATA((uint8_t)((Addr)>>8));
    this->spix->ReadWriteDATA((uint8_t)Addr);
    for(uint16_t ii=0;ii<NumByte;ii++)
        this->spix->ReadWriteDATA(pBuffer[ii]);//ѭ��д��
    this->CSPin.set_output(HIGH);    //ȡ��Ƭѡ
    this->Wait_Busy();					   //�ȴ�д�����
}
//�޼���дSPI FLASH
//����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0XFF,�����ڷ�0XFF��д������ݽ�ʧ��!
//�����Զ���ҳ����
//��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
//CHECK OK
void W25QXX::Write_NoCheck(uint32_t Addr,uint8_t* pBuffer,uint16_t NumByte) const
{
    uint16_t pageremain=256-Addr%256; //��ҳʣ����ֽ���
    if(NumByte<=pageremain)pageremain=NumByte;//������256���ֽ�
    while(true)
    {
        this->Write_Page(Addr,pBuffer,pageremain);
        if(NumByte==pageremain)break;//д�������
        else //NumByteToWrite>pageremain
        {
            pBuffer+=pageremain;
            Addr+=pageremain;

            NumByte-=pageremain;		   //��ȥ�Ѿ�д���˵��ֽ���
            if(NumByte>256)pageremain=256; //һ�ο���д��256���ֽ�
            else pageremain=NumByte; 	   //����256���ֽ���
        }
    }
}
//дSPI FLASH
//��ָ����ַ��ʼд��ָ�����ȵ�����
//�ú�������������!
//pBuffer:���ݴ洢��
//WriteAddr:��ʼд��ĵ�ַ(24bit)
//NumByteToWrite:Ҫд����ֽ���(���65535)
uint16_t W25QXX::write(uint32_t Addr, uint8_t *pBuffer, uint16_t NumByte) {
    uint32_t secpos;
    uint16_t secoff;
    uint16_t secremain;
    uint16_t i;
    secpos=Addr/4096;//������ַ
    secoff=Addr%4096;//�������ڵ�ƫ��
    secremain=4096-secoff;//����ʣ��ռ��С
    if(NumByte<=secremain)secremain=NumByte;//������4096���ֽ�
    while(true)
    {
        //this->read(secpos*4096,(uint8_t*)this->get_data.data(),4096);//������������������
        this->readstr(secpos*4096,&this->get_data,4096);
        for(i=0;i<secremain;i++)//У������
        {
            if(this->get_data[secoff+i]!=0XFF)break;//��Ҫ����
        }
        if(i<secremain)//��Ҫ����
        {
            this->Erase_Sector(secpos);//�����������
            for(i=0;i<secremain;i++)	   //����
            {
                this->get_data[i+secoff]=pBuffer[i];
            }
            this->Write_NoCheck(secpos*4096,(uint8_t*)this->get_data.data(),4096);//д����������

        }else this->Write_NoCheck(Addr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������.
        if(NumByte==secremain)break;//д�������
        else//д��δ����
        {
            secpos++;//������ַ��1
            secoff=0;//ƫ��λ��Ϊ0

            pBuffer+=secremain;  //ָ��ƫ��
            Addr+=secremain;//д��ַƫ��
            NumByte-=secremain;				//�ֽ����ݼ�
            if(NumByte>4096)secremain=4096;	//��һ����������д����
            else secremain=NumByte;			//��һ����������д����
        }
    }
    return NumByte;
}

uint16_t W25QXX::write(uint32_t addr, uint8_t data) {
    return this->write(addr,&data,1);
}
//��ȡSPI FLASH
//��ָ����ַ��ʼ��ȡָ�����ȵ�����
//pBuffer:���ݴ洢��
//ReadAddr:��ʼ��ȡ�ĵ�ַ(24bit)
//NumByteToRead:Ҫ��ȡ���ֽ���(���65535)
void W25QXX::read(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte) {
    this->CSPin.set_output(LOW);     //ʹ������
    this->spix->ReadWriteDATA(W25X_ReadData);     //���Ͷ�ȡ����
    this->spix->ReadWriteDATA((u8)((Addr)>>16));  //����24bit��ַ
    this->spix->ReadWriteDATA((u8)((Addr)>>8));
    this->spix->ReadWriteDATA((u8)Addr);
    for(uint16_t i=0;i<NumByte;i++)
    {
        pBuffer[i]=this->spix->ReadWriteDATA(0XFF);//ѭ������
    }
    this->CSPin.set_output(HIGH);     //ȡ��Ƭѡ
}

uint8_t W25QXX::read(uint32_t addr) {
    uint8_t data_t;
    this->read(addr,&data_t,1);
    return data_t;
}

void W25QXX::read(uint32_t addr, uint8_t *data) {
    this->read(addr,data,1);
}



