/**
* @Author kokirika
* @Name SPI
* @Date 2022-09-17
**/

#include "SPI.h"
#include "DMA.h"
#include "delay.h"

SPI::SPI(SPI_TypeDef* SPI,Queue mode,uint16_t DataSize,uint8_t SPI_BaudRatePrescaler) {
    this->init(SPI,mode,DataSize,SPI_BaudRatePrescaler);
    this->config_flag = 0;
}

SPI::SPI() {
    this->config_flag = 0;
}

void SPI::config(GPIO_TypeDef *PORT_SCK,uint32_t Pin_SCK,\
                GPIO_TypeDef *PORT_MISO,uint32_t Pin_MISO,\
                GPIO_TypeDef *PORT_MOSI,uint32_t Pin_MOSI){
    this->SCK.init(PORT_SCK,Pin_SCK,GPIO_Mode_AF);
    this->MISO.init(PORT_MISO,Pin_MISO,GPIO_Mode_AF);
    this->MOSI.init(PORT_MOSI,Pin_MOSI,GPIO_Mode_AF);
    if (this->SPIx == SPI1) {
        this->SCK.set_AFConfig(GPIO_AF_SPI1);
        this->MISO.set_AFConfig(GPIO_AF_SPI1);
        this->MOSI.set_AFConfig(GPIO_AF_SPI1);
    }
    else if (this->SPIx == SPI2) {
        this->SCK.set_AFConfig(GPIO_AF_SPI2);
        this->MISO.set_AFConfig(GPIO_AF_SPI2);
        this->MOSI.set_AFConfig(GPIO_AF_SPI2);
    }
    else if (this->SPIx == SPI3) {
        this->SCK.set_AFConfig(GPIO_AF_SPI3);
        this->MISO.set_AFConfig(GPIO_AF_SPI3);
        this->MOSI.set_AFConfig(GPIO_AF_SPI3);
    }
    this->config_flag=1;
}

void SPI::config(uint8_t Pin_SCK, uint8_t Pin_MISO, uint8_t Pin_MOSI) {
    this->SCK.set_pinmode(GPIO_Mode_IN);
    this->MISO.set_pinmode(GPIO_Mode_IN);
    this->MOSI.set_pinmode(GPIO_Mode_IN);
    this->SCK.set_PuPD(GPIO_PuPd_NOPULL);
    this->MISO.set_PuPD(GPIO_PuPd_NOPULL);
    this->MOSI.set_PuPD(GPIO_PuPd_NOPULL);
    this->SCK.init(Pin_SCK,GPIO_Mode_AF);
    this->MISO.init(Pin_MISO,GPIO_Mode_AF);
    this->MOSI.init(Pin_MOSI,GPIO_Mode_AF);
    if (this->SPIx == SPI1) {
        this->SCK.set_AFConfig(GPIO_AF_SPI1);
        this->MISO.set_AFConfig(GPIO_AF_SPI1);
        this->MOSI.set_AFConfig(GPIO_AF_SPI1);
    }
    else if (this->SPIx == SPI2) {
        this->SCK.set_AFConfig(GPIO_AF_SPI2);
        this->MISO.set_AFConfig(GPIO_AF_SPI2);
        this->MOSI.set_AFConfig(GPIO_AF_SPI2);
    }
    else if (this->SPIx == SPI3) {
        this->SCK.set_AFConfig(GPIO_AF_SPI3);
        this->MISO.set_AFConfig(GPIO_AF_SPI3);
        this->MOSI.set_AFConfig(GPIO_AF_SPI3);
    }
    this->config_flag=1;
}

void SPI::default_config() {
    if (this->config_flag == 0) {
        if (this->SPIx == SPI1) {
            this->config(GPIOA5, GPIOA6, GPIOA7);
        } else if (this->SPIx == SPI2) {
            this->config(GPIOB10, GPIOC2, GPIOC3);
        } else if (this->SPIx == SPI3) {
            this->config(GPIOC10, GPIOC11, GPIOC12);
        }
    }
}

