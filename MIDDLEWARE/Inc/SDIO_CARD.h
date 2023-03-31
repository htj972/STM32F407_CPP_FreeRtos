/**
* @Author kokirika
* @Name SDIO_CARD
* @Date 2023-03-31
**/

#ifndef KOKIRIKA_SDIO_CARD_H
#define KOKIRIKA_SDIO_CARD_H

#include "sys.h"
#include "SDIO_CARD_ENUM.h"
#include "HARD_BASE.h"
#include "Storage_Link.h"
#include "Storage_BASE.h"

class SDIO_CARD: private Call_Back,public Storage_BASE{
private:
    SDIO_InitTypeDef SDIO_InitStructure{};
    SDIO_CmdInitTypeDef SDIO_CmdInitStructure{};
    SDIO_DataInitTypeDef SDIO_DataInitStructure{};

    static uint8_t CardType;		//SD�����ͣ�Ĭ��Ϊ1.x����
    static uint32_t CSD_Tab[4],CID_Tab[4],RCA;					//SD��CSD,CID�Լ���Ե�ַ(RCA)����
    static uint8_t DeviceMode;		   				//����ģʽ,ע��,����ģʽ����ͨ��SetDeviceMode,�������.����ֻ�Ƕ���һ��Ĭ�ϵ�ģʽ(DMA_MODE)
    static uint8_t StopCondition; 								//�Ƿ���ֹͣ�����־λ,DMA����д��ʱ���õ�
    static SD_Error TransferError;					//���ݴ�������־,DMA��дʱʹ��
    static uint8_t TransferEnd;								//���������־,DMA��дʱʹ��

    static SD_Error CmdError();
    static SD_Error CmdResp7Error();
    static SD_Error CmdResp1Error(uint8_t cmd);
    static SD_Error CmdResp3Error();
    static SD_Error CmdResp2Error();
    static SD_Error CmdResp6Error(uint8_t cmd,u16*prca);
    SD_Error SDEnWideBus(uint8_t enx);
    SD_Error IsCardProgramming(uint8_t *pstatus);
    SD_Error FindSCR(u16 rca,uint32_t *pscr);
    static void SDIO_Register_Deinit();

    static uint8_t convert_from_bytes_to_power_of_two(u16 NumberOfBytes);
    static void  SDIO_IRQHandler_init(Call_Back *event);
protected:

public:
    typedef struct CardInfo
    {
        SD_CSD SD_csd;
        SD_CID SD_cid;
        long long CardCapacity{};  	//SD������,��λ:�ֽ�,���֧��2^64�ֽڴ�С�Ŀ�.
        uint32_t CardBlockSize{}; 		//SD�����С
        uint16_t RCA{};					//����Ե�ַ
        uint8_t CardType{};				//������
    } CardInfo;
    CardInfo SDCardInfo{};
    SD_Error init();
    static void SDIO_Clock_Set(uint8_t clkdiv);

    SD_Error PowerON();
    static SD_Error PowerOFF();
    SD_Error InitializeCards();
    static SD_Error GetCardInfo(CardInfo *cardinfo);
    SD_Error EnableWideBusOperation(uint32_t wmode);
    static SD_Error SetDeviceMode(uint32_t mode);
    SD_Error SelectDeselect(uint32_t addr);
    SD_Error SendStatus(uint32_t *pcardstatus);
    SDCardState GetState();
    SD_Error ReadBlock(uint8_t *buf,long long addr,uint16_t blksize);
    SD_Error ReadMultiBlocks(uint8_t *buf,long long  addr,uint16_t blksize,uint32_t nblks);
    SD_Error WriteBlock(uint8_t *buf,long long addr,  uint16_t blksize);
    SD_Error WriteMultiBlocks(uint8_t *buf,long long addr,uint16_t blksize,uint32_t nblks);
    SD_Error ProcessIRQSrc();

    static void DMA_Config(const uint32_t*mbuf,uint32_t bufsize,uint32_t dir);
//void DMA_Config(uint32_t*mbuf,uint32_t bufsize,uint8_t dir);

    uint8_t ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt); 	//��SD��,fatfs/usb����
    uint8_t WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);	//дSD��,fatfs/usb����

    void USER_init();

    bool FAT_init() override;
    uint32_t GetSectorCount() override;
    uint16_t write(uint32_t addr ,uint8_t data) override;
    uint16_t write(uint32_t addr ,uint8_t *data,uint16_t len) override;

    uint8_t  read(uint32_t addr) override;
    void     read(uint32_t addr,uint8_t *data) override;
    void     read(uint32_t addr,uint8_t *data,uint16_t len) override;

    void Callback(int ,char **) override;
};



//�û�������
//SDIOʱ�Ӽ��㹫ʽ:SDIO_CKʱ��=SDIOCLK/[clkdiv+2];����,SDIOCLK�̶�Ϊ48Mhz
//ʹ��DMAģʽ��ʱ��,�������ʿ��Ե�48Mhz(bypass onʱ),���������Ŀ����Ǹ���
//��,����Ҳ�����,������뽵��ʱ��
#define SDIO_INIT_CLK_DIV        0x76 		//SDIO��ʼ��Ƶ�ʣ����400Kh
#define SDIO_TRANSFER_CLK_DIV    0x00		//SDIO����Ƶ��,��ֵ̫С���ܻᵼ�¶�д�ļ�����


////////////////////////////////////////////////////////////////////////////////////////////////////
//SDIO����ģʽ����,ͨ��SetDeviceMode��������.
#define SD_POLLING_MODE    	0  	//��ѯģʽ,��ģʽ��,�����д������,��������SDIO_TRANSFER_CLK_DIV������.
#define SD_DMA_MODE    		1	//DMAģʽ,��ģʽ��,�����д������,��������SDIO_TRANSFER_CLK_DIV������.


#endif //KOKIRIKA_SDIO_CARD_H
