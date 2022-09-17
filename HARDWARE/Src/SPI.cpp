/**
* @Author kokirika
* @Name SPI
* @Date 2022-09-17
**/

#include "SPI.h"

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

    this->SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    this->SPI_InitStructure.SPI_Mode = SPI_Mode_Master;		//����SPI����ģʽ:����Ϊ��SPI
    this->SPI_InitStructure.SPI_DataSize = DataSize;		//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    this->SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    this->SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;	//����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    this->SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    this->SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler;		//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    this->SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    this->SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
    SPI_Init(this->SPIx, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(this->SPIx, ENABLE); //ʹ��SPI����
}

void SPI::SetSpeed(uint8_t SPI_BaudRatePrescaler)
{
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));
    this->SPIx->CR1&=0XFFC7;
    this->SPIx->CR1|=SPI_BaudRatePrescaler;	//����SPI1�ٶ�
    SPI_Cmd(this->SPIx,ENABLE);
}

uint16_t SPI::ReadWriteDATA(uint16_t TxData)
{
    while (SPI_I2S_GetFlagStatus(this->SPIx, SPI_I2S_FLAG_TXE) == RESET){}//�ȴ���������
    SPI_I2S_SendData(this->SPIx, TxData); //ͨ������SPIx����һ��byte  ����
    while (SPI_I2S_GetFlagStatus(this->SPIx, SPI_I2S_FLAG_RXNE) == RESET){} //�ȴ�������һ��byte
    return SPI_I2S_ReceiveData(this->SPIx); //����ͨ��SPIx������յ�����
}