void SPI::init(SPI_TypeDef* SPI,Queue mode,uint16_t DataSize,uint8_t SPI_BaudRatePrescaler) {
    this->SPIx=SPI;
    this->set_Queue_mode( mode);
    this->default_config();
    this->set_error_times(0xffff);
    if(SPI==SPI1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    if(SPI==SPI2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    if(SPI==SPI3)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI3, ENABLE);

    SPI_I2S_DeInit(this->SPIx);
    SPI_Cmd(this->SPIx, DISABLE); //????SPI????

    this->SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //????SPI??????????????????????:SPI????????????????????
    this->SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//????SPI????????:????????SPI
    this->SPI_InitStructure.SPI_DataSize = DataSize;		//????SPI??????????:SPI????????8????????
    this->SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//??????????????????????????????
    this->SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//??????????????????????????????????????????????????
    this->SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS????????????NSS????????????????????SSI????????:????NSS??????SSI??????
    this->SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler;		//????????????????????:????????????????256
    this->SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//??????????????MSB??????LSB??????:??????????MSB??????
    this->SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC??????????????
    SPI_Init(this->SPIx, &SPI_InitStructure);  //????SPI_InitStruct??????????????????????SPIx??????

    SPI_Cmd(this->SPIx, ENABLE); //????SPI????
}

void SPI::init(Queue mode) {
    if(this->SPIx){
        this->set_Queue_mode( mode);
        SPI_I2S_DeInit(this->SPIx);
        SPI_Init(this->SPIx, &SPI_InitStructure);  //????SPI_InitStruct??????????????????????SPIx??????
        SPI_Cmd(this->SPIx, ENABLE); //????SPI????
    }
}

void SPI::set_error_times(uint32_t times) {
    this->error_times=times;
}

void SPI::SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    this->SPIx->CR1&=0XFFC7;
    this->SPIx->CR1|=SPI_BaudRatePrescaler;	//????SPI1????
    SPI_Cmd(this->SPIx,ENABLE);
}

uint16_t SPI::ReadWriteDATA(uint16_t TxData)
{
    uint32_t error_num=this->error_times;
    while (SPI_I2S_GetFlagStatus(this->SPIx, SPI_I2S_FLAG_TXE) == RESET){
        error_num--;if(error_num==0)break;
    }//????????????
    SPI_I2S_SendData(this->SPIx, TxData); //????????SPIx????????byte  ????
    error_num=this->error_times;
    while (SPI_I2S_GetFlagStatus(this->SPIx, SPI_I2S_FLAG_RXNE) == RESET){
        error_num--;if(error_num==0)break;
    } //??????????????byte
    uint16_t data=SPI_I2S_ReceiveData(this->SPIx); //????????SPIx??????????????

    return data;
}

void SPI::set_send_DMA(FunctionalState enable) {
    SPI_I2S_DMACmd(this->SPIx,SPI_I2S_DMAReq_Tx,enable);  //????????1??DMA????
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
    else if(DMA_GetFlagStatus(this->DMAy_Streamx,this->DMA_FLAG)!=RESET)//????DMA2_Steam7????????
    {
        DMA_ClearFlag(this->DMAy_Streamx, this->DMA_FLAG);//????DMA2_Steam7????????????
    }
    DMA_send(this->DMAy_Streamx,this->DMA_CHANNEL,\
        (u32)&this->SPIx->DR,(uint32_t)TxData,len,\
        DMA_DIR_MemoryToPeripheral,8);
}


SPI_S::SPI_S(uint8_t Pin_SCK, uint8_t Pin_MISO, uint8_t Pin_MOSI,HARD_BASE::Queue mode) {
    this->init(Pin_SCK, Pin_MISO,Pin_MOSI,mode);
}

SPI_S::SPI_S(GPIO_TypeDef *PORT_SCK, uint32_t Pin_SCK, GPIO_TypeDef *PORT_MISO, uint32_t Pin_MISO,
               GPIO_TypeDef *PORT_MOSI, uint32_t Pin_MOSI, HARD_BASE::Queue mode) {
    this->init(PORT_SCK,Pin_SCK,PORT_MISO,Pin_MISO,PORT_MOSI,Pin_MOSI,mode);
}

void SPI_S::init(Queue mode) {
    this->set_Queue_mode( mode);
    this->config(SPI_S::CP::OL_0_HA_1);
    this->SetSpeed(SPI_BaudRatePrescaler_8);
}

void SPI_S::init(uint8_t Pin_SCK, uint8_t Pin_MISO, uint8_t Pin_MOSI,HARD_BASE::Queue mode) {
    this->SCK.init(Pin_SCK,GPIO_Mode_OUT);
    this->MISO.init(Pin_MISO,GPIO_Mode_IN);
    this->MOSI.init(Pin_MOSI,GPIO_Mode_OUT);
    this->SCK.set_output(HIGH);
    this->MOSI.set_output(HIGH);
    this->set_Queue_mode( mode);
    this->config(SPI_S::CP::OL_0_HA_1);
    this->SetSpeed(SPI_BaudRatePrescaler_8);
}

