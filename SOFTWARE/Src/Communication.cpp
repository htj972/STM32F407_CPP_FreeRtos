/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/


#include "Communication.h"

Communication::Communication(USART_TypeDef *USARTx, uint8_t DE, TIM_TypeDef *TIMx, uint16_t frq) {
    RS485::init(USARTx,DE,9600);
    RS485::config(GPIOD8,GPIOD9);
    RS485::set_delay_times(2);
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

uint16_t Communication::find_address(const uint16_t *data) {
    return (uint16_t)(data - this->data_BUS.to_u16)*2;
}

void Communication::data_set(uint16_t address,uint16_t data) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datax[ii][0]  = data;
            this->addx[ii][0] = address;
            this->addx[ii][1] = 1;
            this->queue_flag[ii]= true;
            break;
        }
    }
}

void Communication::data_set(uint16_t address,const uint16_t *data) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datax[ii][0]  = *data;
            this->datax[ii][1]  = *(data+1);
            this->addx[ii][0] = address;
            this->addx[ii][1] = 2;
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
        this->modbus_16_send(this->addx[ii][0], this->datax[ii], this->addx[ii][1]);
        this->queue_flag[ii] = false;
    }
    else {
        this->sensordata_sync();
    }
}
void Communication::sensordata_sync() {
    this->set_id(1);//PH
    if (this->modbus_03_send(0x101, 6) == modbus::modbus_success) {
        uint16_t *data =this->data_BUS.to_u16;
        this->env.water_pump_state = *data;
        this->env.fertilizer_pump_state = *(data+1);
        this->env.water_flow_clear = *(data+2);
        this->env.fertilizer_flow_clear = *(data+3);
        this->env.water_supply_reset = *(data+4);
        this->env.fertilizer_reset = *(data+5);
    }
    if(this->modbus_03_send(0x201, 15) == modbus::modbus_success) {
        uint16_t *data = this->data_BUS.to_u16;
        this->env.PLC_run_state = *data;
        this->env.pressure = *(float *) (data + 1);
        this->env.water_flow = *(float *) (data + 3);
        this->env.water_flow_total = *(float *) (data + 5);
        this->env.fertilizer_flow = *(float *) (data + 7);
        this->env.fertilizer_flow_total = *(float *) (data + 9);
        this->env.water_pump_inverter_state = *(data + 11);
        this->env.water_pump_inverter_fault_code = *(data + 12);
        this->env.fertilizer_pump_inverter_state = *(data + 13);
        this->env.fertilizer_pump_inverter_fault_code = *(data + 14);
    }
    if(this->modbus_03_send(0x301, 13) == modbus::modbus_success) {
        uint16_t *data = this->data_BUS.to_u16;
        this->env.PLC_station_number = *data;
        this->env.communication_rate = *(data + 1);
        this->env.water_pump_working_mode = *(data + 2);
        this->env.water_pump_working_parameter = *(data + 3);
        this->env.fertilizer_pump_working_mode = *(data + 6);
        this->env.fertilizer_pump_working_parameter = *(data + 7);
        this->env.flowmeter1_caliber = *(data + 9);
        this->env.flowmeter1_pulse = *(data + 10);
        this->env.flowmeter2_caliber = *(data + 11);
        this->env.flowmeter2_pulse = *(data + 12);
    }
    if(this->modbus_03_send(0x401, 2) == modbus::modbus_success) {
        uint16_t *data = this->data_BUS.to_u16;
        this->env.firmware_version = *data;
        this->env.firmware_SN = *(data + 1);
    }
}

