/**
* @Author kSD_OKirika
* @Name SDIO_CARD
* @Date 2023-03-31
**/

#include "SDIO_CARD.h"
#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////
//SDIO ָ�
#define SD_CMD_GO_IDLE_STATE                       ((uint8_t)0)
#define SD_CMD_SEND_OP_COND                        ((uint8_t)1)
#define SD_CMD_ALL_SEND_CID                        ((uint8_t)2)
#define SD_CMD_SET_REL_ADDR                        ((uint8_t)3) /*!< SDIO_SEND_REL_ADDR for SD Card */
#define SD_CMD_SET_DSR                             ((uint8_t)4)
#define SD_CMD_SDIO_SEN_OP_COND                    ((uint8_t)5)
#define SD_CMD_HS_SWITCH                           ((uint8_t)6)
#define SD_CMD_SEL_DESEL_CARD                      ((uint8_t)7)
#define SD_CMD_HS_SEND_EXT_CSD                     ((uint8_t)8)
#define SD_CMD_SEND_CSD                            ((uint8_t)9)
#define SD_CMD_SEND_CID                            ((uint8_t)10)
#define SD_CMD_READ_DAT_UNTIL_STOP                 ((uint8_t)11) /*!< SD Card doesn't support it */
#define SD_CMD_STOP_TRANSMISSION                   ((uint8_t)12)
#define SD_CMD_SEND_STATUS                         ((uint8_t)13)
#define SD_CMD_HS_BUSTEST_READ                     ((uint8_t)14)
#define SD_CMD_GO_INACTIVE_STATE                   ((uint8_t)15)
#define SD_CMD_SET_BLOCKLEN                        ((uint8_t)16)
#define SD_CMD_READ_SINGLE_BLOCK                   ((uint8_t)17)
#define SD_CMD_READ_MULT_BLOCK                     ((uint8_t)18)
#define SD_CMD_HS_BUSTEST_WRITE                    ((uint8_t)19)
#define SD_CMD_WRITE_DAT_UNTIL_STOP                ((uint8_t)20)
#define SD_CMD_SET_BLOCK_COUNT                     ((uint8_t)23)
#define SD_CMD_WRITE_SINGLE_BLOCK                  ((uint8_t)24)
#define SD_CMD_WRITE_MULT_BLOCK                    ((uint8_t)25)
#define SD_CMD_PROG_CID                            ((uint8_t)26)
#define SD_CMD_PROG_CSD                            ((uint8_t)27)
#define SD_CMD_SET_WRITE_PROT                      ((uint8_t)28)
#define SD_CMD_CLR_WRITE_PROT                      ((uint8_t)29)
#define SD_CMD_SEND_WRITE_PROT                     ((uint8_t)30)
#define SD_CMD_SD_ERASE_GRP_START                  ((uint8_t)32) /*!< To set the address of the first write
                                                                  block to be erased. (For SD card only) */
#define SD_CMD_SD_ERASE_GRP_END                    ((uint8_t)33) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For SD card only) */
#define SD_CMD_ERASE_GRP_START                     ((uint8_t)35) /*!< To set the address of the first write block to be erased.
                                                                  (For MMC card only spec 3.31) */

#define SD_CMD_ERASE_GRP_END                       ((uint8_t)36) /*!< To set the address of the last write block of the
                                                                  continuous range to be erased. (For MMC card only spec 3.31) */

#define SD_CMD_ERASE                               ((uint8_t)38)
#define SD_CMD_FAST_IO                             ((uint8_t)39) /*!< SD Card doesn't support it */
#define SD_CMD_GO_IRQ_STATE                        ((uint8_t)40) /*!< SD Card doesn't support it */
#define SD_CMD_LOCK_UNLOCK                         ((uint8_t)42)
#define SD_CMD_APP_CMD                             ((uint8_t)55)
#define SD_CMD_GEN_CMD                             ((uint8_t)56)
#define SD_CMD_NO_CMD                              ((uint8_t)64)

/**
  * @brief Following commands are SD Card Specific commands.
  *        SDIO_APP_CMD ��CMD55 should be sent before sending these commands.
  */
#define SD_CMD_APP_SD_SET_BUSWIDTH                 ((uint8_t)6)  /*!< For SD Card only */
#define SD_CMD_SD_APP_STAUS                        ((uint8_t)13) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_NUM_WRITE_BLOCKS        ((uint8_t)22) /*!< For SD Card only */
#define SD_CMD_SD_APP_OP_COND                      ((uint8_t)41) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CLR_CARD_DETECT          ((uint8_t)42) /*!< For SD Card only */
#define SD_CMD_SD_APP_SEND_SCR                     ((uint8_t)51) /*!< For SD Card only */
#define SD_CMD_SDIO_RW_DIRECT                      ((uint8_t)52) /*!< For SD I/O Card only */
#define SD_CMD_SDIO_RW_EXTENDED                    ((uint8_t)53) /*!< For SD I/O Card only */

/**
  * @brief Following commands are SD Card Specific security commands.
  *        SDIO_APP_CMD should be sent before sending these commands.
  */
#define SD_CMD_SD_APP_GET_MKB                      ((uint8_t)43) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_MID                      ((uint8_t)44) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RN1                  ((uint8_t)45) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RN2                  ((uint8_t)46) /*!< For SD Card only */
#define SD_CMD_SD_APP_SET_CER_RES2                 ((uint8_t)47) /*!< For SD Card only */
#define SD_CMD_SD_APP_GET_CER_RES1                 ((uint8_t)48) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_READ_MULTIPLE_BLOCK   ((uint8_t)18) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MULTIPLE_BLOCK  ((uint8_t)25) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_ERASE                 ((uint8_t)38) /*!< For SD Card only */
#define SD_CMD_SD_APP_CHANGE_SECURE_AREA           ((uint8_t)49) /*!< For SD Card only */
#define SD_CMD_SD_APP_SECURE_WRITE_MKB             ((uint8_t)48) /*!< For SD Card only */

//֧�ֵ�SD������
#define SDIO_STD_CAPACITY_SD_CARD_V1_1             ((uint32_t)0x00000000)
#define SDIO_STD_CAPACITY_SD_CARD_V2_0             ((uint32_t)0x00000001)
#define SDIO_HIGH_CAPACITY_SD_CARD                 ((uint32_t)0x00000002)
#define SDIO_MULTIMEDIA_CARD                       ((uint32_t)0x00000003)
#define SDIO_SECURE_DIGITAL_IO_CARD                ((uint32_t)0x00000004)
#define SDIO_HIGH_SPEED_MULTIMEDIA_CARD            ((uint32_t)0x00000005)
#define SDIO_SECURE_DIGITAL_IO_COMBO_CARD          ((uint32_t)0x00000006)
#define SDIO_HIGH_CAPACITY_MMC_CARD                ((uint32_t)0x00000007)

//SDIO��ز�������
//#define NULL 0
#define SDIO_STATIC_FLAGS               ((uint32_t)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((uint32_t)0x00010000)
#define SDIO_DATATIMEOUT                ((uint32_t)0xFFFFFFFF)
#define SDIO_FIFO_Address               ((uint32_t)0x40018080)

//Mask for errors Card Status R1 (OCR Register)
#define SD_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR                 ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET              ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS                ((uint32_t)0xFDFFE008)

