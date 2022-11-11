/**
* @Author kokirika
* @Name SD_SPI
* @Date 2022-11-05
**/

#include "SD_SPI.h"

// SD�����Ͷ���
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06
// SD��ָ���
#define CMD0    0       //����λ
#define CMD1    1
#define CMD8    8       //����8 ��SEND_IF_COND
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define CMD23   23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define CMD41   41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00
//����д���Ӧ������
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//SD����Ӧ�����
#define MSD_RESPONSE_NO_ERROR      0x00
#define MSD_IN_IDLE_STATE          0x01
#define MSD_ERASE_RESET            0x02
#define MSD_ILLEGAL_COMMAND        0x04
#define MSD_COM_CRC_ERROR          0x08
#define MSD_ERASE_SEQUENCE_ERROR   0x10
#define MSD_ADDRESS_ERROR          0x20
#define MSD_PARAMETER_ERROR        0x40
#define MSD_RESPONSE_FAILURE       0xFF


SD_SPI::SD_SPI(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx) {
    this->CSPinx.init(PORTx,Pinx,GPIO_Mode_OUT);
    this->spix=SPIx;
}

SD_SPI::SD_SPI(SPI *SPIx, uint8_t CSpin) {
    this->CSPinx.init(CSpin,GPIO_Mode_OUT);
    this->spix=SPIx;
}

uint8_t SD_SPI::init(SPI *SPIx,uint8_t CSpin)
{
    this->CSPinx.init(CSpin,GPIO_Mode_OUT);
    this->spix=SPIx;
    return this->Initialize();
}

uint8_t SD_SPI::init(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx) {
    this->CSPinx.init(PORTx,Pinx,GPIO_Mode_OUT);
    this->spix=SPIx;
    return this->Initialize();
}

uint8_t SD_SPI::init()
{
    if(this->spix!= nullptr) {
        this->DisSelect();
        return this->Initialize();
    }
    else return 0xaf;
}
//SD����ʼ����ʱ��,��Ҫ����
void SD_SPI::SpeedLow()
{
    this->spix->SetSpeed(SPI_BaudRatePrescaler_256);//���õ�����ģʽ
}
//SD������������ʱ��,���Ը�����
void SD_SPI::SpeedHigh()
{
    this->spix->SetSpeed(SPI_BaudRatePrescaler_2);//���õ�����ģʽ
}
//ȡ��ѡ��,�ͷ�SPI����
void SD_SPI::DisSelect()
{
    this->CSPinx.set_output(HIGH);
    this->spix->ReadWriteDATA(0xff);
}
//ѡ��sd��,���ҵȴ���׼��OK
bool SD_SPI::Select()
{
    this->CSPinx.set_output(LOW);
    if(this->WaitReady())return true;//�ȴ��ɹ�
    this->DisSelect();
    return false;
}

