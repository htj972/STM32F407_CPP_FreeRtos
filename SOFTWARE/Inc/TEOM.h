/**
* @Author kokirika
* @Name TEOM
* @Date 2022-12-27
**/

#ifndef KOKIRIKA_TEOM_H
#define KOKIRIKA_TEOM_H


#include "Out_In_Put.h"
#include "Tim_Capture.h"
#include "IIC.h"
#include "FM24Cxx.h"
#include "SPI.h"
#include "MAX31865.h"
#include "Temp_ctrl.h"
#include "PWM.h"
#include "SMI.h"

#define HARD_version 1.0f

#define Samp_Long_mode  0
#define Samp_Short_mode 1

typedef struct K_TEOM_data{
    float version;          //�汾��
    float l_Samp;           //������ʱ�� Ĭ��
    float s_Samp;           //�̲���ʱ�� Ĭ��
    float Samp_mode;        //����ģʽ
    float Samp_flag;        //�״β���
    float Samp_num;         //����������
    float Samp_TL;          //������ʱ��
    float Samp_TS;          //�̲���ʱ��
    float Samp_H;           //���� ʱ
    float Samp_M;           //���� ��
    float Work_TL;          //��ʱ�乤��
    float Work_TS;          //��ʱ�乤��
    float dis_time;         //��Ļ����ʱ��
    float dis_light;        //��Ļ����

    float stiffness;        //��ϵ��
    float coefficient;      //ѹ��ϵ��

    float Concentration[10];//Ũ��
    float Work_time[10];    //����ʱ��

    float temp_dif[5];
}TEOM_data;

typedef union K_POWER_TEOM_DATA_{
    TEOM_data to_float;
    uint8_t   to_u8t[sizeof(TEOM_data)]{};
    K_POWER_TEOM_DATA_() {
        this->to_float.version=HARD_version;
        this->to_float.l_Samp=8;
        this->to_float.s_Samp=15;
        this->to_float.Work_TL=8;
        this->to_float.Work_TS=0.5;
        this->to_float.dis_time=15;
        this->to_float.dis_light=20;
    };
}POWER_TEOM_DATA;

class PDATA_Storage:protected Software_IIC, public FM24Cxx
{
public:
    PDATA_Storage(uint8_t Pin_Scl, uint8_t Pin_Sda){
        Software_IIC::init(Pin_Scl,Pin_Sda);
    }
    PDATA_Storage()=default;

    virtual void inital();
};

class TEOM {
private:

protected:
    _OutPut_ EN;
    Tim_Capture TimxC;
public:
    float frqdata[256]{};
    POWER_TEOM_DATA DATA;
    TEOM(GPIO_Pin ENx,TIM_TypeDef* TIMx,uint8_t channel);
    TEOM()=default;
    void init(GPIO_Pin ENx,TIM_TypeDef* TIMx,uint8_t channel);
    void turn_on();
    void turn_off();

    float get_frq();
};

class pressure_dif:public SMI,private Software_IIC,private Kalman{
private:
    float data[40]{};
    uint8_t num;
public:
    pressure_dif(uint8_t scl,uint8_t sda) : Kalman(){
        Software_IIC::init(scl,sda);
        SMI::init(this,34473.785,-34473.785);
        Kalman::init(20,5);
        this->num=0;
    }
    void read(){
        this->data[this->num++]=Kalman::Filter(this->get_pres());
        if(this->num>=40)this->num=0;
    };
    float get_value(){
        float sum=0;
        for(float &ii : this->data)
            sum+=ii;
        return sum/40;
    };
};

class TEOM_TEMP:public PWM_H{
private:
    GPIO_Pin CS_P[5]{};
protected:
    SPI *spix;
public:
    MAX31865 temp_sensor[5];
    Temp_ctrl CTRLT[5];
    TEOM_TEMP(SPI * SPIx,GPIO_Pin CS1,GPIO_Pin CS2,GPIO_Pin CS3,
              GPIO_Pin CS4,GPIO_Pin CS5,TIM_TypeDef* TIMx,uint32_t FRQ);
    void initial();
};

class TEOM_Machine:public TEOM, private PDATA_Storage{
public:
    TEOM_Machine(uint8_t Pin_Scl, uint8_t Pin_Sda,GPIO_Pin ENx,TIM_TypeDef* TIMx,uint8_t channel){
        PDATA_Storage::Software_IIC::init(Pin_Scl,Pin_Sda);
        TEOM::init(ENx,TIMx,channel);
    }
    pressure_dif *Pre_link{};
    void Link_PRE(pressure_dif *Pre_linkx);
    void inital() override;
    void data_save(float *data,float value);
    void data_save();
};


#endif //KOKIRIKA_TEOM_H
