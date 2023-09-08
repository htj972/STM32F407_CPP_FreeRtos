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
    this->set_id(0x0a);//���һ������
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
    this->set_id(0x0b);//PM������
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
    this->set_id(0x0c);//���򴫸���
    if(this->modbus_03_send(0,1)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        this->env.wind_dir=((float)*data);
    }
    this->set_id(0x0d);//���ٴ�����
    if(this->modbus_03_send(0,1)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        this->env.wind_speed=((float)*data/100.0f);
    }
    this->set_id(0x0e);//�ܷ��䴫����
    if(this->modbus_03_send(0,1)==modbus::result::modbus_success)
    {
        uint16_t *data=this->data_BUS.to_u16;
        this->env.solar_rad=((float)*data);
    }
    this->set_id(0x0f);//����������
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

    buf.append(R"({"�����¶�":)"+to_string(this->env.temp)+",");
    buf.append(R"("����ʪ��":)"+to_string(this->env.humi)+",");
    buf.append(R"("����ѹ":)"+to_string(this->env.press)+",");
    buf.append(R"("������̼":)"+to_string(this->env.co2)+",");
    buf.append(R"("����":)"+to_string(this->env.light)+",");
    buf.append(R"("PM1":)"+to_string(this->env.pm1)+",");
    buf.append(R"("PM10":)"+to_string(this->env.pm10)+",");
    buf.append(R"("PM2.5":)"+to_string(this->env.pm25)+",");
    buf.append(R"("PM30":)"+to_string(this->env.pm30)+",");
//    buf.append(R"("����":)"+to_string(this->env.wind_dir)+",");
    //����ʮ�������� �� ����ƫ�� ���� �϶�ƫ�� �� ����ƫ�� ���� ������ �� ������ ���� ����ƫ�� �� ����ƫ�� ���� ƫ����
    if(this->env.wind_dir<11.25f){
        buf.append(R"("����":"��",)");
    } else if(this->env.wind_dir<33.75f){
        buf.append(R"("����":"����ƫ��",)");
    } else if(this->env.wind_dir<56.25f){
        buf.append(R"("����":"����",)");
    } else if(this->env.wind_dir<78.75f){
        buf.append(R"("����":"�϶�ƫ��",)");
    } else if(this->env.wind_dir<101.25f){
        buf.append(R"("����":"��",)");
    } else if(this->env.wind_dir<123.75f){
        buf.append(R"("����":"����ƫ��",)");
    } else if(this->env.wind_dir<146.25f){
        buf.append(R"("����":"����",)");
    } else if(this->env.wind_dir<168.75f){
        buf.append(R"("����":"������",)");
    } else if(this->env.wind_dir<191.25f){
        buf.append(R"("����":"��",)");
    } else if(this->env.wind_dir<213.75f){
        buf.append(R"("����":"������",)");
    } else if(this->env.wind_dir<236.25f){
        buf.append(R"("����":"����",)");
    } else if(this->env.wind_dir<258.75f){
        buf.append(R"("����":"����ƫ��",)");
    } else if(this->env.wind_dir<281.25f){
        buf.append(R"("����":"��",)");
    } else if(this->env.wind_dir<303.75f){
        buf.append(R"("����":"����ƫ��",)");
    }
    buf.append(R"("����":)"+to_string(this->env.wind_speed)+",");
    buf.append(R"("�ܷ���":)"+to_string(this->env.solar_rad)+",");
    buf.append(R"("�����¶�":)"+to_string(this->env.soil_temp)+",");
    buf.append(R"("����ʪ��":)"+to_string(this->env.soil_humi)+",");
    buf.append(R"("�����絼��":)"+to_string(this->env.soil_ec)+",");
    buf.append(R"("�����η�":)"+to_string(this->env.soil_salt)+"}");

    return buf;
}