string Communication::data_to_json() const {
    string buf;
    buf.append("{\"water_flowrate\":"+to_string(this->env.water_flow)+",");
    buf.append("\"water_totalflow\":"+to_string(this->env.water_flow_total)+",");
    buf.append("\"pressrate\":"+to_string(this->env.pressure)+",");
    buf.append("\"fertilizer_flowrate\":"+to_string(this->env.fertilizer_flow)+",");
    buf.append("\"fertilizer_totalflow\":"+to_string(this->env.water_flow_total)+",");
    buf.append("\"EC\":"+to_string(0)+",");
    buf.append("\"PH\":"+to_string(0)+",");
    buf.append("\"water_openstatus\":"+to_string(this->env.water_pump_inverter_state)+",");
    buf.append("\"water_workstatus\":"+to_string(this->env.water_pump_inverter_fault_code)+",");
    buf.append("\"fertilizer_openstatus\":"+to_string(this->env.fertilizer_pump_inverter_state)+",");
    buf.append("\"fertilizer_workstatus\":"+to_string(this->env.fertilizer_pump_inverter_fault_code)+",");
    buf.append("\"firmware_version\":"+to_string(this->env.firmware_version)+",");
    buf.append("\"firmware_SN\":"+to_string(this->env.firmware_SN)+",");
    //判断fertilizer_flow_total是否大于500
    buf.append("\"warn_fertilizer\":"+to_string((this->env.fertilizer_flow_total>=500)?1:0)+",");
    buf.append("\"water_run_time\":"+to_string(this->env.water_run_time)+",");
    buf.append("\"fertilizer_run_time\":"+to_string(this->env.fertilizer_run_time)+"}");
    return buf;
}

string Communication::data_to_json(const string& db,const string& str) const {
    string buf;
    buf.append("{\"water_flowrate\":"+to_string(this->env.water_flow)+",");
    buf.append("\"water_totalflow\":"+to_string(this->env.water_flow_total)+",");
    buf.append("\"pressrate\":"+to_string(this->env.pressure)+",");
    buf.append("\"fertilizer_flowrate\":"+to_string(this->env.fertilizer_flow)+",");
    buf.append("\"fertilizer_totalflow\":"+to_string(this->env.water_flow_total)+",");
    buf.append("\"EC\":"+to_string(0)+",");
    buf.append("\"PH\":"+to_string(0)+",");
    buf.append("\"water_openstatus\":"+to_string(this->env.water_pump_inverter_state)+",");
    buf.append("\"water_workstatus\":"+to_string(this->env.water_pump_inverter_fault_code)+",");
    buf.append("\"fertilizer_openstatus\":"+to_string(this->env.fertilizer_pump_inverter_state)+",");
    buf.append("\"fertilizer_workstatus\":"+to_string(this->env.fertilizer_pump_inverter_fault_code)+",");
    buf.append("\"firmware_version\":"+to_string(this->env.firmware_version)+",");
    buf.append("\"firmware_SN\":"+to_string(this->env.firmware_SN)+",");
    //判断fertilizer_flow_total是否大于500
    buf.append("\"warn_fertilizer\":"+to_string((this->env.fertilizer_flow_total>=500)?1:0)+",");
    buf.append("\"water_run_time\":"+to_string(this->env.water_run_time)+",");
    buf.append("\"fertilizer_run_time\":"+to_string(this->env.fertilizer_run_time)+",");
    buf.append("\"db\":"+db+",");
    buf.append("\"call\":"+str+"\"}");
    return buf;
}

void Communication::run_time_sync() {
    if(this->env.water_pump_state==1){
        this->env.fertilizer_run_time++;
        this->env.water_run_time++;
    }
}

void Communication::send_fertilizermach(float Press, float Flow) {
    uint16_t temp[2];
    temp[0]=1;
    temp[1]=(uint16_t)(Press*1000);
    this->data_set(0x303, temp);
    temp[1]=(uint16_t)(Flow*1000);
    this->data_set(0x307, temp);

}

void Communication::send_fertilizerpump(uint16_t state) {
    //清空肥累计流量
    this->data_set(0x104, 1);
    //控制肥泵
    this->data_set(0x102, state);
    this->env.fertilizer_run_time=0;
}

void Communication::send_waterpump(uint16_t state) {
    //清空水累计流量
    this->data_set(0x103, 1);
    //控制水泵
    this->data_set(0x101, state);
    this->env.water_run_time=0;
}



