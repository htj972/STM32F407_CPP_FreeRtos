/**
* @Author kokirika
* @Name STM_FLASH
* @Date 2022-12-23
**/

#include "STM_FLASH.h"

//FLASH ��������ʼ��ַ
#define ADDR_FLASH_SECTOR_0     ((u32)0x08000000) 	//����0��ʼ��ַ, 16 Kbytes
#define ADDR_FLASH_SECTOR_1     ((u32)0x08004000) 	//����1��ʼ��ַ, 16 Kbytes
#define ADDR_FLASH_SECTOR_2     ((u32)0x08008000) 	//����2��ʼ��ַ, 16 Kbytes
#define ADDR_FLASH_SECTOR_3     ((u32)0x0800C000) 	//����3��ʼ��ַ, 16 Kbytes
#define ADDR_FLASH_SECTOR_4     ((u32)0x08010000) 	//����4��ʼ��ַ, 64 Kbytes
#define ADDR_FLASH_SECTOR_5     ((u32)0x08020000) 	//����5��ʼ��ַ, 128 Kbytes
#define ADDR_FLASH_SECTOR_6     ((u32)0x08040000) 	//����6��ʼ��ַ, 128 Kbytes
#define ADDR_FLASH_SECTOR_7     ((u32)0x08060000) 	//����7��ʼ��ַ, 128 Kbytes
#define ADDR_FLASH_SECTOR_8     ((u32)0x08080000) 	//����8��ʼ��ַ, 128 Kbytes
#define ADDR_FLASH_SECTOR_9     ((u32)0x080A0000) 	//����9��ʼ��ַ, 128 Kbytes
#define ADDR_FLASH_SECTOR_10    ((u32)0x080C0000) 	//����10��ʼ��ַ,128 Kbytes
#define ADDR_FLASH_SECTOR_11    ((u32)0x080E0000) 	//����11��ʼ��ַ,128 Kbytes

/*!
 * ��ȡָ����ַ�İ���(16λ����)
 * @param faddr:����ַ
 * @return ��Ӧ����
 */
uint32_t STM_FLASH::ReadWord(uint32_t faddr)
{
    return *(volatile uint32_t *)faddr;
}
/*!
 * ��ȡĳ����ַ���ڵ�flash����
 * @param faddr:����ַ
 * @return 0~11,��addr���ڵ�����
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
 * ��ָ����ַ��ʼд��ָ�����ȵ�����
 * @remark ��ΪSTM32F4������ʵ��̫��,û�취���ر�����������,���Ա�����
 *         д��ַ�����0XFF,��ô���Ȳ������������Ҳ�������������.����
 *         д��0XFF�ĵ�ַ,�����������������ݶ�ʧ.����д֮ǰȷ��������
 *         û����Ҫ����,��������������Ȳ�����,Ȼ����������д
 * @param WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ4�ı���!!)
 * @param pBuffer:����ָ��
 * @param NumToWrite:��(32λ)��(����Ҫд���32λ���ݵĸ���)
 * @return
 */
void STM_FLASH::Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite)
{
    FLASH_Status status = FLASH_COMPLETE;
    uint32_t addrx=0;
    uint32_t endaddr=0;
    if(WriteAddr<STM32_FLASH_BASE||WriteAddr%4)return;	//�Ƿ���ַ
    FLASH_Unlock();									//����
    FLASH_DataCacheCmd(DISABLE);//FLASH�����ڼ�,�����ֹ���ݻ���

    addrx=WriteAddr;				//д�����ʼ��ַ
    endaddr=WriteAddr+NumToWrite*4;	//д��Ľ�����ַ
    if(addrx<0X1FFF0000)			//ֻ�����洢��,����Ҫִ�в�������!!
    {
        while(addrx<endaddr)		//ɨ��һ���ϰ�.(�Է�FFFFFFFF�ĵط�,�Ȳ���)
        {
            if(ReadWord(addrx)!=0XFFFFFFFF)//�з�0XFFFFFFFF�ĵط�,Ҫ�����������
            {
                status=FLASH_EraseSector(GetFlashSector(addrx),VoltageRange_3);//VCC=2.7~3.6V֮��!!
                if(status!=FLASH_COMPLETE)break;	//����������
            }else addrx+=4;
        }
    }
    if(status==FLASH_COMPLETE)
    {
        while(WriteAddr<endaddr)//д����
        {
            if(FLASH_ProgramWord(WriteAddr,*pBuffer)!=FLASH_COMPLETE)//д������
            {
                break;	//д���쳣
            }
            WriteAddr+=4;
            pBuffer++;
        }
    }
    FLASH_DataCacheCmd(ENABLE);	//FLASH��������,�������ݻ���
    FLASH_Lock();//����
}
/*!
 * ��ָ����ַ��ʼд��ָ�����ȵ�����
 * @param ReadAddr:��ʼ��ַ
 * @param pBuffer:����ָ��
 * @param NumToRead:��(4λ)��
 * @return
 */
void STM_FLASH::Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead)
{
    uint32_t i;
    for(i=0;i<NumToRead;i++)
    {
        pBuffer[i]=ReadWord(ReadAddr);//��ȡ4���ֽ�.
        ReadAddr+=4;//ƫ��4���ֽ�.
    }
}