void SPI_S::init(GPIO_TypeDef *PORT_SCK, uint32_t Pin_SCK, GPIO_TypeDef *PORT_MISO, uint32_t Pin_MISO,
                  GPIO_TypeDef *PORT_MOSI, uint32_t Pin_MOSI, HARD_BASE::Queue mode) {
    this->SCK.init(PORT_SCK,Pin_SCK,GPIO_Mode_OUT);
    this->MISO.init(PORT_MISO,Pin_MISO,GPIO_Mode_IN);
    this->MOSI.init(PORT_MOSI,Pin_MOSI,GPIO_Mode_OUT);
    this->SCK.set_output(HIGH);
    this->MOSI.set_output(HIGH);
    this->set_Queue_mode( mode);
    this->config(SPI_S::CP::OL_0_HA_1);
    this->SetSpeed(SPI_BaudRatePrescaler_8);
}
/* CPOL = 0, CPHA = 0, MSB first */
uint8_t SPI_S::SOFT_SPI_RW_MODE0( uint8_t write_dat )
{
    uint8_t read_dat=0;
    for(uint8_t i = 0; i < 8; i++ )
    {
        if( write_dat & 0x80 )
            this->MOSI.set_output(HIGH);
        else
            this->MOSI.set_output(LOW);
        write_dat <<= 1;
        delay_us(1);
        this->SCK.set_output(HIGH);
        read_dat <<= 1;
        if( this->MISO.get_input() )
            read_dat++;
        delay_us(1);
        this->SCK.set_output(LOW);
        delay_us(1);
        //asm("nop");//__asm__("nop");
    }
    return read_dat;
}
/* CPOL=0??CPHA=1, MSB first */
uint8_t SPI_S::SOFT_SPI_RW_MODE1(uint8_t write_dat)
{
    uint8_t Temp=0;
    for(uint8_t i=0;i<8;i++)     // ????8??
    {
        this->SCK.set_output(HIGH);     //????????
        if(write_dat&0x80)
        {
            this->MOSI.set_output(HIGH);  //??????????????????????
        }
        else
        {
            this->MOSI.set_output(LOW);   //??????????????????????
        }
        write_dat <<= 1;     // ??????????????????
        delay_us(1);
        this->SCK.set_output(LOW);     //????????
        Temp <<= 1;     //????????
        if( this->MISO.get_input() )
            Temp++;     //????????????????????????????????
        delay_us(1);
    }
    return (Temp);     //????????
}
/* CPOL=1??CPHA=0, MSB first */
uint8_t SPI_S::SOFT_SPI_RW_MODE2(uint8_t write_dat)
{
    uint8_t Temp=0;
    for(uint8_t i=0;i<8;i++)     // ????8??
    {
        if(write_dat&0x80)
        {
            this->MOSI.set_output(HIGH);  //??????????????????????
        }
        else
        {
            this->MOSI.set_output(LOW);   //??????????????????????
        }
        write_dat <<= 1;     // ??????????????????
        delay_us(1);
        this->SCK.set_output(LOW);     //????????
        Temp <<= 1;     //????????
        if( this->MISO.get_input() )
            Temp++;     //????????????????????????????????
        delay_us(1);
        this->SCK.set_output(HIGH);     //????????
    }
    return (Temp);     //????????
}
/* CPOL = 1, CPHA = 1, MSB first */
uint8_t SPI_S::SOFT_SPI_RW_MODE3( uint8_t write_dat )
{
    uint8_t read_dat=0;
    for(uint8_t i = 0; i < 8; i++ )
    {
        this->SCK.set_output(LOW);
        if( write_dat & 0x80 )
            this->MOSI.set_output(HIGH);
        else
            this->MOSI.set_output(LOW);
        write_dat <<= 1;
        delay_us(1);
        this->SCK.set_output(HIGH);
        read_dat <<= 1;
        if(  this->MISO.get_input()  )
            read_dat++;
        delay_us(1);
        //asm("nop");//__asm__("nop");
    }
    return read_dat;
}

void SPI_S::config(CP RWmode) {
    this->RW_mode=RWmode;
}

void SPI_S::SetSpeed(uint8_t SPI_BaudRatePrescaler) {
    this->BaudRate=SPI_BaudRatePrescaler;
}

uint16_t SPI_S::ReadWriteDATA(uint16_t TxData) {
    uint16_t data;
    switch (this->RW_mode) {
        case 0:data= this->SOFT_SPI_RW_MODE0(TxData);break;
        case 1:data= this->SOFT_SPI_RW_MODE1(TxData);break;
        case 2:data= this->SOFT_SPI_RW_MODE2(TxData);break;
        case 3:data= this->SOFT_SPI_RW_MODE3(TxData);break;
        default:data= 0xffff;
    }
    return data;
}









