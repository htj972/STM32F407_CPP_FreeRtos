/**
* @Author kokirika
* @Name DAC
* @Date 2022-09-22
**/

#include "DAC.h"

void _DAC_::default_config() {
    if(this->channel==0)
    {
        this->GPIO.set_PuPD(GPIO_PuPd_DOWN);
        this->GPIO.init(GPIOA4, GPIO_Mode_AN);//��ʼ��
    }
    else
    {
        this->GPIO.set_PuPD(GPIO_PuPd_DOWN);
        this->GPIO.init(GPIOA5, GPIO_Mode_AN);//��ʼ��
    }
}

void _DAC_::init(uint8_t channelx) {
    this->channel=channelx;
    this->default_config();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);//ʹ��DACʱ��

    DAC_InitType.DAC_Trigger=DAC_Trigger_None;	//��ʹ�ô������� TEN1=0
    DAC_InitType.DAC_WaveGeneration=DAC_WaveGeneration_None;//��ʹ�ò��η���
    DAC_InitType.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bit0;//���Ρ���ֵ����
    DAC_InitType.DAC_OutputBuffer=DAC_OutputBuffer_Disable ;	//DAC1�������ر� BOFF1=1
    if(this->channel==0) {
        DAC_Init(DAC_Channel_1, &DAC_InitType);     //��ʼ��DACͨ��1
        DAC_Cmd(DAC_Channel_1, ENABLE);  //ʹ��DACͨ��1
        DAC_SetChannel1Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
    }
    else{
        DAC_Init(DAC_Channel_2, &DAC_InitType);     //��ʼ��DACͨ��1
        DAC_Cmd(DAC_Channel_2, ENABLE);  //ʹ��DACͨ��1
        DAC_SetChannel2Data(DAC_Align_12b_R, 0);  //12λ�Ҷ������ݸ�ʽ����DACֵ
    }
}

void _DAC_::set_value(uint16_t value) const {
    if(this->channel==0)
        DAC_SetChannel1Data(DAC_Align_12b_R,value);//12λ�Ҷ������ݸ�ʽ����DACֵ
    else
        DAC_SetChannel2Data(DAC_Align_12b_R,value);//12λ�Ҷ������ݸ�ʽ����DACֵ
}

void _DAC_::set_vol_mv(uint16_t mv) const {
    double temp=mv;
    temp/=1000;
    temp=temp*4096/3.3;
    this->set_value((uint16_t)temp);
}

