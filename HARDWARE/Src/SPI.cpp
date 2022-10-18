/**
* @Author kokirika
* @Name SPI
* @Date 2022-09-17
**/

#include "SPI.h"
#include "DMA.h"

SPI::SPI(SPI_TypeDef* SPI,uint16_t DataSize,uint8_t SPI_BaudRatePrescaler) {
    this->init(SPI,DataSize,SPI_BaudRatePrescaler);
}

void SPI::config(GPIO_TypeDef *PORT_SCK,uint32_t Pin_SCK,\
                GPIO_TypeDef *PORT_MISO,uint32_t Pin_MISO,\
                GPIO_TypeDef *PORT_MOSI,uint32_t Pin_MOSI){
    this->SCK.init(PORT_SCK,Pin_SCK,GPIO_Mode_AF);
    this->MISO.init(PORT_MISO,Pin_MISO,GPIO_Mode_AF);
    this->MOSI.init(PORT_MOSI,Pin_MOSI,GPIO_Mode_AF);
    this->config_flag=1;
}

void SPI::config(uint8_t Pin_SCK, uint8_t Pin_MISO, uint8_t Pin_MOSI) {
    this->SCK.init(Pin_SCK,GPIO_Mode_AF);
    this->MISO.init(Pin_MISO,GPIO_Mode_AF);
    this->MOSI.init(Pin_MOSI,GPIO_Mode_AF);
    this->config_flag=1;
}

void SPI::default_config() {
    if (this->SPIx == SPI1) {
        if (this->config_flag == 0)
            this->config(GPIOA5,GPIOA6,GPIOA7);
        this->SCK.set_AFConfig(GPIO_AF_SPI1);
        this->MISO.set_AFConfig(GPIO_AF_SPI1);
        this->MOSI.set_AFConfig(GPIO_AF_SPI1);
    }
    else if (this->SPIx == SPI2) {
        if (this->config_flag == 0)
            this->config(GPIOB10,GPIOC2,GPIOC3);
        this->SCK.set_AFConfig(GPIO_AF_SPI2);
        this->MISO.set_AFConfig(GPIO_AF_SPI2);
        this->MOSI.set_AFConfig(GPIO_AF_SPI2);
    }
    else if (this->SPIx == SPI3) {
        if (this->config_flag == 0)
            this->config(GPIOC10,GPIOC11,GPIOC12);
        this->SCK.set_AFConfig(GPIO_AF_SPI3);
        this->MISO.set_AFConfig(GPIO_AF_SPI3);
        this->MOSI.set_AFConfig(GPIO_AF_SPI3);
    }
}

void SPI::init(SPI_TypeDef* SPI,uint16_t DataSize,uint8_t SPI_BaudRatePrescaler) {
    this->SPIx=SPI;
    this->default_config();
    if(SPI==SPI1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    if(SPI==SPI2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    if(SPI==SPI3)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

    SPI_I2S_DeInit(this->SPIx);

    this->SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
    this->SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//设置SPI工作模式:设置为主SPI
    this->SPI_InitStructure.SPI_DataSize = DataSize;		//设置SPI的数据大小:SPI发送接收8位帧结构
    this->SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//串行同步时钟的空闲状态为高电平
    this->SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    this->SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    this->SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler;		//定义波特率预分频的值:波特率预分频值为256
    this->SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    this->SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
    SPI_Init(this->SPIx, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器

    SPI_Cmd(this->SPIx, ENABLE); //使能SPI外设
}

void SPI::SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    this->SPIx->CR1&=0XFFC7;
    this->SPIx->CR1|=SPI_BaudRatePrescaler;	//设置SPI1速度
    SPI_Cmd(this->SPIx,ENABLE);
}

uint16_t SPI::ReadWriteDATA(uint16_t TxData)
{
    while (SPI_I2S_GetFlagStatus(this->SPIx, SPI_I2S_FLAG_TXE) == RESET){}//等待发送区空
    SPI_I2S_SendData(this->SPIx, TxData); //通过外设SPIx发送一个byte  数据
    while (SPI_I2S_GetFlagStatus(this->SPIx, SPI_I2S_FLAG_RXNE) == RESET){} //等待接收完一个byte
    return SPI_I2S_ReceiveData(this->SPIx); //返回通过SPIx最近接收的数据
}

void SPI::set_send_DMA(FunctionalState enable) {
    SPI_I2S_DMACmd(this->SPIx,SPI_I2S_DMAReq_Tx,enable);  //使能串口1的DMA发送
    this->DMA_Enable=(enable==ENABLE)?ON:OFF;
    if(this->SPIx==SPI1){
        this->DMAy_Streamx=DMA2_Stream3;
        this->DMA_FLAG=DMA_FLAG_TCIF3;
        this->DMA_CHANNEL=DMA_Channel_3;
    }
    else if(this->SPIx==SPI2){
        this->DMAy_Streamx=DMA1_Stream4;
        this->DMA_FLAG=DMA_FLAG_TCIF4;
        this->DMA_CHANNEL=DMA_Channel_0;
    }
    else if(this->SPIx==SPI3){
        this->DMAy_Streamx=DMA1_Stream5;
        this->DMA_FLAG=DMA_FLAG_TCIF5;
        this->DMA_CHANNEL=DMA_Channel_0;
    }
}

void SPI::set_dma_streamx(DMA_Stream_TypeDef *DMAy_Stream) {
    this->DMAy_Streamx=DMAy_Stream;
}

void SPI::DMA_WriteData(uint16_t *TxData, uint16_t len) {
    if(!this->DMA_send_flag)
        this->DMA_send_flag= true;
    else if(DMA_GetFlagStatus(this->DMAy_Streamx,this->DMA_FLAG)!=RESET)//等待DMA2_Steam7传输完成
    {
        DMA_ClearFlag(this->DMAy_Streamx, this->DMA_FLAG);//清除DMA2_Steam7传输完成标志
    }
    DMA_send(this->DMAy_Streamx,this->DMA_CHANNEL,\
        (u32)&this->SPIx->DR,(uint32_t)TxData,len,\
        DMA_DIR_MemoryToPeripheral,8);
}


