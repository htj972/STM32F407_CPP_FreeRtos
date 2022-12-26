/**
* @Author kokirika
* @Name SMI
* @Date 2022-12-23
**/

#include "SMI.h"

SMI::SMI(Software_IIC *IICx,float pmax,float pmin,uint8_t Address)
        :iicx(IICx),Addr(Address),max(pmax),min(pmin){

}

void SMI::init(Software_IIC *IICx, float pmax, float pmin, uint8_t Address) {
    this->iicx=IICx;
    this->Addr=Address;
    this->max=pmax;
    this->min=pmin;
}

void SMI::get_temp_pres_value(float *Temp, float *Pres) {
    float P1=1638.3f;       /* P1= 10% * 16383 14B type*/
    float P2=14744.7f;      /* P2= 90% * 16383 14B type*/
    uint16_t NPA;
    uint8_t NPA_buf[4];
    this->iicx->Queue_star();
    this->iicx->Read_Data(Addr+1,NPA_buf,4);
    this->iicx->Queue_end();
    NPA  = ((NPA_buf[0] & 0x3F) << 8) | NPA_buf[1];

    *Pres = (((float)NPA)  - P1) * (this->max - (this->min)) / (P2 - P1) + (this->min);       //-5~40cmH20

    /***********计算压力***********1毫米水柱（mmH2O）=9.80665帕（Pa）****1psi=6.894757kPa****************/
    //  Pressure_LYA = (((float)NPA)  - P1) * (3922.66    - (-490.3325))   / (P2 - P1) + (-490.3325);       //-5~40cmH20
    //    YaLy[0] = (((float)NPA_Pd)  - P1) * (3922.66    - (-490.3325))   / (P2 - P1) + (-490.3325);       //-5~40cmH20
    //    YaLy[1] = (((float)NPA_Pt)  - P1) * (34.473785  - (-34.473785))  / (P2 - P1) + (-34.473785);     //-5psi~5psi
    //    YaLy[2] = (((float)NPA_CLY) - P1) * (3922.66    - (-490.3325))   / (P2 - P1) + (-490.3325);      //-5~40cmH20
    //    YaLy[3] = (((float)NPA_Pr)  - P1) * (103.421355 - (-103.421355)) / (P2 - P1) + (-103.421355);    //-15psi~15psi
    //    YaLy[4] = (((float)NPA_QLY) - P1) * (3922.66    - (-490.3325))   / (P2 - P1) + (-490.3325);      //-5~40cmH20
    //
    /************温度计算****************/
    NPA  = (NPA_buf[2]  << 3) | (NPA_buf[3] >> 5);
    *Temp = (float)NPA  * 20.0f / 2048.0f - 50.0f;
}

float SMI::get_sensor_temp() {
    float GT,GP;
    this->get_temp_pres_value(&GT,&GP);
    return GT;
}

float SMI::get_sensor_pres() {
    float GT,GP;
    this->get_temp_pres_value(&GT,&GP);
    return GP;
}

bool SMI::get_sensor_temp_pres(float *Temp, float *Pres) {
    this->get_temp_pres_value(Temp,Pres);
    return true;
}