//Masks for R6 Response
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((uint32_t)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((uint32_t)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY                ((uint32_t)0x40000000)
#define SD_STD_CAPACITY                 ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN                ((uint32_t)0x000001AA)
#define SD_VOLTAGE_WINDOW_MMC           ((uint32_t)0x80FF8000)

#define SD_MAX_VOLT_TRIAL               ((uint32_t)0x0000FFFF)
#define SD_ALLZERO                      ((uint32_t)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((uint32_t)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((uint32_t)0x00010000)
#define SD_CARD_LOCKED                  ((uint32_t)0x02000000)
#define SD_CARD_PROGRAMMING             ((uint32_t)0x00000007)
#define SD_CARD_RECEIVING               ((uint32_t)0x00000006)
#define SD_DATATIMEOUT                  ((uint32_t)0xFFFFFFFF)
#define SD_0TO7BITS                     ((uint32_t)0x000000FF)
#define SD_8TO15BITS                    ((uint32_t)0x0000FF00)
#define SD_16TO23BITS                   ((uint32_t)0x00FF0000)
#define SD_24TO31BITS                   ((uint32_t)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFF)

#define SD_HALFFIFO                     ((uint32_t)0x00000008)
#define SD_HALFFIFOBYTES                ((uint32_t)0x00000020)

//Command Class Supported
#define SD_CCCC_LOCK_UNLOCK             ((uint32_t)0x00000080)
#define SD_CCCC_WRITE_PROT              ((uint32_t)0x00000040)
#define SD_CCCC_ERASE                   ((uint32_t)0x00000020)

//CMD8ָ��
#define SDIO_SEND_IF_COND               ((uint32_t)0x00000008)


uint8_t SDIO_CARD::CardType=SDIO_STD_CAPACITY_SD_CARD_V1_1;		//SD�����ͣ�Ĭ��Ϊ1.x����
uint32_t SDIO_CARD::CSD_Tab[4];
uint32_t SDIO_CARD::CID_Tab[4];
uint32_t SDIO_CARD::RCA=0;					//SD��CSD,CID�Լ���Ե�ַ(RCA)����
uint8_t SDIO_CARD::DeviceMode=SD_DMA_MODE;		   				//����ģʽ,ע��,����ģʽ����ͨ��SD_SetDeviceMode,�������.����ֻ�Ƕ���һ��Ĭ�ϵ�ģʽ(SD_DMA_MODE)
uint8_t SDIO_CARD::StopCondition=0; 								//�Ƿ���ֹͣ�����־λ,DMA����д��ʱ���õ�
SD_Error SDIO_CARD::TransferError=SD_OK;					//���ݴ�������־,DMA��дʱʹ��
uint8_t SDIO_CARD::TransferEnd=0;								//���������־,DMA��дʱʹ��


//SD_ReadDisk/SD_WriteDisk����ר��buf,�����������������ݻ�������ַ����4�ֽڶ����ʱ��,
//��Ҫ�õ�������,ȷ�����ݻ�������ַ��4�ֽڶ����.
__attribute__((aligned(4)))  uint8_t SDIO_DATA_BUFFER[512];


void SDIO_CARD::SDIO_Register_Deinit()
{
    SDIO->POWER=0x00000000;
    SDIO->CLKCR=0x00000000;
    SDIO->ARG=0x00000000;
    SDIO->CMD=0x00000000;
    SDIO->DTIMER=0x00000000;
    SDIO->DLEN=0x00000000;
    SDIO->DCTRL=0x00000000;
    SDIO->ICR=0x00C007FF;
    SDIO->MASK=0x00000000;
}

//��ʼ��SD��
//����ֵ:�������;(0,�޴���)
SD_Error SDIO_CARD::init()
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    SD_Error errorstatus;
    uint8_t clkdiv;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_DMA2, ENABLE);//ʹ��GPIOC,GPIOD DMA2ʱ��

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SDIO, ENABLE);//SDIOʱ��ʹ��

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, ENABLE);//SDIO��λ


    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12; 	//PC8,9,10,11,12���ù������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100M
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOC, &GPIO_InitStructure);// PC8,9,10,11,12���ù������


    GPIO_InitStructure.GPIO_Pin =GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);//PD2���ù������

    //���Ÿ���ӳ������
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_SDIO); //PC8,AF12
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_SDIO);
    GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_SDIO);

    RCC_APB2PeriphResetCmd(RCC_APB2Periph_SDIO, DISABLE);//SDIO������λ

    //SDIO����Ĵ�������ΪĬ��ֵ
    SDIO_Register_Deinit();

    NVIC_InitStructure.NVIC_IRQChannel = SDIO_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����

    errorstatus=PowerON();			//SD���ϵ�
    if(errorstatus==SD_OK)errorstatus=InitializeCards();			//��ʼ��SD��
    if(errorstatus==SD_OK)errorstatus=GetCardInfo(&SDCardInfo);	//��ȡ����Ϣ
    if(errorstatus==SD_OK)errorstatus=SelectDeselect((uint32_t)(SDCardInfo.RCA<<16));//ѡ��SD��
    if(errorstatus==SD_OK)errorstatus=EnableWideBusOperation(SDIO_BusWide_4b);	//4λ���,�����MMC��,������4λģʽ
    if((errorstatus==SD_OK)||(SDIO_MULTIMEDIA_CARD==CardType))
    {
        if(SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1||SDCardInfo.CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)
        {
            clkdiv=SDIO_TRANSFER_CLK_DIV+2;	//V1.1/V2.0�����������48/4=12Mhz
        }else clkdiv=SDIO_TRANSFER_CLK_DIV;	//SDHC�����������������48/2=24Mhz
        SDIO_Clock_Set(clkdiv);	//����ʱ��Ƶ��,SDIOʱ�Ӽ��㹫ʽ:SDIO_CKʱ��=SDIOCLK/[clkdiv+2];����,SDIOCLK�̶�Ϊ48Mhz
        errorstatus=SetDeviceMode(SD_DMA_MODE);	//����ΪDMAģʽ
//        errorstatus=SetDeviceMode(SD_POLLING_MODE);//����Ϊ��ѯģʽ
    }
    this->Set_Block_Size(SDCardInfo.CardBlockSize );
    this->Set_Sector_Size(512);
    SDIO_CARD::SDIO_IRQHandler_init(this);
    if(errorstatus==SD_OK)
        this->init_flag= true;
    else
        this->init_flag= false;
    return errorstatus;
}
//SDIOʱ�ӳ�ʼ������
//clkdiv:ʱ�ӷ�Ƶϵ��
//CKʱ��=SDIOCLK/[clkdiv+2];(SDIOCLKʱ�ӹ̶�Ϊ48Mhz)
void SDIO_CARD::SDIO_Clock_Set(uint8_t clkdiv)
{
    uint32_t tmpreg=SDIO->CLKCR;
    tmpreg&=0XFFFFFF00;
    tmpreg|=clkdiv;
    SDIO->CLKCR=tmpreg;
}


