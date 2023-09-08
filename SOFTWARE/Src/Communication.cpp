/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/


#include "Communication.h"

Communication::Communication(USART_TypeDef *USARTx, uint8_t DE, TIM_TypeDef *TIMx, uint16_t frq) {
    RS485::init(USARTx,DE);
//    RS485::config(PinTx,PinRx);
    Timer::init(TIMx,10000/frq,8400,true);
    modbus::init(this,modbus::HOST);
    this->initial();
    for (bool & ii : this->queue_flag) {
        ii= false;
    }
}

void Communication::initial() {
    modbus::Link_UART_CALLback();
    modbus::Link_TIMER_CALLback(this);
    modbus::config(this->data_BUS.to_u16,sizeof(this->data_BUS)/2-1);

}

uint16_t Communication::find_address(const float *data) {
    return (uint16_t)(data - (float *)&this->data_BUS.to_float)*2;
}

void Communication::data_set(float *data,float value) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datax[ii]  = data;
            this->valuex[ii] = value;
            this->queue_flag[ii]= true;
            break;
        }
    }
}

void Communication::data_sync() {
    uint8_t ii=0;
    for(;ii<COM_queue_num;ii++){
        if(this->queue_flag[ii])
            break;
    }
    if(ii<COM_queue_num){
        union {
            float _f;
            uint16_t _u16[2];
        }temp{
                ._f=this->valuex[ii],
        };
        this->modbus_16_send(this->find_address(this->datax[ii]),
                             temp._u16,2);
        this->queue_flag[ii]= false;
    }
    else {
        this->sensordata_sync();
    }
}

void Communication::sensordata_sync() {
    this->set_id(0x0a);//五合一传感器
    if(this->modbus_03_send(0,5)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        if(*data<0x8000){
            this->env.temp=((float)*data/10.0f);
        } else{
            this->env.temp=((float)(0xffff-*data+1))/10.0f;
        }
        data++;
        this->env.humi=((float)*data/10.0f);
        data++;
        this->env.press=((float)*data/10.0f);
        data++;
        this->env.co2=((float)*data);
        data++;
        this->env.light=((float)*data*10);
    }
    this->set_id(0x0b);//PM传感器
    if(this->modbus_03_send(0,4)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        this->env.pm1=((float)*data);
        data++;
        this->env.pm10=((float)*data);
        data++;
        this->env.pm25=((float)*data);
        data++;
        this->env.pm30=((float)*data);
    }
    this->set_id(0x0c);//风向传感器
    if(this->modbus_03_send(0,1)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        this->env.wind_dir=((float)*data);
    }
    this->set_id(0x0d);//风速传感器
    if(this->modbus_03_send(0,1)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        this->env.wind_speed=((float)*data/100.0f);
    }
    this->set_id(0x0e);//总辐射传感器
    if(this->modbus_03_send(0,1)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        this->env.solar_rad=((float)*data);
    }
    this->set_id(0x0f);//土壤传感器
    if(this->modbus_03_send(0,4)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        if(*data<0x8000){
            this->env.soil_temp=((float)*data/10.0f);
        } else{
            this->env.soil_temp=((float)(0xffff-*data+1))/10.0f;
        }
        data++;
        this->env.soil_humi=((float)*data/10.0f);
        data++;
        this->env.soil_ec=((float)*data/100.0f);
        data++;
        this->env.soil_salt=((float)*data);
    }
}

string Communication::data_to_json() const {
    string buf;

    buf.append(R"({"空气温度":)"+to_string(this->env.temp)+",");
    buf.append(R"("空气湿度":)"+to_string(this->env.humi)+",");
    buf.append(R"("大气压":)"+to_string(this->env.press)+",");
    buf.append(R"("二氧化碳":)"+to_string(this->env.co2)+",");
    buf.append(R"("光照":)"+to_string(this->env.light)+",");
    buf.append(R"("PM1":)"+to_string(this->env.pm1)+",");
    buf.append(R"("PM10":)"+to_string(this->env.pm10)+",");
    buf.append(R"("PM2.5":)"+to_string(this->env.pm25)+",");
    buf.append(R"("PM30":)"+to_string(this->env.pm30)+",");
//    buf.append(R"("风向":)"+to_string(this->env.wind_dir)+",");
    //区分十六个方向 东 东南偏东 东南 南东偏南 南 南西偏南 西南 西西南 西 西西北 西北 北西偏北 北 北东偏北 东北 偏东北
    if(this->env.wind_dir<11.25f){
        buf.append(R"("风向":"东",)");
    } else if(this->env.wind_dir<33.75f){
        buf.append(R"("风向":"东南偏东",)");
    } else if(this->env.wind_dir<56.25f){
        buf.append(R"("风向":"东南",)");
    } else if(this->env.wind_dir<78.75f){
        buf.append(R"("风向":"南东偏南",)");
    } else if(this->env.wind_dir<101.25f){
        buf.append(R"("风向":"南",)");
    } else if(this->env.wind_dir<123.75f){
        buf.append(R"("风向":"南西偏南",)");
    } else if(this->env.wind_dir<146.25f){
        buf.append(R"("风向":"西南",)");
    } else if(this->env.wind_dir<168.75f){
        buf.append(R"("风向":"西西南",)");
    } else if(this->env.wind_dir<191.25f){
        buf.append(R"("风向":"西",)");
    } else if(this->env.wind_dir<213.75f){
        buf.append(R"("风向":"西西北",)");
    } else if(this->env.wind_dir<236.25f){
        buf.append(R"("风向":"西北",)");
    } else if(this->env.wind_dir<258.75f){
        buf.append(R"("风向":"北西偏北",)");
    } else if(this->env.wind_dir<281.25f){
        buf.append(R"("风向":"北",)");
    } else if(this->env.wind_dir<303.75f){
        buf.append(R"("风向":"北东偏北",)");
    }
    buf.append(R"("风速":)"+to_string(this->env.wind_speed)+",");
    buf.append(R"("总辐射":)"+to_string(this->env.solar_rad)+",");
    buf.append(R"("土壤温度":)"+to_string(this->env.soil_temp)+",");
    buf.append(R"("土壤湿度":)"+to_string(this->env.soil_humi)+",");
    buf.append(R"("土壤电导率":)"+to_string(this->env.soil_ec)+",");
    buf.append(R"("土壤盐分":)"+to_string(this->env.soil_salt)+"}");

    return buf;
}

