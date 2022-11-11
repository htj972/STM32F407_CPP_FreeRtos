/**
* @Author kokirika
* @Name SD_SPI
* @Date 2022-11-05
**/

#include "SD_SPI.h"

// SD卡类型定义
#define SD_TYPE_ERR     0X00
#define SD_TYPE_MMC     0X01
#define SD_TYPE_V1      0X02
#define SD_TYPE_V2      0X04
#define SD_TYPE_V2HC    0X06
// SD卡指令表
#define CMD0    0       //卡复位
#define CMD1    1
#define CMD8    8       //命令8 ，SEND_IF_COND
#define CMD9    9       //命令9 ，读CSD数据
#define CMD10   10      //命令10，读CID数据
#define CMD12   12      //命令12，停止数据传输
#define CMD16   16      //命令16，设置SectorSize 应返回0x00
#define CMD17   17      //命令17，读sector
#define CMD18   18      //命令18，读Multi sector
#define CMD23   23      //命令23，设置多sector写入前预先擦除N个block
#define CMD24   24      //命令24，写sector
#define CMD25   25      //命令25，写Multi sector
#define CMD41   41      //命令41，应返回0x00
#define CMD55   55      //命令55，应返回0x01
#define CMD58   58      //命令58，读OCR信息
#define CMD59   59      //命令59，使能/禁止CRC，应返回0x00
//数据写入回应字意义
#define MSD_DATA_OK                0x05
#define MSD_DATA_CRC_ERROR         0x0B
#define MSD_DATA_WRITE_ERROR       0x0D
#define MSD_DATA_OTHER_ERROR       0xFF
//SD卡回应标记字
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
//SD卡初始化的时候,需要低速
void SD_SPI::SpeedLow()
{
    this->spix->SetSpeed(SPI_BaudRatePrescaler_256);//设置到低速模式
}
//SD卡正常工作的时候,可以高速了
void SD_SPI::SpeedHigh()
{
    this->spix->SetSpeed(SPI_BaudRatePrescaler_2);//设置到高速模式
}
//取消选择,释放SPI总线
void SD_SPI::DisSelect()
{
    this->CSPinx.set_output(HIGH);
    this->spix->ReadWriteDATA(0xff);
}
//选择sd卡,并且等待卡准备OK
bool SD_SPI::Select()
{
    this->CSPinx.set_output(LOW);
    if(this->WaitReady())return true;//等待成功
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
//等待SD卡回应
uint8_t SD_SPI::GetResponse(uint8_t Response)
{
    uint16_t Count=0xFFFF;//等待次数
    while ((this->spix->ReadWriteDATA(0XFF)!=Response)&&Count)Count--;//等待得到准确的回应
    if (Count==0)return MSD_RESPONSE_FAILURE;//得到回应失败
    else return MSD_RESPONSE_NO_ERROR;//正确回应
}
//从sd卡读取一个数据包的内容
//buf:数据缓存区
//len:要读取的数据长度.
bool SD_SPI::RecvData(uint8_t *buf,uint16_t len)
{
    if(this->GetResponse(0xFE)!=MSD_RESPONSE_NO_ERROR)return false;//等待SD卡发回数据起始令牌0xFE
    while(len--)//开始接收数据
    {
        *buf=this->spix->ReadWriteDATA(0xFF);
        buf++;
    }
    //下面是2个伪CRC（dummy CRC）
    this->spix->ReadWriteDATA(0xFF);
    this->spix->ReadWriteDATA(0xFF);
    return true;//读取成功
}
//向sd卡写入一个数据包的内容 512字节
//buf:数据缓存区
//cmd:指令
bool SD_SPI::SendBlock(uint8_t *buf,uint8_t cmd)
{
    uint16_t t;
    if(!this->WaitReady())return false;//等待准备失效
    this->spix->ReadWriteDATA(cmd);
    if(cmd!=0XFD)//不是结束指令
    {
        for(t=0;t<512;t++)this->spix->ReadWriteDATA(buf[t]);//提高速度,减少函数传参时间
        this->spix->ReadWriteDATA(0xFF);//忽略crc
        this->spix->ReadWriteDATA(0xFF);
        t=this->spix->ReadWriteDATA(0xFF);//接收响应
        if((t&0x1F)!=0x05)return false;//响应错误
    }
    return true;//写入成功
}
//向SD卡发送一个命令
//输入: u8 cmd   命令
//      u32 arg  命令参数
//      u8 crc   crc校验值
uint8_t SD_SPI::SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc)
{
    uint8_t r1;
    uint8_t Retry;
    this->DisSelect();//取消上次片选
    if(!this->Select())return 0XFF;//片选失效
    //发送
    this->spix->ReadWriteDATA(cmd | 0x40);//分别写入命令
    this->spix->ReadWriteDATA((uint8_t )(arg >> 24));
    this->spix->ReadWriteDATA((uint8_t )(arg >> 16));
    this->spix->ReadWriteDATA((uint8_t )(arg >> 8));
    this->spix->ReadWriteDATA((uint8_t )(arg >> 0));
    this->spix->ReadWriteDATA(crc);
    if(cmd==CMD12)this->spix->ReadWriteDATA(0xff);//Skip a stuff byte when stop reading
    //等待响应，或超时退出
    Retry=0X1F;
    do
    {
        r1=this->spix->ReadWriteDATA(0xFF);
    }while((r1&0X80) && Retry--);
    //返回状态值
    return r1;
}
//获取SD卡的CID信息，包括制造商信息
//输入: u8 *cid_data(存放CID的内存，至少16Byte）
bool SD_SPI::GetCID(uint8_t *cid_data)
{
    bool r1;
    //发CMD10命令，读CID
    r1=this->SendCmd(CMD10,0,0x01);
    if(r1)
    {
        r1=this->RecvData(cid_data,16);//接收16个字节的数据
    }
    this->DisSelect();//取消片选
    return r1;
}
//获取SD卡的CSD信息，包括容量和速度信息
//输入:u8 *cid_data(存放CID的内存，至少16Byte）
bool SD_SPI::GetCSD(uint8_t *csd_data) {
    uint8_t r1;
    r1 = this->SendCmd(CMD9, 0, 0x01);//发CMD9命令，读CSD
    if (r1==0) {
        r1 = this->RecvData(csd_data, 16);//接收16个字节的数据
    }
    this->DisSelect();//取消片选
    if(r1>0)return true;
    else return false;
}
//获取SD卡的总扇区数（扇区数）
//返回值:0： 取容量出错
//       其他:SD卡的容量(扇区数/512字节)
//每扇区的字节数必为512，因为如果不是512，则初始化不能通过.
uint32_t SD_SPI::GetSectorCount()
{
    uint8_t csd[16];
    uint32_t Capacity;
    uint8_t n;
    uint16_t csize;
    //取CSD信息，如果期间出错，返回0
    if(!this->GetCSD(csd)) return 0;
    //如果为SDHC卡，按照下面方式计算
    if((csd[0]&0xC0)==0x40)	 //V2.00的卡
    {
        csize = csd[9] + ((u16)csd[8] << 8) + 1;
        Capacity = (u32)csize << 10;//得到扇区数
    }else//V1.XX的卡
    {
        n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
        csize = (csd[8] >> 6) + ((u16)csd[7] << 2) + ((u16)(csd[6] & 3) << 10) + 1;
        Capacity= (u32)csize << (n - 9);//得到扇区数
    }
    return Capacity;
}
//初始化SD卡
uint8_t SD_SPI::Initialize()
{
    uint8_t r1;      // 存放SD卡的返回值
    uint16_t retry;  // 用来进行超时计数
    uint8_t buf[4];
    uint16_t i;
    this->SpeedLow();	//设置到低速模式
    for(i=0;i<10;i++)this->spix->ReadWriteDATA(0XFF);//发送最少74个脉冲
    retry=20;
    do
    {
        r1=this->SendCmd(CMD0,0,0x95);//进入IDLE状态
    }while((r1!=0X01) && retry--);
    this->SD_Type=0;//默认无卡
    if(r1==0X01)
    {
        if(this->SendCmd(CMD8,0x1AA,0x87)==1)//SD V2.0
        {
            for(i=0;i<4;i++)buf[i]=this->spix->ReadWriteDATA(0XFF);	//Get trailing return value of R7 resp
            if(buf[2]==0X01&&buf[3]==0XAA)//卡是否支持2.7~3.6V
            {
                retry=0XFFFE;
                do
                {
                    this->SendCmd(CMD55,0,0X01);	//发送CMD55
                    r1=this->SendCmd(CMD41,0x40000000,0X01);//发送CMD41
                }while(r1&&retry--);
                if(retry&&this->SendCmd(CMD58,0,0X01)==0)//鉴别SD2.0卡版本开始
                {
                    for(i=0;i<4;i++)buf[i]=this->spix->ReadWriteDATA(0XFF);//得到OCR值
                    if(buf[0]&0x40)this->SD_Type=SD_TYPE_V2HC;    //检查CCS
                    else this->SD_Type=SD_TYPE_V2;
                }
            }
        }else//SD V1.x/ MMC	V3
        {
            this->SendCmd(CMD55,0,0X01);		//发送CMD55
            r1=this->SendCmd(CMD41,0,0X01);	//发送CMD41
            if(r1<=1)
            {
                this->SD_Type=SD_TYPE_V1;
                retry=0XFFFE;
                do //等待退出IDLE模式
                {
                    this->SendCmd(CMD55,0,0X01);	//发送CMD55
                    r1=this->SendCmd(CMD41,0,0X01);//发送CMD41
                }while(r1&&retry--);
            }else//MMC卡不支持CMD55+CMD41识别
            {
                this->SD_Type=SD_TYPE_MMC;//MMC V3
                retry=0XFFFE;
                do //等待退出IDLE模式
                {
                    r1=this->SendCmd(CMD1,0,0X01);//发送CMD1
                }while(r1&&retry--);
            }
            if(retry==0||this->SendCmd(CMD16,512,0X01)!=0)this->SD_Type=SD_TYPE_ERR;//错误的卡
        }
    }
    this->DisSelect();//取消片选
    this->SpeedHigh();//高速
    if(this->SD_Type)return 0;
    else if(r1)return r1;
    return 0xaa;//其他错误
}
//读SD卡
//buf:数据缓存区
//sector:扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
uint8_t SD_SPI::ReadDisk(uint8_t *buf,uint32_t sector,uint8_t cnt)
{
    uint8_t r1;
    if(this->SD_Type!=SD_TYPE_V2HC)sector =0;//转换为字节地址
    if(cnt==1)
    {
        r1=this->SendCmd(CMD17,sector,0X01);//读命令
        if(r1==0)//指令发送成功
        {
            r1=this->RecvData(buf,512);//接收512个字节
        }
    }else
    {
        this->SendCmd(CMD18,sector,0X01);//连续读命令
        do
        {
            r1=this->RecvData(buf,512);//接收512个字节
            buf+=512;
        }while(--cnt && r1==0);
        this->SendCmd(CMD12,0,0X01);	//发送停止命令
    }
    this->DisSelect();//取消片选
    return r1;//
}
//写SD卡
//buf:数据缓存区
//sector:起始扇区
//cnt:扇区数
//返回值:0,ok;其他,失败.
uint8_t SD_SPI::WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt)
{
    uint8_t r1;
    if(SD_Type!=SD_TYPE_V2HC)sector *= 512;//转换为字节地址
    if(cnt==1)
    {
        r1=this->SendCmd(CMD24,sector,0X01);//读命令
        if(r1==0)//指令发送成功
        {
            r1=this->SendBlock(buf,0xFE);//写512个字节
        }
    }else
    {
        if(SD_Type!=SD_TYPE_MMC)
        {
            this->SendCmd(CMD55,0,0X01);
            this->SendCmd(CMD23,cnt,0X01);//发送指令
        }
        r1=this->SendCmd(CMD25,sector,0X01);//连续读命令
        if(r1==0)
        {
            do
            {
                r1=this->SendBlock(buf,0xFC);//接收512个字节
                buf+=512;
            }while(--cnt && r1==0);
            r1=this->SendBlock(nullptr,0xFD);//接收512个字节
        }
    }
    this->DisSelect();//取消片选
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





