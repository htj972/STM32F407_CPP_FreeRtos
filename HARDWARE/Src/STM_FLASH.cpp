/**
* @Author kokirika
* @Name STM_FLASH
* @Date 2022-12-23
**/

#include "STM_FLASH.h"

//FLASH 扇区的起始地址
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//扇区0起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//扇区1起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//扇区2起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//扇区3起始地址, 16 Kbytes
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//扇区4起始地址, 64 Kbytes
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//扇区5起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//扇区6起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//扇区7起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//扇区8起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//扇区9起始地址, 128 Kbytes
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//扇区10起始地址,128 Kbytes
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//扇区11起始地址,128 Kbytes

/*!
 * 读取指定地址的半字(16位数据)
 * @param faddr:读地址
 * @return 对应数据
 */
uint32_t STM_FLASH::ReadWord(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}
/*!
 * 获取某个地址所在的flash扇区
 * @param faddr:读地址
 * @return 0~11,即addr所在的扇区
 */
uint16_t STM_FLASH::GetFlashSector(uint32_t addr)
{
    if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
    else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
    else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
    else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
    else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
    else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
    else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
    else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
    else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
    else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
    else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10;
    return FLASH_Sector_11;
}
/*!
 * 从指定地址开始写入指定长度的数据
 * @remark 因为STM32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数
 *         写地址如果非0XFF,那么会先擦除整个扇区且不保存扇区数据.所以
 *         写非0XFF的地址,将导致整个扇区数据丢失.建议写之前确保扇区里
 *         没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写
 * @param WriteAddr:起始地址(此地址必须为4的倍数!!)
 * @param pBuffer:数据指针
 * @param NumToWrite:字(32位)数(就是要写入的32位数据的个数)
 * @return
 */
void STM_FLASH::Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t addrx=0;
    uint32_t endaddr=0;
    if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//非法地址
    FLASH_Unlock();									//解锁
    FLASH_DataCacheCmd(DISABLE);//FLASH擦除期间,必须禁止数据缓存

    addrx=WriteAddr;				//写入的起始地址
    endaddr=WriteAddr+NumToWrite*4;	//写入的结束地址
    if(addrx<0X1FFF0000)			//只有主存储区,才需要执行擦除操作!!
    {
        while(addrx<endaddr)		//扫清一切障碍.(对非FFFFFFFF的地方,先擦除)
        {
            if(ReadWord(addrx)!=0XFFFFFFFF)//有非0XFFFFFFFF的地方,要擦除这个扇区
            {
                status=FLASH_EraseSector(GetFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V之间!!
                if(status!=FLASH_COMPLETE)break;	//发生错误了
            }else addrx+=4;
        }
    }
    if(status==FLASH_COMPLETE)
    {
        while(WriteAddr<endaddr)//写数据
        {
            if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)//写入数据
            {
                break;	//写入异常
            }
            WriteAddr+=4;
            pBuffer++;
        }
    }
    FLASH_DataCacheCmd(ENABLE);	//FLASH擦除结束,开启数据缓存
    FLASH_Lock();//上锁
}
/*!
 * 从指定地址开始写入指定长度的数据
 * @param ReadAddr:起始地址
 * @param pBuffer:数据指针
 * @param NumToRead:字(4位)数
 * @return
 */
void STM_FLASH::Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)
{
    uint32_t i;
    for(i=0;i<NumToRead;i++)
    {
        pBuffer[i]=ReadWord(ReadAddr);//读取4个字节.
        ReadAddr+=4;//偏移4个字节.
    }
}