//���ϵ�
//��ѯ����SDIO�ӿ��ϵĿ��豸,����ѯ���ѹ������ʱ��
//����ֵ:�������;(0,�޴���)
SD_Error SDIO_CARD::PowerON()
{
    uint8_t i;
    SD_Error errorstatus=SD_OK;
    uint32_t response=0,count=0,validvoltage=0;
    uint32_t SDType=SD_STD_CAPACITY;

    /*��ʼ��ʱ��ʱ�Ӳ��ܴ���400KHz*/
    SDIO_InitStructure.SDIO_ClockDiv = SDIO_INIT_CLK_DIV;	/* HCLK = 72MHz, SDIOCLK = 72MHz, SDIO_CK = HCLK/(178 + 2) = 400 KHz */
    SDIO_InitStructure.SDIO_ClockEdge = SDIO_ClockEdge_Rising;
    SDIO_InitStructure.SDIO_ClockBypass = SDIO_ClockBypass_Disable;  //��ʹ��bypassģʽ��ֱ����HCLK���з�Ƶ�õ�SDIO_CK
    SDIO_InitStructure.SDIO_ClockPowerSave = SDIO_ClockPowerSave_Disable;	// ����ʱ���ر�ʱ�ӵ�Դ
    SDIO_InitStructure.SDIO_BusWide = SDIO_BusWide_1b;	 				//1λ������
    SDIO_InitStructure.SDIO_HardwareFlowControl = SDIO_HardwareFlowControl_Disable;//Ӳ����
    SDIO_Init(&SDIO_InitStructure);

    SDIO_SetPowerState(SDIO_PowerState_ON);	//�ϵ�״̬,������ʱ��
    SDIO->CLKCR|=1<<8;			//SDIOCKʹ��

    for(i=0;i<74;i++)
    {

        SDIO_CmdInitStructure.SDIO_Argument = 0x0;//����CMD0����IDLE STAGEģʽ����.
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_GO_IDLE_STATE; //cmd0
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_No;  //����Ӧ
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;  //��CPSM�ڿ�ʼ��������֮ǰ�ȴ����ݴ��������
        SDIO_SendCommand(&SDIO_CmdInitStructure);	  		//д���������Ĵ���

        errorstatus=CmdError();

        if(errorstatus==SD_OK)break;
    }
    if(errorstatus)return errorstatus;//���ش���״̬

    SDIO_CmdInitStructure.SDIO_Argument = SD_CHECK_PATTERN;	//����CMD8,����Ӧ,���SD���ӿ�����
    SDIO_CmdInitStructure.SDIO_CmdIndex = SDIO_SEND_IF_COND;	//cmd8
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;	 //r7
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;			 //�رյȴ��ж�
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus=CmdResp7Error();						//�ȴ�R7��Ӧ

    if(errorstatus==SD_OK) 								//R7��Ӧ����
    {
        CardType=SDIO_STD_CAPACITY_SD_CARD_V2_0;		//SD 2.0��
        SDType=SD_HIGH_CAPACITY;			   			//��������
    }

    SDIO_CmdInitStructure.SDIO_Argument = 0x00;//����CMD55,����Ӧ
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);		//����CMD55,����Ӧ

    errorstatus=CmdResp1Error(SD_CMD_APP_CMD); 		 	//�ȴ�R1��Ӧ

    if(errorstatus==SD_OK)//SD2.0/SD 1.1,����ΪMMC��
    {
        //SD��,����ACMD41 SD_APP_OP_COND,����Ϊ:0x80100000
        while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
        {
            SDIO_CmdInitStructure.SDIO_Argument = 0x00;//����CMD55,����Ӧ
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;	  //CMD55
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);			//����CMD55,����Ӧ

            errorstatus=CmdResp1Error(SD_CMD_APP_CMD); 	 	//�ȴ�R1��Ӧ

            if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����

            //acmd41�����������֧�ֵĵ�ѹ��Χ��HCSλ��ɣ�HCSλ��һ�����ֿ���SDSc����sdhc
            SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_SD | SDType;	//����ACMD41,����Ӧ
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_OP_COND;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r3
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus=CmdResp3Error(); 					//�ȴ�R3��Ӧ

            if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
            response=SDIO->RESP1;;			   				//�õ���Ӧ
            validvoltage=(((response>>31)==1)?1:0);			//�ж�SD���ϵ��Ƿ����
            count++;
        }
        if(count>=SD_MAX_VOLT_TRIAL)
        {
            errorstatus=SD_INVALID_VOLTRANGE;
            return errorstatus;
        }
        if(response&=SD_HIGH_CAPACITY)
        {
            CardType=SDIO_HIGH_CAPACITY_SD_CARD;
        }
    }else//MMC��
    {
        //MMC��,����CMD1 SDIO_SEND_OP_COND,����Ϊ:0x80FF8000
        while((!validvoltage)&&(count<SD_MAX_VOLT_TRIAL))
        {
            SDIO_CmdInitStructure.SDIO_Argument = SD_VOLTAGE_WINDOW_MMC;//����CMD1,����Ӧ
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_OP_COND;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r3
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus=CmdResp3Error(); 					//�ȴ�R3��Ӧ

            if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
            response=SDIO->RESP1;;			   				//�õ���Ӧ
            validvoltage=(((response>>31)==1)?1:0);
            count++;
        }
        if(count>=SD_MAX_VOLT_TRIAL)
        {
            errorstatus=SD_INVALID_VOLTRANGE;
            return errorstatus;
        }
        CardType=SDIO_MULTIMEDIA_CARD;
    }
    return(errorstatus);
}
//SD�� Power OFF
//����ֵ:�������;(0,�޴���)
SD_Error SDIO_CARD::PowerOFF()
{

    SDIO_SetPowerState(SDIO_PowerState_OFF);//SDIO��Դ�ر�,ʱ��ֹͣ

    return SD_OK;
}
//��ʼ�����еĿ�,���ÿ��������״̬
//����ֵ:�������
SD_Error SDIO_CARD::InitializeCards()
{
    SD_Error errorstatus;
    uint16_t rca = 0x01;

    if (SDIO_GetPowerState() == SDIO_PowerState_OFF)	//����Դ״̬,ȷ��Ϊ�ϵ�״̬
    {
        errorstatus = SD_REQUEST_NOT_APPLICABLE;
        return(errorstatus);
    }

    if(SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//��SECURE_DIGITAL_IO_CARD
    {
        SDIO_CmdInitStructure.SDIO_Argument = 0x0;//����CMD2,ȡ��CID,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_ALL_SEND_CID;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);//����CMD2,ȡ��CID,����Ӧ

        errorstatus=CmdResp2Error(); 					//�ȴ�R2��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����

        CID_Tab[0]=SDIO->RESP1;
        CID_Tab[1]=SDIO->RESP2;
        CID_Tab[2]=SDIO->RESP3;
        CID_Tab[3]=SDIO->RESP4;
    }
    if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_SECURE_DIGITAL_IO_COMBO_CARD==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))//�жϿ�����
    {
        SDIO_CmdInitStructure.SDIO_Argument = 0x00;//����CMD3,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;	//cmd3
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r6
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);	//����CMD3,����Ӧ

        errorstatus=CmdResp6Error(SD_CMD_SET_REL_ADDR,&rca);//�ȴ�R6��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
    }
    if (SDIO_MULTIMEDIA_CARD==CardType)
    {

        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca<<16);//����CMD3,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_REL_ADDR;	//cmd3
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short; //r6
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);	//����CMD3,����Ӧ

        errorstatus=CmdResp2Error(); 					//�ȴ�R2��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����
    }
    if (SDIO_SECURE_DIGITAL_IO_CARD!=CardType)			//��SECURE_DIGITAL_IO_CARD
    {
        RCA = rca;

        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)(rca << 16);//����CMD9+��RCA,ȡ��CSD,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_CSD;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Long;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus=CmdResp2Error(); 					//�ȴ�R2��Ӧ
        if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����

        CSD_Tab[0]=SDIO->RESP1;
        CSD_Tab[1]=SDIO->RESP2;
        CSD_Tab[2]=SDIO->RESP3;
        CSD_Tab[3]=SDIO->RESP4;
    }
    return SD_OK;//����ʼ���ɹ�
}
//�õ�����Ϣ
//cardinfo:����Ϣ�洢��
//����ֵ:����״̬
SD_Error SDIO_CARD::GetCardInfo(CardInfo *cardinfo)
{
    SD_Error errorstatus=SD_OK;
    uint8_t tmp=0;
    cardinfo->CardType=(uint8_t) CardType; 				//������
    cardinfo->RCA=(u16)RCA;							//��RCAֵ
    tmp=(uint8_t) ((CSD_Tab[0]&0xFF000000)>>24);
    cardinfo->SD_csd.CSDStruct=(tmp&0xC0)>>6;		//CSD�ṹ
    cardinfo->SD_csd.SysSpecVersion=(tmp&0x3C)>>2;	//2.0Э�黹û�����ⲿ��(Ϊ����),Ӧ���Ǻ���Э�鶨���
    cardinfo->SD_csd.Reserved1=tmp&0x03;			//2������λ
    tmp=(uint8_t) ((CSD_Tab[0]&0x00FF0000)>>16);			//��1���ֽ�
    cardinfo->SD_csd.TAAC=tmp;				   		//���ݶ�ʱ��1
    tmp=(uint8_t) ((CSD_Tab[0]&0x0000FF00)>>8);	  		//��2���ֽ�
    cardinfo->SD_csd.NSAC=tmp;		  				//���ݶ�ʱ��2
    tmp=(uint8_t) (CSD_Tab[0]&0x000000FF);				//��3���ֽ�
    cardinfo->SD_csd.MaxBusClkFrec=tmp;		  		//�����ٶ�
    tmp=(uint8_t) ((CSD_Tab[1]&0xFF000000)>>24);			//��4���ֽ�
    cardinfo->SD_csd.CardComdClasses=tmp<<4;    	//��ָ�������λ
    tmp=(uint8_t) ((CSD_Tab[1]&0x00FF0000)>>16);	 		//��5���ֽ�
    cardinfo->SD_csd.CardComdClasses|=(tmp&0xF0)>>4;//��ָ�������λ
    cardinfo->SD_csd.RdBlockLen=tmp&0x0F;	    	//����ȡ���ݳ���
    tmp=(uint8_t) ((CSD_Tab[1]&0x0000FF00)>>8);			//��6���ֽ�
    cardinfo->SD_csd.PartBlockRead=(tmp&0x80)>>7;	//����ֿ��
    cardinfo->SD_csd.WrBlockMisalign=(tmp&0x40)>>6;	//д���λ
    cardinfo->SD_csd.RdBlockMisalign=(tmp&0x20)>>5;	//�����λ
    cardinfo->SD_csd.DSRImpl=(tmp&0x10)>>4;
    cardinfo->SD_csd.Reserved2=0; 					//����
    if((CardType==SDIO_STD_CAPACITY_SD_CARD_V1_1)||(CardType==SDIO_STD_CAPACITY_SD_CARD_V2_0)||(SDIO_MULTIMEDIA_CARD==CardType))//��׼1.1/2.0��/MMC��
    {
        cardinfo->SD_csd.DeviceSize=(tmp&0x03)<<10;	//C_SIZE(12λ)
        tmp=(uint8_t) (CSD_Tab[1]&0x000000FF); 			//��7���ֽ�
        cardinfo->SD_csd.DeviceSize|=(tmp)<<2;
        tmp=(uint8_t) ((CSD_Tab[2]&0xFF000000)>>24);		//��8���ֽ�
        cardinfo->SD_csd.DeviceSize|=(tmp&0xC0)>>6;
        cardinfo->SD_csd.MaxRdCurrentVDDMin=(tmp&0x38)>>3;
        cardinfo->SD_csd.MaxRdCurrentVDDMax=(tmp&0x07);
        tmp=(uint8_t) ((CSD_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�
        cardinfo->SD_csd.MaxWrCurrentVDDMin=(tmp&0xE0)>>5;
        cardinfo->SD_csd.MaxWrCurrentVDDMax=(tmp&0x1C)>>2;
        cardinfo->SD_csd.DeviceSizeMul=(tmp&0x03)<<1;//C_SIZE_MULT
        tmp=(uint8_t) ((CSD_Tab[2]&0x0000FF00)>>8);	  	//��10���ֽ�
        cardinfo->SD_csd.DeviceSizeMul|=(tmp&0x80)>>7;
        cardinfo->CardCapacity=(cardinfo->SD_csd.DeviceSize+1);//���㿨����
        cardinfo->CardCapacity*=(1<<(cardinfo->SD_csd.DeviceSizeMul+2));
        cardinfo->CardBlockSize=1<<(cardinfo->SD_csd.RdBlockLen);//���С
        cardinfo->CardCapacity*=cardinfo->CardBlockSize;
    }else if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//��������
    {
        tmp=(uint8_t) (CSD_Tab[1]&0x000000FF); 		//��7���ֽ�
        cardinfo->SD_csd.DeviceSize=(tmp&0x3F)<<16;//C_SIZE
        tmp=(uint8_t) ((CSD_Tab[2]&0xFF000000)>>24); 	//��8���ֽ�
        cardinfo->SD_csd.DeviceSize|=(tmp<<8);
        tmp=(uint8_t) ((CSD_Tab[2]&0x00FF0000)>>16);	//��9���ֽ�
        cardinfo->SD_csd.DeviceSize|=(tmp);
        tmp=(uint8_t) ((CSD_Tab[2]&0x0000FF00)>>8); 	//��10���ֽ�
        cardinfo->CardCapacity=(long long)(cardinfo->SD_csd.DeviceSize+1)*512*1024;//���㿨����
        cardinfo->CardBlockSize=512; 			//���С�̶�Ϊ512�ֽ�
    }
    cardinfo->SD_csd.EraseGrSize=(tmp&0x40)>>6;
    cardinfo->SD_csd.EraseGrMul=(tmp&0x3F)<<1;
    tmp=(uint8_t) (CSD_Tab[2]&0x000000FF);			//��11���ֽ�
    cardinfo->SD_csd.EraseGrMul|=(tmp&0x80)>>7;
    cardinfo->SD_csd.WrProtectGrSize=(tmp&0x7F);
    tmp=(uint8_t) ((CSD_Tab[3]&0xFF000000)>>24);		//��12���ֽ�
    cardinfo->SD_csd.WrProtectGrEnable=(tmp&0x80)>>7;
    cardinfo->SD_csd.ManDeflECC=(tmp&0x60)>>5;
    cardinfo->SD_csd.WrSpeedFact=(tmp&0x1C)>>2;
    cardinfo->SD_csd.MaxWrBlockLen=(tmp&0x03)<<2;
    tmp=(uint8_t) ((CSD_Tab[3]&0x00FF0000)>>16);		//��13���ֽ�
    cardinfo->SD_csd.MaxWrBlockLen|=(tmp&0xC0)>>6;
    cardinfo->SD_csd.WriteBlockPaPartial=(tmp&0x20)>>5;
    cardinfo->SD_csd.Reserved3=0;
    cardinfo->SD_csd.ContentProtectAppli=(tmp&0x01);
    tmp=(uint8_t) ((CSD_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
    cardinfo->SD_csd.FileFormatGrouop=(tmp&0x80)>>7;
    cardinfo->SD_csd.CopyFlag=(tmp&0x40)>>6;
    cardinfo->SD_csd.PermWrProtect=(tmp&0x20)>>5;
    cardinfo->SD_csd.TempWrProtect=(tmp&0x10)>>4;
    cardinfo->SD_csd.FileFormat=(tmp&0x0C)>>2;
    cardinfo->SD_csd.ECC=(tmp&0x03);
    tmp=(uint8_t) (CSD_Tab[3]&0x000000FF);			//��15���ֽ�
    cardinfo->SD_csd.CCRC=(tmp&0xFE)>>1;
    cardinfo->SD_csd.Reserved4=1;
    tmp=(uint8_t) ((CID_Tab[0]&0xFF000000)>>24);		//��0���ֽ�
    cardinfo->SD_cid.ManufacturerID=tmp;
    tmp=(uint8_t) ((CID_Tab[0]&0x00FF0000)>>16);		//��1���ֽ�
    cardinfo->SD_cid.OEM_AppliID=tmp<<8;
    tmp=(uint8_t) ((CID_Tab[0]&0x000000FF00)>>8);		//��2���ֽ�
    cardinfo->SD_cid.OEM_AppliID|=tmp;
    tmp=(uint8_t) (CID_Tab[0]&0x000000FF);			//��3���ֽ�
    cardinfo->SD_cid.ProdName1=tmp<<24;
    tmp=(uint8_t) ((CID_Tab[1]&0xFF000000)>>24); 		//��4���ֽ�
    cardinfo->SD_cid.ProdName1|=tmp<<16;
    tmp=(uint8_t) ((CID_Tab[1]&0x00FF0000)>>16);	   	//��5���ֽ�
    cardinfo->SD_cid.ProdName1|=tmp<<8;
    tmp=(uint8_t) ((CID_Tab[1]&0x0000FF00)>>8);		//��6���ֽ�
    cardinfo->SD_cid.ProdName1|=tmp;
    tmp=(uint8_t) (CID_Tab[1]&0x000000FF);	  		//��7���ֽ�
    cardinfo->SD_cid.ProdName2=tmp;
    tmp=(uint8_t) ((CID_Tab[2]&0xFF000000)>>24); 		//��8���ֽ�
    cardinfo->SD_cid.ProdRev=tmp;
    tmp=(uint8_t) ((CID_Tab[2]&0x00FF0000)>>16);		//��9���ֽ�
    cardinfo->SD_cid.ProdSN=tmp<<24;
    tmp=(uint8_t) ((CID_Tab[2]&0x0000FF00)>>8); 		//��10���ֽ�
    cardinfo->SD_cid.ProdSN|=tmp<<16;
    tmp=(uint8_t) (CID_Tab[2]&0x000000FF);   			//��11���ֽ�
    cardinfo->SD_cid.ProdSN|=tmp<<8;
    tmp=(uint8_t) ((CID_Tab[3]&0xFF000000)>>24); 		//��12���ֽ�
    cardinfo->SD_cid.ProdSN|=tmp;
    tmp=(uint8_t) ((CID_Tab[3]&0x00FF0000)>>16);	 	//��13���ֽ�
    cardinfo->SD_cid.Reserved1|=(tmp&0xF0)>>4;
    cardinfo->SD_cid.ManufactDate=(tmp&0x0F)<<8;
    tmp=(uint8_t) ((CID_Tab[3]&0x0000FF00)>>8);		//��14���ֽ�
    cardinfo->SD_cid.ManufactDate|=tmp;
    tmp=(uint8_t) (CID_Tab[3]&0x000000FF);			//��15���ֽ�
    cardinfo->SD_cid.CID_CRC=(tmp&0xFE)>>1;
    cardinfo->SD_cid.Reserved2=1;
    return errorstatus;
}
//����SDIO���߿��(MMC����֧��4bitģʽ)
//wmode:λ��ģʽ.0,1λ���ݿ��;1,4λ���ݿ��;2,8λ���ݿ��
//����ֵ:SD������״̬

//����SDIO���߿��(MMC����֧��4bitģʽ)
//   @arg SDIO_BusWide_8b: 8-bit data transfer (Only for MMC)
//   @arg SDIO_BusWide_4b: 4-bit data transfer
//   @arg SDIO_BusWide_1b: 1-bit data transfer (Ĭ��)
//����ֵ:SD������״̬


SD_Error SDIO_CARD::EnableWideBusOperation(uint32_t WideMode)
{
    SD_Error errorstatus=SD_OK;
    if (SDIO_MULTIMEDIA_CARD == CardType)
    {
        errorstatus = SD_UNSUPPORTED_FEATURE;
        return(errorstatus);
    }

    else if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
    {
        if (SDIO_BusWide_8b == WideMode)   //2.0 sd��֧��8bits
        {
            errorstatus = SD_UNSUPPORTED_FEATURE;
            return(errorstatus);
        }
        else
        {
            errorstatus=SDEnWideBus(WideMode);
            if(SD_OK==errorstatus)
            {
                SDIO->CLKCR&=~(3<<11);		//���֮ǰ��λ������
                SDIO->CLKCR|=WideMode;//1λ/4λ���߿��
                SDIO->CLKCR|=0<<14;			//������Ӳ��������
            }
        }
    }
    return errorstatus;
}
//����SD������ģʽ
//Mode:
//����ֵ:����״̬
SD_Error SDIO_CARD::SetDeviceMode(uint32_t Mode)
{
    SD_Error errorstatus = SD_OK;
    if((Mode==SD_DMA_MODE)||(Mode==SD_POLLING_MODE))DeviceMode=Mode;
    else errorstatus=SD_INVALID_PARAMETER;
    return errorstatus;
}
//ѡ��
//����CMD7,ѡ����Ե�ַ(rca)Ϊaddr�Ŀ�,ȡ��������.���Ϊ0,�򶼲�ѡ��.
//addr:����RCA��ַ
SD_Error SDIO_CARD::SelectDeselect(uint32_t addr)
{

    SDIO_CmdInitStructure.SDIO_Argument =  addr;//����CMD7,ѡ��,����Ӧ
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEL_DESEL_CARD;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);//����CMD7,ѡ��,����Ӧ

    return CmdResp1Error(SD_CMD_SEL_DESEL_CARD);
}
//SD����ȡһ����
//buf:�����ݻ�����(����4�ֽڶ���!!)
//addr:��ȡ��ַ
//blksize:���С
SD_Error SDIO_CARD::ReadBlock(uint8_t *buf,long long addr,u16 blksize)
{
    SD_Error errorstatus=SD_OK;
    uint8_t power;
    uint32_t count=0,*tempbuff=(uint32_t*)buf;//ת��Ϊuint32_tָ��
    uint32_t timeout=SDIO_DATATIMEOUT;
    if(nullptr==buf)
        return SD_INVALID_PARAMETER;
    SDIO->DCTRL=0x0;	//���ݿ��ƼĴ�������(��DMA)

    if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
    {
        blksize=512;
        addr>>=9;
    }
    SDIO_DataInitStructure.SDIO_DataBlockSize= SDIO_DataBlockSize_1b ;//���DPSM״̬������
    SDIO_DataInitStructure.SDIO_DataLength= 0 ;
    SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);


    if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
    if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
    {
        power=convert_from_bytes_to_power_of_two(blksize);


        SDIO_CmdInitStructure.SDIO_Argument =  blksize;
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);//����CMD16+�������ݳ���Ϊblksize,����Ӧ


        errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����

    }else return SD_INVALID_PARAMETER;

    SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4 ;//���DPSM״̬������
    SDIO_DataInitStructure.SDIO_DataLength= blksize ;
    SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATATIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    SDIO_CmdInitStructure.SDIO_Argument =  addr;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_SINGLE_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);//����CMD17+��addr��ַ����ȡ����,����Ӧ

    errorstatus=CmdResp1Error(SD_CMD_READ_SINGLE_BLOCK);//�ȴ�R1��Ӧ
    if(errorstatus!=SD_OK)return errorstatus;   		//��Ӧ����
    if(DeviceMode==SD_POLLING_MODE)						//��ѯģʽ,��ѯ����
    {
        INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
        while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<10)|(1<<9))))//������/CRC/��ʱ/���(��־)/��ʼλ����
        {
            if(SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)						//����������,��ʾ���ٴ���8����
            {
                for(count=0;count<8;count++)			//ѭ����ȡ����
                {
                    *(tempbuff+count)=SDIO->FIFO;
                }
                tempbuff+=8;
                timeout=0X7FFFFF; 	//���������ʱ��
            }else 	//����ʱ
            {
                if(timeout==0)return SD_DATA_TIMEOUT;
                timeout--;
            }
        }
        if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
        {
            SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
            return SD_DATA_TIMEOUT;
        }else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
        {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
            return SD_DATA_CRC_FAIL;
        }else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	//����fifo�������
        {
            SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		//������־
            return SD_RX_OVERRUN;
        }else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
        {
            SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
            return SD_START_BIT_ERR;
        }
        while(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	//FIFO����,�����ڿ�������
        {
            *tempbuff=SDIO->FIFO;	//ѭ����ȡ����
            tempbuff++;
        }
        INTX_ENABLE();//�������ж�
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��

    }else if(DeviceMode==SD_DMA_MODE)
    {
        TransferError=SD_OK;
        StopCondition=0;			//�����,����Ҫ����ֹͣ����ָ��
        TransferEnd=0;				//�����������λ�����жϷ�����1
        SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж�
        SDIO->DCTRL|=1<<3;		 	//SDIO DMAʹ��
        DMA_Config((uint32_t*)buf,blksize,DMA_DIR_PeripheralToMemory);
        while(((DMA2->LISR&(1<<27))==RESET)&&(TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;//�ȴ��������
        if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
        if(TransferError!=SD_OK)errorstatus=TransferError;
    }
    return errorstatus;
}
//SD����ȡ�����
//buf:�����ݻ�����
//addr:��ȡ��ַ
//blksize:���С
//nblks:Ҫ��ȡ�Ŀ���
//����ֵ:����״̬
__attribute__((aligned(4))) uint32_t *tempbuff;
SD_Error SDIO_CARD::ReadMultiBlocks(uint8_t *buf,long long addr,u16 blksize,uint32_t nblks)
{
    SD_Error errorstatus=SD_OK;
    uint8_t power;
    uint32_t count=0;
    uint32_t timeout=SDIO_DATATIMEOUT;
    tempbuff=(uint32_t*)buf;//ת��Ϊuint32_tָ��

    SDIO->DCTRL=0x0;		//���ݿ��ƼĴ�������(��DMA)
    if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
    {
        blksize=512;
        addr>>=9;
    }

    SDIO_DataInitStructure.SDIO_DataBlockSize= 0; ;//���DPSM״̬������
    SDIO_DataInitStructure.SDIO_DataLength= 0 ;
    SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATA_TIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
    if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
    {
        power=convert_from_bytes_to_power_of_two(blksize);

        SDIO_CmdInitStructure.SDIO_Argument =  blksize;//����CMD16+�������ݳ���Ϊblksize,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����

    }else return SD_INVALID_PARAMETER;

    if(nblks>1)											//����
    {
        if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;//�ж��Ƿ񳬹������ճ���

        SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4; ;//nblks*blksize,512���С,����������
        SDIO_DataInitStructure.SDIO_DataLength= nblks*blksize ;
        SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATA_TIMEOUT ;
        SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
        SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToSDIO;
        SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
        SDIO_DataConfig(&SDIO_DataInitStructure);

        SDIO_CmdInitStructure.SDIO_Argument =  addr;//����CMD18+��addr��ַ����ȡ����,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_READ_MULT_BLOCK;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus=CmdResp1Error(SD_CMD_READ_MULT_BLOCK);//�ȴ�R1��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����

        if(DeviceMode==SD_POLLING_MODE)
        {
            INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
            while(!(SDIO->STA&((1<<5)|(1<<1)|(1<<3)|(1<<8)|(1<<9))))//������/CRC/��ʱ/���(��־)/��ʼλ����
            {
                if(SDIO_GetFlagStatus(SDIO_FLAG_RXFIFOHF) != RESET)						//����������,��ʾ���ٴ���8����
                {
                    for(count=0;count<8;count++)			//ѭ����ȡ����
                    {
                        *(tempbuff+count)=SDIO->FIFO;
                    }
                    tempbuff+=8;
                    timeout=0X7FFFFF; 	//���������ʱ��
                }else 	//����ʱ
                {
                    if(timeout==0)return SD_DATA_TIMEOUT;
                    timeout--;
                }
            }
            if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
            {
                SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
                return SD_DATA_TIMEOUT;
            }else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
            {
                SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
                return SD_DATA_CRC_FAIL;
            }else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	//����fifo�������
            {
                SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		//������־
                return SD_RX_OVERRUN;
            }else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
            {
                SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
                return SD_START_BIT_ERR;
            }

            while(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)	//FIFO����,�����ڿ�������
            {
                *tempbuff=SDIO->FIFO;	//ѭ����ȡ����
                tempbuff++;
            }
            if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		//���ս���
            {
                if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
                {
                    SDIO_CmdInitStructure.SDIO_Argument =  0;//����CMD12+��������
                    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
                    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
                    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
                    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
                    SDIO_SendCommand(&SDIO_CmdInitStructure);

                    errorstatus=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ

                    if(errorstatus!=SD_OK)return errorstatus;
                }
            }
            INTX_ENABLE();//�������ж�
            SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
        }else if(DeviceMode==SD_DMA_MODE)
        {
            TransferError=SD_OK;
            StopCondition=1;			//����,��Ҫ����ֹͣ����ָ��
            TransferEnd=0;				//�����������λ�����жϷ�����1
            SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<5)|(1<<9);	//������Ҫ���ж�
            SDIO->DCTRL|=1<<3;		 						//SDIO DMAʹ��
            DMA_Config((uint32_t*)buf,nblks*blksize,DMA_DIR_PeripheralToMemory);
            while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//�ȴ��������
            if(timeout==0)return SD_DATA_TIMEOUT;//��ʱ
            while((TransferEnd==0)&&(TransferError==SD_OK));
            if(TransferError!=SD_OK)errorstatus=TransferError;
        }
    }
    return errorstatus;
}
//SD��д1����
//buf:���ݻ�����
//addr:д��ַ
//blksize:���С
//����ֵ:����״̬
SD_Error SDIO_CARD::WriteBlock(uint8_t *buf,long long addr,  u16 blksize)
{
    SD_Error errorstatus = SD_OK;

    uint8_t power=0,cardstate=0;

    uint32_t timeout=0,bytestransferred=0;

    uint32_t cardstatus=0,count=0,restwords=0;

    uint32_t	tlen=blksize;						//�ܳ���(�ֽ�)

    tempbuff=(uint32_t*)buf;

    if(buf==nullptr)return SD_INVALID_PARAMETER;//��������

    SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)

    SDIO_DataInitStructure.SDIO_DataBlockSize= 0; ;//���DPSM״̬������
    SDIO_DataInitStructure.SDIO_DataLength= 0 ;
    SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATA_TIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);


    if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
    if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)	//��������
    {
        blksize=512;
        addr>>=9;
    }
    if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
    {
        power=convert_from_bytes_to_power_of_two(blksize);

        SDIO_CmdInitStructure.SDIO_Argument = blksize;//����CMD16+�������ݳ���Ϊblksize,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����

    }else return SD_INVALID_PARAMETER;

    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA<<16;//����CMD13,��ѯ����״̬,����Ӧ
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);		//�ȴ�R1��Ӧ

    if(errorstatus!=SD_OK)return errorstatus;
    cardstatus=SDIO->RESP1;
    timeout=SD_DATA_TIMEOUT;
    while(((cardstatus&0x00000100)==0)&&(timeout>0)) 	//���READY_FOR_DATAλ�Ƿ���λ
    {
        timeout--;

        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA<<16;//����CMD13,��ѯ����״̬,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);	//�ȴ�R1��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;

        cardstatus=SDIO->RESP1;
    }
    if(timeout==0)return SD_ERROR;

    SDIO_CmdInitStructure.SDIO_Argument = addr;//����CMD24,д����ָ��,����Ӧ
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_SINGLE_BLOCK;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus=CmdResp1Error(SD_CMD_WRITE_SINGLE_BLOCK);//�ȴ�R1��Ӧ

    if(errorstatus!=SD_OK)return errorstatus;

    StopCondition=0;									//����д,����Ҫ����ֹͣ����ָ��

    SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4; ;	//blksize, ����������
    SDIO_DataInitStructure.SDIO_DataLength= blksize ;
    SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATA_TIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);


    timeout=SDIO_DATATIMEOUT;

    if (DeviceMode == SD_POLLING_MODE)
    {
        INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
        while(!(SDIO->STA&((1<<10)|(1<<4)|(1<<1)|(1<<3)|(1<<9))))//���ݿ鷢�ͳɹ�/����/CRC/��ʱ/��ʼλ����
        {
            if(SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)							//���������,��ʾ���ٴ���8����
            {
                if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
                {
                    restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);

                    for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
                    {
                        SDIO->FIFO=*tempbuff;
                    }
                }else
                {
                    for(count=0;count<8;count++)
                    {
                        SDIO->FIFO=*(tempbuff+count);
                    }
                    tempbuff+=8;
                    bytestransferred+=32;
                }
                timeout=0X3FFFFFFF;	//д�������ʱ��
            }else
            {
                if(timeout==0)return SD_DATA_TIMEOUT;
                timeout--;
            }
        }
        if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
        {
            SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
            return SD_DATA_TIMEOUT;
        }else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
        {
            SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
            return SD_DATA_CRC_FAIL;
        }else if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	//����fifo�������
        {
            SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		//������־
            return SD_TX_UNDERRUN;
        }else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
        {
            SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
            return SD_START_BIT_ERR;
        }

        INTX_ENABLE();//�������ж�
        SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    }else if(DeviceMode==SD_DMA_MODE)
    {
        TransferError=SD_OK;
        StopCondition=0;			//����д,����Ҫ����ֹͣ����ָ��
        TransferEnd=0;				//�����������λ�����жϷ�����1
        SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�
        DMA_Config((uint32_t*)buf,blksize,DMA_DIR_MemoryToPeripheral);				//SDIO DMA����
        SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��.
        while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//�ȴ��������
        if(timeout==0)
        {
            init();	 					//���³�ʼ��SD��,���Խ��д������������
            return SD_DATA_TIMEOUT;			//��ʱ
        }
        timeout=SDIO_DATATIMEOUT;
        while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
        if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ
        if(TransferError!=SD_OK)return TransferError;
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    errorstatus=IsCardProgramming(&cardstate);
    while((errorstatus==SD_OK)&&((cardstate==CARD_PROGRAMMING)||(cardstate==CARD_RECEIVING)))
    {
        errorstatus=IsCardProgramming(&cardstate);
    }
    return errorstatus;
}
//SD��д�����
//buf:���ݻ�����
//addr:д��ַ
//blksize:���С
//nblks:Ҫд��Ŀ���
//����ֵ:����״̬
SD_Error SDIO_CARD::WriteMultiBlocks(uint8_t *buf,long long addr,u16 blksize,uint32_t nblks)
{
    SD_Error errorstatus = SD_OK;
    uint8_t power = 0, cardstate = 0;
    uint32_t timeout=0,bytestransferred=0;
    uint32_t count = 0, restwords = 0;
    uint32_t tlen=nblks*blksize;				//�ܳ���(�ֽ�)
    tempbuff = (uint32_t*)buf;
    if(buf==nullptr)return SD_INVALID_PARAMETER; //��������
    SDIO->DCTRL=0x0;							//���ݿ��ƼĴ�������(��DMA)

    SDIO_DataInitStructure.SDIO_DataBlockSize= 0; ;	//���DPSM״̬������
    SDIO_DataInitStructure.SDIO_DataLength= 0 ;
    SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATA_TIMEOUT ;
    SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
    SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
    SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//������
    if(CardType==SDIO_HIGH_CAPACITY_SD_CARD)//��������
    {
        blksize=512;
        addr>>=9;
    }
    if((blksize>0)&&(blksize<=2048)&&((blksize&(blksize-1))==0))
    {
        power=convert_from_bytes_to_power_of_two(blksize);

        SDIO_CmdInitStructure.SDIO_Argument = blksize;	//����CMD16+�������ݳ���Ϊblksize,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);	//�ȴ�R1��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;   	//��Ӧ����

    }else return SD_INVALID_PARAMETER;
    if(nblks>1)
    {
        if(nblks*blksize>SD_MAX_DATA_LENGTH)return SD_INVALID_PARAMETER;
        if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
        {
            //�������
            SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)RCA<<16;		//����ACMD55,����Ӧ
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus=CmdResp1Error(SD_CMD_APP_CMD);		//�ȴ�R1��Ӧ

            if(errorstatus!=SD_OK)return errorstatus;

            SDIO_CmdInitStructure.SDIO_Argument =nblks;		//����CMD23,���ÿ�����,����Ӧ
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCK_COUNT;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            errorstatus=CmdResp1Error(SD_CMD_SET_BLOCK_COUNT);//�ȴ�R1��Ӧ

            if(errorstatus!=SD_OK)return errorstatus;

        }

        SDIO_CmdInitStructure.SDIO_Argument =addr;	//����CMD25,���дָ��,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_WRITE_MULT_BLOCK;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus=CmdResp1Error(SD_CMD_WRITE_MULT_BLOCK);	//�ȴ�R1��Ӧ

        if(errorstatus!=SD_OK)return errorstatus;

        SDIO_DataInitStructure.SDIO_DataBlockSize= power<<4; ;	//blksize, ����������
        SDIO_DataInitStructure.SDIO_DataLength= nblks*blksize ;
        SDIO_DataInitStructure.SDIO_DataTimeOut=SD_DATA_TIMEOUT ;
        SDIO_DataInitStructure.SDIO_DPSM=SDIO_DPSM_Enable;
        SDIO_DataInitStructure.SDIO_TransferDir=SDIO_TransferDir_ToCard;
        SDIO_DataInitStructure.SDIO_TransferMode=SDIO_TransferMode_Block;
        SDIO_DataConfig(&SDIO_DataInitStructure);

        if(DeviceMode==SD_POLLING_MODE)
        {
            timeout=SDIO_DATATIMEOUT;
            INTX_DISABLE();//�ر����ж�(POLLINGģʽ,�Ͻ��жϴ��SDIO��д����!!!)
            while(!(SDIO->STA&((1<<4)|(1<<1)|(1<<8)|(1<<3)|(1<<9))))//����/CRC/���ݽ���/��ʱ/��ʼλ����
            {
                if(SDIO_GetFlagStatus(SDIO_FLAG_TXFIFOHE) != RESET)							//���������,��ʾ���ٴ���8��(32�ֽ�)
                {
                    if((tlen-bytestransferred)<SD_HALFFIFOBYTES)//����32�ֽ���
                    {
                        restwords=((tlen-bytestransferred)%4==0)?((tlen-bytestransferred)/4):((tlen-bytestransferred)/4+1);
                        for(count=0;count<restwords;count++,tempbuff++,bytestransferred+=4)
                        {
                            SDIO->FIFO=*tempbuff;
                        }
                    }else 										//���������,���Է�������8��(32�ֽ�)����
                    {
                        for(count=0;count<SD_HALFFIFO;count++)
                        {
                            SDIO->FIFO=*(tempbuff+count);
                        }
                        tempbuff+=SD_HALFFIFO;
                        bytestransferred+=SD_HALFFIFOBYTES;
                    }
                    timeout=0X3FFFFFFF;	//д�������ʱ��
                }else
                {
                    if(timeout==0)return SD_DATA_TIMEOUT;
                    timeout--;
                }
            }
            if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
            {
                SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
                return SD_DATA_TIMEOUT;
            }else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
            {
                SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
                return SD_DATA_CRC_FAIL;
            }else if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET) 	//����fifo�������
            {
                SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);		//������־
                return SD_TX_UNDERRUN;
            }else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
            {
                SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
                return SD_START_BIT_ERR;
            }

            if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)		//���ͽ���
            {
                if((SDIO_STD_CAPACITY_SD_CARD_V1_1==CardType)||(SDIO_STD_CAPACITY_SD_CARD_V2_0==CardType)||(SDIO_HIGH_CAPACITY_SD_CARD==CardType))
                {
                    SDIO_CmdInitStructure.SDIO_Argument =0;//����CMD12+��������
                    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
                    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
                    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
                    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
                    SDIO_SendCommand(&SDIO_CmdInitStructure);

                    errorstatus=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);//�ȴ�R1��Ӧ
                    if(errorstatus!=SD_OK)return errorstatus;
                }
            }
            INTX_ENABLE();//�������ж�
            SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
        }else if(DeviceMode==SD_DMA_MODE)
        {
            TransferError=SD_OK;
            StopCondition=1;			//���д,��Ҫ����ֹͣ����ָ��
            TransferEnd=0;				//�����������λ�����жϷ�����1
            SDIO->MASK|=(1<<1)|(1<<3)|(1<<8)|(1<<4)|(1<<9);	//���ò������ݽ�������ж�
            DMA_Config((uint32_t*)buf,nblks*blksize,DMA_DIR_MemoryToPeripheral);		//SDIO DMA����
            SDIO->DCTRL|=1<<3;								//SDIO DMAʹ��.
            timeout=SDIO_DATATIMEOUT;
            while(((DMA2->LISR&(1<<27))==RESET)&&timeout)timeout--;//�ȴ��������
            if(timeout==0)	 								//��ʱ
            {
                init();	 					//���³�ʼ��SD��,���Խ��д������������
                return SD_DATA_TIMEOUT;			//��ʱ
            }
            timeout=SDIO_DATATIMEOUT;
            while((TransferEnd==0)&&(TransferError==SD_OK)&&timeout)timeout--;
            if(timeout==0)return SD_DATA_TIMEOUT;			//��ʱ
            if(TransferError!=SD_OK)return TransferError;
        }
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    errorstatus=IsCardProgramming(&cardstate);
    while((errorstatus==SD_OK)&&((cardstate==CARD_PROGRAMMING)||(cardstate==CARD_RECEIVING)))
    {
        errorstatus=IsCardProgramming(&cardstate);
    }
    return errorstatus;
}
//SDIO�жϴ�����
//����SDIO��������еĸ����ж�����
//����ֵ:�������
SD_Error SDIO_CARD::ProcessIRQSrc()
{
    if(SDIO_GetFlagStatus(SDIO_FLAG_DATAEND) != RESET)//��������ж�
    {
        if (StopCondition==1)
        {
            SDIO_CmdInitStructure.SDIO_Argument =0;//����CMD12+��������
            SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_STOP_TRANSMISSION;
            SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
            SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
            SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
            SDIO_SendCommand(&SDIO_CmdInitStructure);

            TransferError=CmdResp1Error(SD_CMD_STOP_TRANSMISSION);
        }else TransferError = SD_OK;
        SDIO->ICR|=1<<8;//�������жϱ��
        SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
        TransferEnd = 1;
        return(TransferError);
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)//����CRC����
    {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
        SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
        TransferError = SD_DATA_CRC_FAIL;
        return(SD_DATA_CRC_FAIL);
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)//���ݳ�ʱ����
    {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT);  			//���жϱ�־
        SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
        TransferError = SD_DATA_TIMEOUT;
        return(SD_DATA_TIMEOUT);
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET)//FIFO�������
    {
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);  			//���жϱ�־
        SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
        TransferError = SD_RX_OVERRUN;
        return(SD_RX_OVERRUN);
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_TXUNDERR) != RESET)//FIFO�������
    {
        SDIO_ClearFlag(SDIO_FLAG_TXUNDERR);  			//���жϱ�־
        SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
        TransferError = SD_TX_UNDERRUN;
        return(SD_TX_UNDERRUN);
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET)//��ʼλ����
    {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);  		//���жϱ�־
        SDIO->MASK&=~((1<<1)|(1<<3)|(1<<8)|(1<<14)|(1<<15)|(1<<4)|(1<<5)|(1<<9));//�ر�����ж�
        TransferError = SD_START_BIT_ERR;
        return(SD_START_BIT_ERR);
    }
    return(SD_OK);
}

