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


//USB OTG 中断设置,开启USB FS中断
//pdev:USB OTG内核结构体指针
void USB_OTG_BSP_DisableInterrupt()
{
}
//pdev:USB OTG内核结构体指针
//state:0,断电;1,上电
void USB_OTG_BSP_DriveVBUS(USB_OTG_CORE_HANDLE *pdev, uint8_t state)
{
}
//pdev:USB OTG内核结构体指针
void  USB_OTG_BSP_ConfigVBUS(USB_OTG_CORE_HANDLE *pdev)
{
}
//USB_OTG us级延时函数

void USB_OTG_BSP_uDelay (const uint32_t usec)
{
    delay_us(usec);
}
//USB_OTG ms级延时函数
void USB_OTG_BSP_mDelay (const uint32_t msec)
{
    delay_ms(msec);
}

//USB HOST 初始化
void USBH_USR_Init()
{

}
//检测到U盘插入
void USBH_USR_DeviceAttached()//U盘插入
{
//    Picture_cut(pic01_1,2,261,0,340,47,261,0);  //自动
    AppState=USB_MSC::SATA::Linking;;
}
//检测到U盘拔出
void USBH_USR_DeviceDisconnected ()//U盘移除
{
//    Picture_cut(pic01_1,2,176,0,255,47,176,0);  //自动
    AppState=USB_MSC::SATA::UnLink;;
}
//复位从机
void USBH_USR_ResetDevice()
{
}
//检测到从机速度
//DeviceSpeed:从机速度(0,1,2 / 其他)
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
}
//检测到从机的描述符
//DeviceDesc:设备描述符指针
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
//	USBH_DevDesc_TypeDef *hs;
//	hs=DeviceDesc;
}
//从机地址分配成功
void USBH_USR_DeviceAddressAssigned()
{
}
//配置描述符获有效
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
//	USBH_InterfaceDesc_TypeDef *id;
//	id = itfDesc;
}
//获取到设备Manufacturer String
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
}
//获取到设备Product String
void USBH_USR_Product_String(void *ProductString)
{
}
//获取到设备SerialNum String
void USBH_USR_SerialNum_String(void *SerialNumString)
{
}
//设备USB枚举完成
void USBH_USR_EnumerationDone()
{
}
//无法识别的USB设备
void USBH_USR_DeviceNotSupported()
{
}
//等待用户输入按键,执行下一步操作
USBH_USR_Status USBH_USR_UserInput()
{
    return USBH_USR_RESP_OK;
}
//USB接口电流过载
void USBH_USR_OverCurrentDetected ()
{
}
//USB HOST MSC类用户应用程序
int USBH_USR_MSC_Application()
{
    uint8_t res=0;
    switch(AppState)
    {
        case USB_MSC::SATA::UnLink ://初始化文件系统
//            f_mount(fs[2],"2:",1); 	//挂载U盘
//            AppState=USB_MSC::SATA::Linking;
            AppState=USB_MSC::SATA::UnLinking;
            break;
        case USB_MSC::SATA::Linking:	//执行USB OTG 测试主程序
            if(HCD_IsDeviceConnected(&USB_OTG_Core))//设备连接成功
            {
                AppState=USB_MSC::SATA::Linked;
//                USB_MCS_DIRVE_SATA=USB_MSC::SATA::Linked;
            }
            break;
        default:break;
    }
    return res;
}
//用户要求重新初始化设备
void USBH_USR_DeInit()
{
    AppState=USB_MSC::SATA::UnLink;
}
//无法恢复的错误!!
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
    if(HCD_IsDeviceConnected(&USB_OTG_Core))//连接还存在
    {
        do
        {
            res=USBH_MSC_Read10(&USB_OTG_Core,buf,sector,512*cnt);
            USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);
            if(!HCD_IsDeviceConnected(&USB_OTG_Core))
            {
                res=1;//读写错误
                break;
            };
        }while(res==USBH_MSC_BUSY);
    }else res=1;
    if(res==USBH_MSC_OK)res=0;
    return res;
}

uint8_t USB_MSC::UDISK_Write(uint8_t *buf, uint32_t sector, uint16_t cnt) {
    uint8_t res=1;
    if(HCD_IsDeviceConnected(&USB_OTG_Core))//连接还存在
    {
        do
        {
            res=USBH_MSC_Write10(&USB_OTG_Core,buf,sector,512*cnt);
            USBH_MSC_HandleBOTXfer(&USB_OTG_Core ,&USB_Host);
            if(!HCD_IsDeviceConnected(&USB_OTG_Core))
            {
                res=1;//读写错误
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
    while(this->Get_device_sata()!=USB_MSC::SATA::Linked)//检测不到SD卡
    {
      this->Upset();
    }
}


