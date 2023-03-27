/**
* @Author kokirika
* @Name USB_MSC
* @Date 2022-11-25
**/

#include "USB_MSC.h"
#include "delay.h"

uint8_t AppState=0;
USBH_HOST  USB_Host;
USB_OTG_CORE_HANDLE  USB_OTG_Core;
//uint8_t USB_MCS_DIRVE_SATA=0;


void USB_OTG_BSP_Init(USB_OTG_CORE_HANDLE *pdev)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_OTG_FS, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource11,GPIO_AF_OTG_FS);//PA11,AF10(USB)
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource12,GPIO_AF_OTG_FS);//PA12,AF10(USB)
}

/**
  * @brief  USB_OTG_BSP_EnableInterrupt
  *         Configures USB Global interrupt
  * @param  None
  * @retval None
  */
void USB_OTG_BSP_EnableInterrupt(USB_OTG_CORE_HANDLE *pdev)
{
    NVIC_InitTypeDef   NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = OTG_FS_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}


//USB OTG �ж�����,����USB FS�ж�
//pdev:USB OTG�ں˽ṹ��ָ��
void USB_OTG_BSP_DisableInterrupt()
{
}
//pdev:USB OTG�ں˽ṹ��ָ��
//state:0,�ϵ�;1,�ϵ�
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
}
//pdev:USB OTG�ں˽ṹ��ָ��
void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{
}
//USB_OTG us����ʱ����

void USB_OTG_BSP_uDelay (const uint32_t usec)
{
    delay_us(usec);
}
//USB_OTG ms����ʱ����
void USB_OTG_BSP_mDelay (const uint32_t msec)
{
    delay_ms(msec);
}

//USB HOST ��ʼ��
void USBH_USR_Init()
{

}
//��⵽U�̲���
void USBH_USR_DeviceAttached()//U�̲���
{
//    Picture_cut(pic01_1,2,261,0,340,47,261,0);  //�Զ�
    AppState=USB_MSC::SATA::Linking;;
}
//��⵽U�̰γ�
void USBH_USR_DeviceDisconnected ()//U���Ƴ�
{
//    Picture_cut(pic01_1,2,176,0,255,47,176,0);  //�Զ�
    AppState=USB_MSC::SATA::UnLink;;
}
//��λ�ӻ�
void USBH_USR_ResetDevice()
{
}
//��⵽�ӻ��ٶ�
//DeviceSpeed:�ӻ��ٶ�(0,1,2 / ����)
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
}
//��⵽�ӻ���������
//DeviceDesc:�豸������ָ��
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
//	USBH_DevDesc_TypeDef *hs;
//	hs=DeviceDesc;
}
//�ӻ���ַ����ɹ�
void USBH_USR_DeviceAddressAssigned()
{
}
//��������������Ч
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
//	USBH_InterfaceDesc_TypeDef *id;
//	id = itfDesc;
}
//��ȡ���豸Manufacturer String
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
}
//��ȡ���豸Product String
void USBH_USR_Product_String(void *ProductString)
{
}
//��ȡ���豸SerialNum String
void USBH_USR_SerialNum_String(void *SerialNumString)
{
}
//�豸USBö�����
void USBH_USR_EnumerationDone()
{
}
//�޷�ʶ���USB�豸
void USBH_USR_DeviceNotSupported()
{
}
//�ȴ��û����밴��,ִ����һ������
USBH_USR_Status USBH_USR_UserInput()
{
    return USBH_USR_RESP_OK;
}
//USB�ӿڵ�������
void USBH_USR_OverCurrentDetected ()
{
}
//USB HOST MSC���û�Ӧ�ó���
int USBH_USR_MSC_Application()
{
    uint8_t res=0;
    switch(AppState)
    {
        case USB_MSC::SATA::UnLink ://��ʼ���ļ�ϵͳ
//            f_mount(fs[2],"2:",1); 	//����U��
//            AppState=USB_MSC::SATA::Linking;
            AppState=USB_MSC::SATA::UnLinking;
            break;
        case USB_MSC::SATA::Linking:	//ִ��USB OTG ����������
            if(HCD_IsDeviceConnected(&USB_OTG_Core))//�豸���ӳɹ�
            {
                AppState=USB_MSC::SATA::Linked;
//                USB_MCS_DIRVE_SATA=USB_MSC::SATA::Linked;
            }
            break;
        default:break;
    }
    return res;
}
//�û�Ҫ�����³�ʼ���豸
void USBH_USR_DeInit()
{
    AppState=USB_MSC::SATA::UnLink;
}
//�޷��ָ��Ĵ���!!
void USBH_USR_UnrecoveredError ()
{
}