//���CMD0��ִ��״̬
//����ֵ:sd��������
SD_Error SDIO_CARD::CmdError()
{
    SD_Error errorstatus = SD_OK;
    uint32_t timeout=SDIO_CMD0TIMEOUT;
    while(timeout--)
    {
        if(SDIO_GetFlagStatus(SDIO_FLAG_CMDSENT) != RESET)break;	//�����ѷ���(������Ӧ)
    }
    if(timeout==0)return SD_CMD_RSP_TIMEOUT;
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    return errorstatus;
}
//���R7��Ӧ�Ĵ���״̬
//����ֵ:sd��������
SD_Error SDIO_CARD::CmdResp7Error()
{
    SD_Error errorstatus=SD_OK;
    uint32_t status;
    uint32_t timeout=SDIO_CMD0TIMEOUT;
    while(timeout--)
    {
        status=SDIO->STA;
        if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
    }
    if((timeout==0)||(status&(1<<2)))	//��Ӧ��ʱ
    {
        errorstatus=SD_CMD_RSP_TIMEOUT;	//��ǰ������2.0���ݿ�,���߲�֧���趨�ĵ�ѹ��Χ
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 			//���������Ӧ��ʱ��־
        return errorstatus;
    }
    if(status&1<<6)						//�ɹ����յ���Ӧ
    {
        errorstatus=SD_OK;
        SDIO_ClearFlag(SDIO_FLAG_CMDREND); 				//�����Ӧ��־
    }
    return errorstatus;
}
//���R1��Ӧ�Ĵ���״̬
//cmd:��ǰ����
//����ֵ:sd��������
SD_Error SDIO_CARD::CmdResp1Error(uint8_t cmd)
{
    uint32_t status;
    while(true)
    {
        status=SDIO->STA;
        if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)					//��Ӧ��ʱ
    {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 				//���������Ӧ��ʱ��־
        return SD_CMD_RSP_TIMEOUT;
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)					//CRC����
    {
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL); 				//�����־
        return SD_CMD_CRC_FAIL;
    }
    if(SDIO->RESPCMD!=cmd)return SD_ILLEGAL_CMD;//���ƥ��
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    return (SD_Error)(SDIO->RESP1&SD_OCR_ERRORBITS);//���ؿ���Ӧ
}
//���R3��Ӧ�Ĵ���״̬
//����ֵ:����״̬
SD_Error SDIO_CARD::CmdResp3Error()
{
    uint32_t status;
    while(true)
    {
        status=SDIO->STA;
        if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)					//��Ӧ��ʱ
    {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			//���������Ӧ��ʱ��־
        return SD_CMD_RSP_TIMEOUT;
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    return SD_OK;
}
//���R2��Ӧ�Ĵ���״̬
//����ֵ:����״̬
SD_Error SDIO_CARD::CmdResp2Error()
{
    SD_Error errorstatus=SD_OK;
    uint32_t status;
    uint32_t timeout=SDIO_CMD0TIMEOUT;
    while(timeout--)
    {
        status=SDIO->STA;
        if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
    }
    if((timeout==0)||(status&(1<<2)))	//��Ӧ��ʱ
    {
        errorstatus=SD_CMD_RSP_TIMEOUT;
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT); 		//���������Ӧ��ʱ��־
        return errorstatus;
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)						//CRC����
    {
        errorstatus=SD_CMD_CRC_FAIL;
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);		//�����Ӧ��־
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    return errorstatus;
}
//���R6��Ӧ�Ĵ���״̬
//cmd:֮ǰ���͵�����
//prca:�����ص�RCA��ַ
//����ֵ:����״̬
SD_Error SDIO_CARD::CmdResp6Error(uint8_t cmd,u16*prca)
{
    SD_Error errorstatus=SD_OK;
    uint32_t status;
    uint32_t rspr1;
    while(true)
    {
        status=SDIO->STA;
        if(status&((1<<0)|(1<<2)|(1<<6)))break;//CRC����/������Ӧ��ʱ/�Ѿ��յ���Ӧ(CRCУ��ɹ�)
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)					//��Ӧ��ʱ
    {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			//���������Ӧ��ʱ��־
        return SD_CMD_RSP_TIMEOUT;
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)						//CRC����
    {
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);					//�����Ӧ��־
        return SD_CMD_CRC_FAIL;
    }
    if(SDIO->RESPCMD!=cmd)				//�ж��Ƿ���Ӧcmd����
    {
        return SD_ILLEGAL_CMD;
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    rspr1=SDIO->RESP1;					//�õ���Ӧ
    if(SD_ALLZERO==(rspr1&(SD_R6_GENERAL_UNKNOWN_ERROR|SD_R6_ILLEGAL_CMD|SD_R6_COM_CRC_FAILED)))
    {
        *prca=(u16)(rspr1>>16);			//����16λ�õ�,rca
        return errorstatus;
    }
    if(rspr1&SD_R6_GENERAL_UNKNOWN_ERROR)return SD_GENERAL_UNKNOWN_ERROR;
    if(rspr1&SD_R6_ILLEGAL_CMD)return SD_ILLEGAL_CMD;
    if(rspr1&SD_R6_COM_CRC_FAILED)return SD_COM_CRC_FAILED;
    return errorstatus;
}