bool SD_SPI::WaitReady()
{
    uint32_t t=0;
    do
    {
        if(this->spix->ReadWriteDATA(0XFF)==0XFF)
            return true;
        t++;
    }while(t<0XFFFFFF);
    return false;
}
//�ȴ�SD����Ӧ
uint8_t SD_SPI::GetResponse(uint8_t Response)
{
    uint16_t Count=0xFFFF;//�ȴ�����
    while ((this->spix->ReadWriteDATA(0XFF)!=Response)&&Count)Count--;//�ȴ��õ�׼ȷ�Ļ�Ӧ
    if (Count==0)return MSD_RESPONSE_FAILURE;//�õ���Ӧʧ��
    else return MSD_RESPONSE_NO_ERROR;//��ȷ��Ӧ
}
//��sd����ȡһ�����ݰ�������
//buf:���ݻ�����
//len:Ҫ��ȡ�����ݳ���.
bool SD_SPI::RecvData(uint8_t *buf,uint16_t len)
{
    if(this->GetResponse(0xFE)!=MSD_RESPONSE_NO_ERROR)return false;//�ȴ�SD������������ʼ����0xFE
    while(len--)//��ʼ��������
    {
        *buf=this->spix->ReadWriteDATA(0xFF);
        buf++;
    }
    //������2��αCRC��dummy CRC��
    this->spix->ReadWriteDATA(0xFF);
    this->spix->ReadWriteDATA(0xFF);
    return true;//��ȡ�ɹ�
}
//��sd��д��һ�����ݰ������� 512�ֽ�
//buf:���ݻ�����
//cmd:ָ��
bool SD_SPI::SendBlock(uint8_t *buf,uint8_t cmd)
{
    uint16_t t;
    if(!this->WaitReady())return false;//�ȴ�׼��ʧЧ
    this->spix->ReadWriteDATA(cmd);
    if(cmd!=0XFD)//���ǽ���ָ��
    {
        for(t=0;t<512;t++)this->spix->ReadWriteDATA(buf[t]);//����ٶ�,���ٺ�������ʱ��
        this->spix->ReadWriteDATA(0xFF);//����crc
        this->spix->ReadWriteDATA(0xFF);
        t=this->spix->ReadWriteDATA(0xFF);//������Ӧ
        if((t&0x1F)!=0x05)return false;//��Ӧ����
    }
    return true;//д��ɹ�
}
//��SD������һ������
//����: u8 cmd   ����
//      u32 arg  �������
//      u8 crc   crcУ��ֵ
uint8_t SD_SPI::SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t r1;
    uint8_t Retry;
    this->DisSelect();//ȡ���ϴ�Ƭѡ
    if(!this->Select())return 0XFF;//ƬѡʧЧ
    //����
    this->spix->ReadWriteDATA(cmd | 0x40);//�ֱ�д������
    this->spix->ReadWriteDATA((uint8_t )(arg >> 24));
    this->spix->ReadWriteDATA((uint8_t )(arg >> 16));
    this->spix->ReadWriteDATA((uint8_t )(arg >> 8));
    this->spix->ReadWriteDATA((uint8_t )(arg >> 0));
    this->spix->ReadWriteDATA(crc);
    if(cmd==CMD12)this->spix->ReadWriteDATA(0xff);//Skip a stuff byte when stop reading
    //�ȴ���Ӧ����ʱ�˳�
    Retry=0X1F;
    do
    {
        r1=this->spix->ReadWriteDATA(0xFF);
    }while((r1&0X80) && Retry--);
    //����״ֵ̬
    return r1;
}
//��ȡSD����CID��Ϣ��������������Ϣ
//����: u8 *cid_data(���CID���ڴ棬����16Byte��
bool SD_SPI::GetCID(uint8_t *cid_data)
{
    bool r1;
    //��CMD10�����CID
    r1=this->SendCmd(CMD10,0,0x01);
    if(r1)
    {
        r1=this->RecvData(cid_data,16);//����16���ֽڵ�����
    }
    this->DisSelect();//ȡ��Ƭѡ
    return r1;
}
//��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
//����:u8 *cid_data(���CID���ڴ棬����16Byte��
bool SD_SPI::GetCSD(uint8_t *csd_data) {
    uint8_t r1;
    r1 = this->SendCmd(CMD9, 0, 0x01);//��CMD9�����CSD
    if (r1==0) {
        r1 = this->RecvData(csd_data, 16);//����16���ֽڵ�����
    }
    this->DisSelect();//ȡ��Ƭѡ
    if(r1>0)return true;
    else return false;
}
//��ȡSD����������������������
//����ֵ:0�� ȡ��������
//       ����:SD��������(������/512�ֽ�)
//ÿ�������ֽ�����Ϊ512����Ϊ�������512�����ʼ������ͨ��.
uint32_t SD_SPI::GetSectorCount()
{
    uint8_t csd[16];
    uint32_t Capacity;
    uint8_t n;
    uint16_t csize;
    //ȡCSD��Ϣ������ڼ��������0
    if(!this->GetCSD(csd)) return 0;
    //���ΪSDHC�����������淽ʽ����
    if((csd[0]&0xC0)==0x40)	 //V2.00�Ŀ�
    {
        csize = csd[9] + ((u16)csd[8] << 8) + 1;
        Capacity = (u32)csize << 10;//�õ�������
    }else//V1.XX�Ŀ�
    {
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
        Capacity= (u32)csize << (n - 9);//�õ�������
    }
    return Capacity;
}
//��ʼ��SD��
uint8_t SD_SPI::Initialize()
{
    uint8_t r1;      // ���SD���ķ���ֵ
    uint16_t retry;  // �������г�ʱ����
    uint8_t buf[4];
    uint16_t i;
    this->SpeedLow();	//���õ�����ģʽ
    for(i=0;i<10;i++)this->spix->ReadWriteDATA(0XFF);//��������74������
    retry=20;
    do
    {
        r1=this->SendCmd(CMD0,0,0x95);//����IDLE״̬
    }while((r1!=0X01) && retry--);
    this->SD_Type=0;//Ĭ���޿�
    if(r1==0X01)
    {
        if(this->SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
        {
            for(i=0;i<4;i++)buf[i]=this->spix->ReadWriteDATA(0XFF);	//Get trailing return value of R7 resp
            if(buf[2]==0X01&&buf[3]==0XAA)//���Ƿ�֧��2.7~3.6V
            {
                retry=0XFFFE;
                do
                {
                    this->SendCmd(CMD55,0,0X01);	//����CMD55
                    r1=this->SendCmd(CMD41,0x40000000,0X01);//����CMD41
                }while(r1&&retry--);
                if(retry&&this->SendCmd(CMD58,0,0X01)==0)//����SD2.0���汾��ʼ
                {
                    for(i=0;i<4;i++)buf[i]=this->spix->ReadWriteDATA(0XFF);//�õ�OCRֵ
                    if(buf[0]&0x40)this->SD_Type=SD_TYPE_V2HC;    //���CCS
                    else this->SD_Type=SD_TYPE_V2;
                }
            }
        }else//SD V1.x/ MMC	V3
        {
            this->SendCmd(CMD55,0,0X01);		//����CMD55
            r1=this->SendCmd(CMD41,0,0X01);	//����CMD41
            if(r1<=1)
            {
                this->SD_Type=SD_TYPE_V1;
                retry=0XFFFE;
                do //�ȴ��˳�IDLEģʽ
                {
                    this->SendCmd(CMD55,0,0X01);	//����CMD55
                    r1=this->SendCmd(CMD41,0,0X01);//����CMD41
                }while(r1&&retry--);
            }else//MMC����֧��CMD55+CMD41ʶ��
            {
                this->SD_Type=SD_TYPE_MMC;//MMC V3
                retry=0XFFFE;
                do //�ȴ��˳�IDLEģʽ
                {
                    r1=this->SendCmd(CMD1,0,0X01);//����CMD1
                }while(r1&&retry--);
            }
            if(retry==0||this->SendCmd(CMD16,512,0X01)!=0)this->SD_Type=SD_TYPE_ERR;//����Ŀ�
        }
    }
    this->DisSelect();//ȡ��Ƭѡ
    this->SpeedHigh();//����
    if(this->SD_Type)return 0;
    else if(r1)return r1;
    return 0xaa;//��������
}
//��SD��
//buf:���ݻ�����
//sector:����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
uint8_t SD_SPI::ReadDisk(uint8_t *buf,uint32_t sector,uint8_t cnt)
{
    uint8_t r1;
    if(this->SD_Type!=SD_TYPE_V2HC)sector =0;//ת��Ϊ�ֽڵ�ַ
    if(cnt==1)
    {
        r1=this->SendCmd(CMD17,sector,0X01);//������
        if(r1==0)//ָ��ͳɹ�
        {
            r1=this->RecvData(buf,512);//����512���ֽ�
        }
    }else
    {
        this->SendCmd(CMD18,sector,0X01);//����������
        do
        {
            r1=this->RecvData(buf,512);//����512���ֽ�
            buf+=512;
        }while(--cnt && r1==0);
        this->SendCmd(CMD12,0,0X01);	//����ֹͣ����
    }
    this->DisSelect();//ȡ��Ƭѡ
    return r1;//
}
//дSD��
//buf:���ݻ�����
//sector:��ʼ����
//cnt:������
//����ֵ:0,ok;����,ʧ��.
uint8_t SD_SPI::WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
    uint8_t r1;
    if(SD_Type!=SD_TYPE_V2HC)sector *= 512;//ת��Ϊ�ֽڵ�ַ
    if(cnt==1)
    {
        r1=this->SendCmd(CMD24,sector,0X01);//������
        if(r1==0)//ָ��ͳɹ�
        {
            r1=this->SendBlock(buf,0xFE);//д512���ֽ�
        }
    }else
    {
        if(SD_Type!=SD_TYPE_MMC)
        {
            this->SendCmd(CMD55,0,0X01);
            this->SendCmd(CMD23,cnt,0X01);//����ָ��
        }
        r1=this->SendCmd(CMD25,sector,0X01);//����������
        if(r1==0)
        {
            do
            {
                r1=this->SendBlock(buf,0xFC);//����512���ֽ�
                buf+=512;
            }while(--cnt && r1==0);
            r1=this->SendBlock(nullptr,0xFD);//����512���ֽ�
        }
    }
    this->DisSelect();//ȡ��Ƭѡ
    return r1;//
}

uint16_t SD_SPI::write(uint32_t addr, uint8_t data) {
    if(this->WriteDisk(&data,addr,1)==0)
        return 1;
    else return 0;
}

uint16_t SD_SPI::write(uint32_t addr, uint8_t *data, uint16_t len) {
    if(this->WriteDisk(data,addr,1)==0)
        return len;
    else return 0;
}

uint8_t SD_SPI::read(uint32_t addr) {
    uint8_t data;
    if(this->ReadDisk(&data,addr,1)==0)
        return data;
    else return 0;
}

void SD_SPI::read(uint32_t addr, uint8_t *data) {
    this->ReadDisk(data,addr,1);
}

void SD_SPI::read(uint32_t addr, uint8_t *data, uint16_t len) {
    this->ReadDisk(data,addr,len);
}

uint8_t SD_SPI::FAT_init() {
    return this->init();
}





