/**
* @Author kokirika
* @Name Project
* @Date 2022-12-23
**/

#include "SMI.h"

SMI::SMI(Software_IIC *IICx, uint8_t Address):iicx(IICx),Addr(Address){

}

float SMI::get_sensor_temp() {
    float P1=1638.3f;       /* P1= 10% * 16383 ?B type*/
    float P2=14744.7f;      /* P2= 90% * 16383 ?B type*/
    uint16_t NPA;
    uint8_t NPA_buf[4];
    this->iicx->Queue_star();
    this->iicx->Read_Data(Addr+1,NPA_buf,4);
    this->iicx->Queue_end();
    NPA  = (NPA_buf[2]  << 3) | (NPA_buf[3] >> 5);
    return (float)NPA  * 20.0f / 2048.0f - 50.0f;
}

float SMI::get_sensor_pres() {
    float P1=1638.3f;       /* P1= 10% * 16383 ?B type*/
    float P2=14744.7f;      /* P2= 90% * 16383 ?B type*/
    uint16_t NPA;
    uint8_t NPA_buf[4];
    this->iicx->Queue_star();
    this->iicx->Read_Data(Addr+1,NPA_buf,4);
    this->iicx->Queue_end();
    NPA  = ((NPA_buf[0] & 0x3F) << 8) | NPA_buf[1];

    return (((float)NPA)  - P1) * (34473.785f    - (-34473.785f))   / (P2 - P1) + (-34473.785f);       //-5~40cmH20
}

bool SMI::get_sensor_temp_pres(float *Temp, float *Pres) {
    float P1=1638.3f;       /* P1= 10% * 16383 ?B type*/
    float P2=14744.7f;      /* P2= 90% * 16383 ?B type*/
    uint16_t NPA;
    uint8_t NPA_buf[4];
    this->iicx->Queue_star();
    this->iicx->Read_Data(Addr+1,NPA_buf,4);
    this->iicx->Queue_end();
    NPA  = ((NPA_buf[0] & 0x3F) << 8) | NPA_buf[1];

    *Pres = (((float)NPA)  - P1) * (34473.785f    - (-34473.785f))   / (P2 - P1) + (-34473.785f);       //-5~40cmH20

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
    return true;
}