//SDIOʹ�ܿ�����ģʽ
//enx:0,��ʹ��;1,ʹ��;
//����ֵ:����״̬
SD_Error SDIO_CARD::SDEnWideBus(uint8_t enx)
{
    SD_Error errorstatus = SD_OK;
    uint32_t scr[2]={0,0};
    uint8_t arg=0X00;
    if(enx)arg=0X02;
    else arg=0X00;
    if(SDIO->RESP1&SD_CARD_LOCKED)return SD_LOCK_UNLOCK_FAILED;//SD������LOCKED״̬
    errorstatus=FindSCR(RCA,scr);						//�õ�SCR�Ĵ�������
    if(errorstatus!=SD_OK)return errorstatus;
    if((scr[1]&SD_WIDE_BUS_SUPPORT)!=SD_ALLZERO)		//֧�ֿ�����
    {
        SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;//����CMD55+RCA,����Ӧ
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus=CmdResp1Error(SD_CMD_APP_CMD);

        if(errorstatus!=SD_OK)return errorstatus;

        SDIO_CmdInitStructure.SDIO_Argument = arg;//����ACMD6,����Ӧ,����:10,4λ;00,1λ.
        SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_SD_SET_BUSWIDTH;
        SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
        SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
        SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
        SDIO_SendCommand(&SDIO_CmdInitStructure);

        errorstatus=CmdResp1Error(SD_CMD_APP_SD_SET_BUSWIDTH);

        return errorstatus;
    }else return SD_REQUEST_NOT_APPLICABLE;				//��֧�ֿ���������
}
//��鿨�Ƿ�����ִ��д����
//pstatus:��ǰ״̬.
//����ֵ:�������
SD_Error SDIO_CARD::IsCardProgramming(uint8_t *pstatus)
{
    volatile uint32_t respR1 = 0, status = 0;

    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16; //����Ե�ַ����
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;//����CMD13
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    status=SDIO->STA;

    while(!(status&((1<<0)|(1<<6)|(1<<2))))status=SDIO->STA;//�ȴ��������
    if(SDIO_GetFlagStatus(SDIO_FLAG_CCRCFAIL) != RESET)			//CRC���ʧ��
    {
        SDIO_ClearFlag(SDIO_FLAG_CCRCFAIL);	//���������
        return SD_CMD_CRC_FAIL;
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_CTIMEOUT) != RESET)			//���ʱ
    {
        SDIO_ClearFlag(SDIO_FLAG_CTIMEOUT);			//���������
        return SD_CMD_RSP_TIMEOUT;
    }
    if(SDIO->RESPCMD!=SD_CMD_SEND_STATUS)return SD_ILLEGAL_CMD;
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    respR1=SDIO->RESP1;
    *pstatus=(uint8_t) ((respR1>>9)&0x0000000F);
    return SD_OK;
}
//��ȡ��ǰ��״̬
//pcardstatus:��״̬
//����ֵ:�������
SD_Error SDIO_CARD::SendStatus(uint32_t *pcardstatus)
{
    SD_Error errorstatus = SD_OK;
    if(pcardstatus==nullptr)
    {
        errorstatus=SD_INVALID_PARAMETER;
        return errorstatus;
    }

    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;//����CMD13,����Ӧ
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SEND_STATUS;
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus=CmdResp1Error(SD_CMD_SEND_STATUS);	//��ѯ��Ӧ״̬
    if(errorstatus!=SD_OK)return errorstatus;
    *pcardstatus=SDIO->RESP1;//��ȡ��Ӧֵ
    return errorstatus;
}
//����SD����״̬
//����ֵ:SD��״̬
SDCardState SDIO_CARD::GetState()
{
    uint32_t resp1=0;
    if(SendStatus(&resp1)!=SD_OK)return CARD_ERROR;
    else return (SDCardState)((resp1>>9) & 0x0F);
}
//����SD����SCR�Ĵ���ֵ
//rca:����Ե�ַ
//pscr:���ݻ�����(�洢SCR����)
//����ֵ:����״̬
SD_Error SDIO_CARD::SDIO_CARD::FindSCR(u16 rca,uint32_t *pscr)
{
    uint32_t index = 0;
    SD_Error errorstatus = SD_OK;
    uint32_t tempscr[2]={0,0};

    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t)8;	 //����CMD16,����Ӧ,����Block SizeΪ8�ֽ�
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SET_BLOCKLEN; //	 cmd16
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r1
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus=CmdResp1Error(SD_CMD_SET_BLOCKLEN);

    if(errorstatus!=SD_OK)return errorstatus;

    SDIO_CmdInitStructure.SDIO_Argument = (uint32_t) RCA << 16;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_APP_CMD;//����CMD55,����Ӧ
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus=CmdResp1Error(SD_CMD_APP_CMD);
    if(errorstatus!=SD_OK)return errorstatus;

    SDIO_DataInitStructure.SDIO_DataTimeOut = SD_DATATIMEOUT;
    SDIO_DataInitStructure.SDIO_DataLength = 8;  //8���ֽڳ���,blockΪ8�ֽ�,SD����SDIO.
    SDIO_DataInitStructure.SDIO_DataBlockSize = SDIO_DataBlockSize_8b  ;  //���С8byte
    SDIO_DataInitStructure.SDIO_TransferDir = SDIO_TransferDir_ToSDIO;
    SDIO_DataInitStructure.SDIO_TransferMode = SDIO_TransferMode_Block;
    SDIO_DataInitStructure.SDIO_DPSM = SDIO_DPSM_Enable;
    SDIO_DataConfig(&SDIO_DataInitStructure);

    SDIO_CmdInitStructure.SDIO_Argument = 0x0;
    SDIO_CmdInitStructure.SDIO_CmdIndex = SD_CMD_SD_APP_SEND_SCR;	//����ACMD51,����Ӧ,����Ϊ0
    SDIO_CmdInitStructure.SDIO_Response = SDIO_Response_Short;  //r1
    SDIO_CmdInitStructure.SDIO_Wait = SDIO_Wait_No;
    SDIO_CmdInitStructure.SDIO_CPSM = SDIO_CPSM_Enable;
    SDIO_SendCommand(&SDIO_CmdInitStructure);

    errorstatus=CmdResp1Error(SD_CMD_SD_APP_SEND_SCR);
    if(errorstatus!=SD_OK)return errorstatus;
    while(!(SDIO->STA&(SDIO_FLAG_RXOVERR|SDIO_FLAG_DCRCFAIL|SDIO_FLAG_DTIMEOUT|SDIO_FLAG_DBCKEND|SDIO_FLAG_STBITERR)))
    {
        if(SDIO_GetFlagStatus(SDIO_FLAG_RXDAVL) != RESET)//����FIFO���ݿ���
        {
            *(tempscr+index)=SDIO->FIFO;	//��ȡFIFO����
            index++;
            if(index>=2)break;
        }
    }
    if(SDIO_GetFlagStatus(SDIO_FLAG_DTIMEOUT) != RESET)		//���ݳ�ʱ����
    {
        SDIO_ClearFlag(SDIO_FLAG_DTIMEOUT); 	//������־
        return SD_DATA_TIMEOUT;
    }else if(SDIO_GetFlagStatus(SDIO_FLAG_DCRCFAIL) != RESET)	//���ݿ�CRC����
    {
        SDIO_ClearFlag(SDIO_FLAG_DCRCFAIL);  		//������־
        return SD_DATA_CRC_FAIL;
    }else if(SDIO_GetFlagStatus(SDIO_FLAG_RXOVERR) != RESET) 	//����fifo�������
    {
        SDIO_ClearFlag(SDIO_FLAG_RXOVERR);		//������־
        return SD_RX_OVERRUN;
    }else if(SDIO_GetFlagStatus(SDIO_FLAG_STBITERR) != RESET) 	//������ʼλ����
    {
        SDIO_ClearFlag(SDIO_FLAG_STBITERR);//������־
        return SD_START_BIT_ERR;
    }
    SDIO_ClearFlag(SDIO_STATIC_FLAGS);//������б��
    //������˳��8λΪ��λ������.
    *(pscr+1)=((tempscr[0]&SD_0TO7BITS)<<24)|((tempscr[0]&SD_8TO15BITS)<<8)|((tempscr[0]&SD_16TO23BITS)>>8)|((tempscr[0]&SD_24TO31BITS)>>24);
    *(pscr)=((tempscr[1]&SD_0TO7BITS)<<24)|((tempscr[1]&SD_8TO15BITS)<<8)|((tempscr[1]&SD_16TO23BITS)>>8)|((tempscr[1]&SD_24TO31BITS)>>24);
    return errorstatus;
}
//�õ�NumberOfBytes��2Ϊ�׵�ָ��.
//NumberOfBytes:�ֽ���.
//����ֵ:��2Ϊ�׵�ָ��ֵ
uint8_t SDIO_CARD::convert_from_bytes_to_power_of_two(u16 NumberOfBytes)
{
    uint8_t count=0;
    while(NumberOfBytes!=1)
    {
        NumberOfBytes>>=1;
        count++;
    }
    return count;
}