USBH_Usr_cb_TypeDef USR_Callbacks=
        {
                USBH_USR_Init,
                USBH_USR_DeInit,
                USBH_USR_DeviceAttached,
                USBH_USR_ResetDevice,
                USBH_USR_DeviceDisconnected,
                USBH_USR_OverCurrentDetected,
                USBH_USR_DeviceSpeedDetected,
                USBH_USR_Device_DescAvailable,
                USBH_USR_DeviceAddressAssigned,
                USBH_USR_Configuration_DescAvailable,
                USBH_USR_Manufacturer_String,
                USBH_USR_Product_String,
                USBH_USR_SerialNum_String,
                USBH_USR_EnumerationDone,
                USBH_USR_UserInput,
                USBH_USR_MSC_Application,
                USBH_USR_DeviceNotSupported,
                USBH_USR_UnrecoveredError
        };

extern "C" void OTG_FS_IRQHandler()
{
    USBH_OTG_ISR_Handler(&USB_OTG_Core);
}

uint8_t USB_MSC::UDISK_Read(uint8_t *buf, uint32_t sector, uint16_t cnt) {
    uint8_t res;
    if(HCD_IsDeviceConnected(&USB_OTG_Core))//���ӻ�����
    {
        do
        {
            res=USBH_MSC_Read10(&USB_OTG_Core,buf,sector,512*cnt);
            USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);
            if(!HCD_IsDeviceConnected(&USB_OTG_Core))
            {
                res=1;//��д����
                break;
            };
        }while(res==USBH_MSC_BUSY);
    }else res=1;
    if(res==USBH_MSC_OK)res=0;
    return res;
}

uint8_t USB_MSC::UDISK_Write(uint8_t *buf, uint32_t sector, uint16_t cnt) {
    uint8_t res=1;
    if(HCD_IsDeviceConnected(&USB_OTG_Core))//���ӻ�����
    {
        do
        {
            res=USBH_MSC_Write10(&USB_OTG_Core,buf,sector,512*cnt);
            USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);
            if(!HCD_IsDeviceConnected(&USB_OTG_Core))
            {
                res=1;//��д����
                break;
            };
        }while(res==USBH_MSC_BUSY);
    }else res=1;
    if(res==USBH_MSC_OK)res=0;
    return res;
}

void USB_MSC::init()
{
    this->init_flag= true;
    USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_Callbacks);
    this->Set_Block_Size(512);
    this->Set_Sector_Size(USBH_MSC_Param.MSCapacity);
    this->device_sata= USB_MSC::SATA::UnLink;
};

bool USB_MSC::FAT_init() {
    return HCD_IsDeviceConnected(&USB_OTG_Core);
}

uint32_t USB_MSC::GetSectorCount() {
    return Storage_BASE::GetSectorCount();
}

uint16_t USB_MSC::write(uint32_t addr, uint8_t data) {
    if(UDISK_Write(&data,addr,1))
        return 0;
    else
        return 1;
}

uint16_t USB_MSC::write(uint32_t addr, uint8_t *data, uint16_t len) {
    if(UDISK_Write(data,addr,len))
        return 0;
    else
        return len;
}

uint8_t USB_MSC::read(uint32_t addr) {
    uint8_t data;
    UDISK_Read(&data,addr,1);
    return data;
}

void USB_MSC::read(uint32_t addr, uint8_t *data) {
    UDISK_Read(data,addr,1);
}

void USB_MSC::read(uint32_t addr, uint8_t *data, uint16_t len) {
    UDISK_Read(data,addr,len);
}

void USB_MSC::Upset() {
    USBH_Process(&USB_OTG_Core, &USB_Host);
    switch (AppState) {
        case SATA::UnLink : this->device_sata=SATA::UnLink;break;
        case SATA::Linking :this->device_sata=SATA::Linking;break;
        case SATA::Linked : this->device_sata=SATA::Linked;break;
        case SATA::UnLinking :this->device_sata=SATA::UnLinking;break;
//        default:  this->device_sata=SATA::UnLink;break;
    }
    this->Set_Sector_Size(USBH_MSC_Param.MSCapacity);
}

USB_MSC::SATA USB_MSC::Get_device_sata() const {
    return this->device_sata;
}

void USB_MSC::wait_Linked() {
    this->Upset();
    while(this->Get_device_sata()!=USB_MSC::SATA::Linked)//��ⲻ��SD��
    {
      this->Upset();
    }
}


