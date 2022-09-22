/**
* @Author kokirika
* @Name ADC
* @Date 2022-09-22
**/

#include "ADC.h"
#include "delay.h"

void _ADC_::default_config() {
    this->GPIO.set_PuPD(GPIO_PuPd_NOPULL);
    switch (this->channel) {
        case 0:
            this->GPIO.init(GPIOA0,GPIO_Mode_AN);
            break;
        case 1:
            this->GPIO.init(GPIOA1,GPIO_Mode_AN);
            break;
        case 2:
            this->GPIO.init(GPIOA2,GPIO_Mode_AN);
            break;
        case 3:
            this->GPIO.init(GPIOA3,GPIO_Mode_AN);
            break;
        case 4:
            if(this->ADCx!=ADC3)
                this->GPIO.init(GPIOA4,GPIO_Mode_AN);
            else
                this->GPIO.init(GPIOF6,GPIO_Mode_AN);
            break;
        case 5:
            if(this->ADCx!=ADC3)
                this->GPIO.init(GPIOA5,GPIO_Mode_AN);
            else
                this->GPIO.init(GPIOF7,GPIO_Mode_AN);
            break;
        case 6:
            if(this->ADCx!=ADC3)
                this->GPIO.init(GPIOA6,GPIO_Mode_AN);
            else
                this->GPIO.init(GPIOF8,GPIO_Mode_AN);
            break;
        case 7:
            if(this->ADCx!=ADC3)
                this->GPIO.init(GPIOA7,GPIO_Mode_AN);
            else
                this->GPIO.init(GPIOF9,GPIO_Mode_AN);
            break;
        case 8:
            if(this->ADCx!=ADC3)
                this->GPIO.init(GPIOB0,GPIO_Mode_AN);
            else
                this->GPIO.init(GPIOF10,GPIO_Mode_AN);
            break;
        case 9:
            if(this->ADCx!=ADC3)
                this->GPIO.init(GPIOB1,GPIO_Mode_AN);
            else
                this->GPIO.init(GPIOF3,GPIO_Mode_AN);
            break;
        case 10:
            this->GPIO.init(GPIOC0,GPIO_Mode_AN);
            break;
        case 11:
            this->GPIO.init(GPIOC1,GPIO_Mode_AN);
            break;
        case 12:
            this->GPIO.init(GPIOC2,GPIO_Mode_AN);
            break;
        case 13:
            this->GPIO.init(GPIOC13,GPIO_Mode_AN);
            break;
        case 14:
            if(this->ADCx!=ADC3)
                this->GPIO.init(GPIOC4,GPIO_Mode_AN);
            else
                this->GPIO.init(GPIOF4,GPIO_Mode_AN);
            break;
        case 15:
            if(this->ADCx!=ADC3)
                this->GPIO.init(GPIOC5,GPIO_Mode_AN);
            else
                this->GPIO.init(GPIOF5,GPIO_Mode_AN);
            break;
    }
}

void _ADC_::RCC_config() {
    if(this->ADCx==ADC1) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC1,DISABLE);
    }
    else if(this->ADCx==ADC2){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);
    }

    else if(this->ADCx==ADC3){
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC3,DISABLE);
    }
}

void _ADC_::delay() const {
    delay_ms(this->delay_time);
}

void _ADC_::init(ADC_TypeDef *ADCX, uint8_t channel) {
    this->ADCx=ADCX;
    this->channel=channel;
    this->default_config();
    this->RCC_config();

    this->ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
    this->ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//两个采样阶段之间的延迟5个时钟
    this->ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMA失能
    this->ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//预分频4分频。ADCCLK=PCLK2/4=84/4=21Mhz,ADC时钟最好不要超过36Mhz
    ADC_CommonInit(&this->ADC_CommonInitStructure);//初始化

    this->ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12位模式
    this->ADC_InitStructure.ADC_ScanConvMode = DISABLE;//非扫描模式
    this->ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//关闭连续转换
    this->ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//禁止触发检测，使用软件触发
    this->ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//右对齐
    this->ADC_InitStructure.ADC_NbrOfConversion = 1;//1个转换在规则序列中 也就是只转换规则序列1
    ADC_Init(this->ADCx, &this->ADC_InitStructure);//ADC初始化

    ADC_Cmd(this->ADCx, ENABLE);//开启AD转换器
}

uint16_t _ADC_::get_value() {
    ADC_RegularChannelConfig(this->ADCx,this->channel,1,ADC_SampleTime_480Cycles );	//ADC1,ADC通道,480个周期,提高采样时间可以提高精确度
    ADC_SoftwareStartConv(this->ADCx);		//使能指定的ADC1的软件转换启动功能
    while(!ADC_GetFlagStatus(this->ADCx, ADC_FLAG_EOC ));//等待转换结束
    return ADC_GetConversionValue(this->ADCx);	//返回最近一次ADC1规则组的转换结果
}

uint16_t _ADC_::Get_Adc_Average(uint16_t times) {
    uint32_t temp_val=0;
    for(uint16_t t=0;t<times;t++)
    {
        temp_val+=this->get_value();
        this->delay();
    }
    return temp_val/times;
}

void _ADC_::set_delay_time(uint16_t delay) {
    this->delay_time=delay;
}

uint16_t _ADC_::Get_mv_value() {
    float value=this->Get_v_value();
    return (uint16_t)(value*1000);
}

float _ADC_::Get_v_value() {
    float value=this->get_value();
    return (value/4096.0f*3.3f);
}