//����SDIO DMA
//mbuf:�洢����ַ
//bufsize:����������
//dir:����;DMA_DIR_MemoryToPeripheral  �洢��-->SDIO(д����);DMA_DIR_PeripheralToMemory SDIO-->�洢��(������);
void SDIO_CARD::DMA_Config(const uint32_t*mbuf,uint32_t bufsize,uint32_t dir)
{

    DMA_InitTypeDef  DMA_InitStructure;

    while (DMA_GetCmdStatus(DMA2_Stream3) != DISABLE){}//�ȴ�DMA������

    DMA_DeInit(DMA2_Stream3);//���֮ǰ��stream3�ϵ������жϱ�־


    DMA_InitStructure.DMA_Channel = DMA_Channel_4;  //ͨ��ѡ��
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SDIO->FIFO;//DMA�����ַ
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)mbuf;//DMA �洢��0��ַ
    DMA_InitStructure.DMA_DIR = dir;//�洢��������ģʽ
    DMA_InitStructure.DMA_BufferSize = 0;//���ݴ�����
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//���������ģʽ
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//�洢������ģʽ
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;//�������ݳ���:32λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;//�洢�����ݳ���:32λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;// ʹ����ͨģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//������ȼ�
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Enable;   //FIFOʹ��
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;//ȫFIFO
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;//����ͻ��4�δ���
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;//�洢��ͻ��4�δ���
    DMA_Init(DMA2_Stream3, &DMA_InitStructure);//��ʼ��DMA Stream

    DMA_FlowControllerConfig(DMA2_Stream3,DMA_FlowCtrl_Peripheral);//����������

    DMA_Cmd(DMA2_Stream3 ,ENABLE);//����DMA����

}


