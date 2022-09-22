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

    this->ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;//����ģʽ
    this->ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;//���������׶�֮����ӳ�5��ʱ��
    this->ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled; //DMAʧ��
    this->ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;//Ԥ��Ƶ4��Ƶ��ADCCLK=PCLK2/4=84/4=21Mhz,ADCʱ����ò�Ҫ����36Mhz
    ADC_CommonInit(&this->ADC_CommonInitStructure);//��ʼ��

    this->ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;//12λģʽ
    this->ADC_InitStructure.ADC_ScanConvMode = DISABLE;//��ɨ��ģʽ
    this->ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;//�ر�����ת��
    this->ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//��ֹ������⣬ʹ���������
    this->ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//�Ҷ���
    this->ADC_InitStructure.ADC_NbrOfConversion = 1;//1��ת���ڹ��������� Ҳ����ֻת����������1
    ADC_Init(this->ADCx, &this->ADC_InitStructure);//ADC��ʼ��

    ADC_Cmd(this->ADCx, ENABLE);//����ADת����
}

uint16_t _ADC_::get_value() {
    ADC_RegularChannelConfig(this->ADCx,this->channel,1,ADC_SampleTime_480Cycles );	//ADC1,ADCͨ��,480������,��߲���ʱ�������߾�ȷ��
    ADC_SoftwareStartConv(this->ADCx);		//ʹ��ָ����ADC1�����ת����������
    while(!ADC_GetFlagStatus(this->ADCx, ADC_FLAG_EOC ));//�ȴ�ת������
    return ADC_GetConversionValue(this->ADCx);	//�������һ��ADC1�������ת�����
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






