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

#define HARD_version 1.0f

typedef struct K_TEOM_data{
    float version;          //版本号
    float l_Samp;           //长采样时间 默认
    float s_Samp;           //短采样时间 默认
    float Samp_flag;        //首次采样
    float Samp_num;         //采样点数量
    float Samp_TL;          //长采样时间
    float Samp_TS;          //短采样时间
    float Samp_H;           //采样 时
    float Samp_M;           //采样 分
    float Work_TL;          //长时间工作
    float Work_TS;          //短时间工作

}TEOM_data;

typedef union K_POWER_TEOM_DATA_{
    TEOM_data to_float;
    uint8_t   to_u8t[sizeof(TEOM_data)]{};
    K_POWER_TEOM_DATA_() {
        this->to_float.version=HARD_version;
        this->to_float.l_Samp=8;
        this->to_float.s_Samp=15;
        this->to_float.Work_TL=8;
        this->to_float.Work_TS=20;
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
    POWER_TEOM_DATA DATA;
    TEOM(GPIO_Pin ENx,TIM_TypeDef* TIMx,uint8_t channel);
    TEOM()=default;
    void init(GPIO_Pin ENx,TIM_TypeDef* TIMx,uint8_t channel);
    void turn_on();
    void turn_off();

    float get_frq();
};

class TEOM_Machine:public TEOM, private PDATA_Storage{
public:
    TEOM_Machine(uint8_t Pin_Scl, uint8_t Pin_Sda,GPIO_Pin ENx,TIM_TypeDef* TIMx,uint8_t channel){
        PDATA_Storage::Software_IIC::init(Pin_Scl,Pin_Sda);
        TEOM::init(ENx,TIMx,channel);
    }
    void inital() override;
};


#endif //KOKIRIKA_TEOM_H