//��SD��
//buf:�����ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
uint8_t SDIO_CARD::ReadDisk(uint8_t* buf,uint32_t sector,uint8_t cnt)
{
    uint8_t sta=SD_OK;
    long long lsector=sector;
    uint8_t n;
    lsector<<=9;
    if((uint32_t)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            sta=ReadBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector�Ķ�����
            memcpy(buf,SDIO_DATA_BUFFER,512);
            buf+=512;
        }
    }else
    {
        if(cnt==1)sta=ReadBlock(buf,lsector,512);    	//����sector�Ķ�����
        else sta=ReadMultiBlocks(buf,lsector,512,cnt);//���sector
    }
    return sta;
}
//дSD��
//buf:д���ݻ�����
//sector:������ַ
//cnt:��������
//����ֵ:����״̬;0,����;����,�������;
uint8_t SDIO_CARD::WriteDisk(uint8_t* buf,uint32_t sector,uint8_t cnt)
{
    uint8_t sta=SD_OK;
    uint8_t n;
    long long lsector=sector;
    lsector<<=9;
    if((uint32_t)buf%4!=0)
    {
        for(n=0;n<cnt;n++)
        {
            memcpy(SDIO_DATA_BUFFER,buf,512);
            sta=WriteBlock(SDIO_DATA_BUFFER,lsector+512*n,512);//����sector��д����
            buf+=512;
        }
    }else
    {
        if(cnt==1)sta=WriteBlock(buf,lsector,512);    	//����sector��д����
        else sta=WriteMultiBlocks(buf,lsector,512,cnt);	//���sector
    }
    return sta;
}

