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

    static uint8_t CardType;		//SD卡类型（默认为1.x卡）
    static uint32_t CSD_Tab[4],CID_Tab[4],RCA;					//SD卡CSD,CID以及相对地址(RCA)数据
    static uint8_t DeviceMode;		   				//工作模式,注意,工作模式必须通过SetDeviceMode,后才算数.这里只是定义一个默认的模式(DMA_MODE)
    static uint8_t StopCondition; 								//是否发送停止传输标志位,DMA多块读写的时候用到
    static SD_Error TransferError;					//数据传输错误标志,DMA读写时使用
    static uint8_t TransferEnd;								//传输结束标志,DMA读写时使用

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
        long long CardCapacity{};  	//SD卡容量,单位:字节,最大支持2^64字节大小的卡.
        uint32_t CardBlockSize{}; 		//SD卡块大小
        uint16_t RCA{};					//卡相对地址
        uint8_t CardType{};				//卡类型
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

    uint8_t ReadDisk(uint8_t*buf,uint32_t sector,uint8_t cnt); 	//读SD卡,fatfs/usb调用
    uint8_t WriteDisk(uint8_t*buf,uint32_t sector,uint8_t cnt);	//写SD卡,fatfs/usb调用

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



//用户配置区
//SDIO时钟计算公式:SDIO_CK时钟=SDIOCLK/[clkdiv+2];其中,SDIOCLK固定为48Mhz
//使用DMA模式的时候,传输速率可以到48Mhz(bypass on时),不过如果你的卡不是高速
//卡,可能也会出错,出错就请降低时钟
#define SDIO_INIT_CLK_DIV        0x76 		//SDIO初始化频率，最大400Kh
#define SDIO_TRANSFER_CLK_DIV    0x00		//SDIO传输频率,该值太小可能会导致读写文件出错


////////////////////////////////////////////////////////////////////////////////////////////////////
//SDIO工作模式定义,通过SetDeviceMode函数设置.
#define SD_POLLING_MODE    	0  	//查询模式,该模式下,如果读写有问题,建议增大SDIO_TRANSFER_CLK_DIV的设置.
#define SD_DMA_MODE    		1	//DMA模式,该模式下,如果读写有问题,建议增大SDIO_TRANSFER_CLK_DIV的设置.


#endif //KOKIRIKA_SDIO_CARD_H