struct SDIO_STRUCT_{
    bool extern_flag= false;
    Call_Back *ext{};
}SDIO_STRUCT;
//SDIO�жϷ�����
extern "C" void SDIO_IRQHandler()
{
//    SD_ProcessIRQSrc();//��������SDIO����ж�
    SDIO_STRUCT.ext->Callback(0,nullptr);
}
void SDIO_CARD::SDIO_IRQHandler_init(Call_Back *event) {
    SDIO_STRUCT.ext=event;
}


#define WAIT_TIME 5
void SDIO_CARD::USER_init()
{
    uint8_t time_i=WAIT_TIME;
    uint32_t total,free;
    while(init()&&time_i)
    {
        time_i--;
//        delay_ms(100);
    }
    if(time_i>0)
    {
//        f_mount(fs[0],"0:",1);
//        time_i=WAIT_TIME;
//        while(exf_getfree((uint8_t )"0:",&total,&free)&&time_i)	//�õ�SD������������ʣ������
//        {
//            time_i--;
////			KDebug("SD Card Fatfs Error!\r\n");
//            delay_ms(500);
//        }
        if(time_i>0)
        {
//			KDebug("SD������:	%d.%d	GB\r\n",total/1048576,total%1048576/10240);
//			KDebug("ʣ������:	%d.%d	GB\r\n", free/1048576, free%1048576/10240);
        }
    }
}

void SDIO_CARD::Callback(int, char **) {
    this->ProcessIRQSrc();
}

bool SDIO_CARD::FAT_init() {
    return init();
}

uint32_t SDIO_CARD::GetSectorCount() {
    return SDCardInfo.CardCapacity/512;
}

uint16_t SDIO_CARD::write(uint32_t addr, uint8_t data) {
    if(WriteDisk(&data,addr,1)==SD_OK)
        return 1;
    else return 0;
}

uint16_t SDIO_CARD::write(uint32_t addr, uint8_t *data, uint16_t len) {
    if(WriteDisk(data,addr,len)==SD_OK)
        return len;
    else return 0;
}

uint8_t SDIO_CARD::read(uint32_t addr) {
    uint8_t data=0;
    ReadDisk(&data,addr,1);
    return data;
}

void SDIO_CARD::read(uint32_t addr, uint8_t *data) {
    ReadDisk(data,addr,1);
}

void SDIO_CARD::read(uint32_t addr, uint8_t *data, uint16_t len) {
    ReadDisk(data,addr,len);
}




