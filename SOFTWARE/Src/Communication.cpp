/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/


#include "Communication.h"

#include <utility>

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

void Communication::data_set(uint8_t id,uint16_t address,uint16_t data) {
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            this->datax[ii][0]  = data;
            this->addx[ii][0] = address;
            this->addx[ii][1] = 1;
            this->addx[ii][2] = id;
            this->queue_flag[ii]= true;
            break;
        }
    }
}

void Communication::data_set(uint8_t id,uint16_t address,const uint16_t* data,uint8_t num){
    for(uint8_t ii=0;ii<COM_queue_num;ii++){
        if(!this->queue_flag[ii]){
            // this->datax[ii][0]  = *data;
            // this->datax[ii][1]  = *(data+1);
            for (uint8_t jj=0;jj<num;jj++)
            {
                this->datax[ii][jj]  = data[jj];
            }
            this->addx[ii][0] = address;
            this->addx[ii][1] = num;
            this->addx[ii][2] = id;
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
        this->set_id(this->addx[ii][2]);
        this->modbus_16_send(this->addx[ii][0], this->datax[ii], this->addx[ii][1]);
        this->queue_flag[ii] = false;
    }
    else {
        this->sensordata_sync();
    }
}
void Communication::sensordata_sync() {
    static uint16_t times=0;
    times++;
    if(times>100)
    {
        queue_len=5;
        times=0;
    }
    switch (queue_num)
    {
    case 0:
        this->set_id(1);
        if (this->modbus_03_send(0x101, 6) == modbus::modbus_success) {
            uint16_t *data =this->data_BUS.to_u16;
            //this->env.water_pump_state = *data;
            //this->env.fertilizer_pump_state = *(data+1);
            data+=2;
            this->env.water_flow_clear = *(data+2);
            this->env.fertilizer_flow_clear = *(data+3);
            this->env.water_supply_reset = *(data+4);
            this->env.fertilizer_reset = *(data+5);
        }
    break;
    case 1:
        this->set_id(1);
        if(this->modbus_03_send(0x201, 24) == modbus::modbus_success) {
            uint16_t *data = this->data_BUS.to_u16;
//            this->env.PLC_run_state = *data;//201
            this->env.pressure =  (float)(*(data))/1000.0f;//202
            if(this->env.pressure>=0.06)
                this->env.water_pump_state|=0x02;
            else
                this->env.water_pump_state&=~0x02;
            uint32_t flow_t=0;
            //data [7]-[8] 为流量计流量float 4字节小端
            flow_t = *(data + 7) + (*(data + 8)<<16);
            //使用unoin 指针转换为floato
            union {
                uint32_t u32;
                float f;
            } flow{};
            flow.u32 = flow_t;
            this->env.fertilizer_flow = flow.f;//208
            flow_t = *(data + 9) + (*(data + 10)<<16);
            flow.u32 = flow_t;
//            if(this->env.fertilizer_flow>0.25)
//                this->env.fertilizer_pump_state=1;
//            else
//                this->env.fertilizer_pump_state=0;
            this->env.fertilizer_flow_total = flow.f;//20A
            this->env.water_pump_inverter_state = *(data + 11);//20C
            this->env.water_pump_inverter_fault_code = *(data + 13);//20E
            this->env.fertilizer_pump_inverter_state = *(data + 20);//215
            this->env.fertilizer_pump_inverter_fault_code = *(data + 21);//216
            if(this->env.fertilizer_pump_inverter_fault_code>3)
                this->env.fertilizer_pump_state=1;
            else
                this->env.fertilizer_pump_state=0;

        }
    break;
//    case 2:
//        this->set_id(1);
//        if(this->modbus_03_send(0x301, 23) == modbus::modbus_success) {
//            uint16_t *data = this->data_BUS.to_u16;
////            this->env.PLC_station_number = *data;//301
////            this->env.communication_rate = *(data + 1);//302
////            this->env.water_pump_working_mode = *(data + 2);//303
////            this->env.water_pump_working_parameter = *(data + 3);//304
////            this->env.fertilizer_pump_working_mode = *(data + 6);//307
////            this->env.fertilizer_pump_working_parameter = *(data + 7);//308
////            this->env.flowmeter1_caliber = *(data + 9);//30A
////            this->env.flowmeter1_pulse = *(data + 10);//30B
////            this->env.flowmeter2_caliber = *(data + 11);//30C
////            this->env.flowmeter2_pulse = *(data + 12);//30D
////            //读取启动流量0x316 0x317 数据
////            this->env.water_flow_last = *(data + 21) + (*(data + 22)<<16);
//
//        }
//    break;
    case 2:
        this->set_id(2);
        if (this->modbus_03_send(0x0, 10) == modbus::modbus_success) {
            uint16_t *data =this->data_BUS.to_u16;
            union {
                uint32_t u32;
                float f;
            } flow{};
            flow.u32 = *data + (*(data + 1)<<16);
            //保留>0的值
            if(flow.f<0){
                flow.f=0;
            }
            //求五位均值
            float sum=0;
            for(uint8_t i=4;i>0;i--){
                this->flow_sum[i]=this->flow_sum[i-1];
                sum+=this->flow_sum[i];
            }
            this->flow_sum[0]=flow.f;
            sum+=this->flow_sum[0];
            this->env.water_flow = sum/5.0f; //平均流量
//            this->env.water_flow = (this->env.water_flow*5+flow.f)/6;
            if(this->env.water_flow>10)
                this->env.water_pump_state|=0x01;
            else
                this->env.water_pump_state&=~0x01;
            flow.u32 = *(data + 8) + (*(data + 9)<<16);
            //this->env.water_flow_total = float(flow.u32-this->env.water_flow_last);
            this->env.water_flow_total = flow.u32/10.0f;
        }
        break;
//    case 3:
//        this->set_id(1);
//        if(this->modbus_03_send(0x501, 1) == modbus::modbus_success) {
//            uint16_t *data = this->data_BUS.to_u16;
//            this->env.firmware_version = *data;
//        }
//    break;
//    case 3:
//        this->set_id(1);
//        if(!get_version_flag) {
//            if (this->modbus_03_send(0x511, 8) == modbus::modbus_success) {
//                uint16_t *data = this->data_BUS.to_u16;
//                uint16_t dstr = *data;
//                this->firmware_SNSTR.push_back((dstr >> 8) & 0xff);
//                this->firmware_SNSTR.push_back(dstr & 0xff);
//                dstr = *(data + 1);
//                this->firmware_SNSTR.push_back((dstr >> 8) & 0xff);
//                this->firmware_SNSTR.push_back(dstr & 0xff);
//                dstr = *(data + 2);
//                this->firmware_SNSTR.push_back((dstr >> 8) & 0xff);
//                this->firmware_SNSTR.push_back(dstr & 0xff);
//                dstr = *(data + 3);
//                this->firmware_SNSTR.push_back((dstr >> 8) & 0xff);
//                this->firmware_SNSTR.push_back(dstr & 0xff);
//                dstr = *(data + 4);
//                this->firmware_SNSTR.push_back((dstr >> 8) & 0xff);
//                this->firmware_SNSTR.push_back(dstr & 0xff);
//                dstr = *(data + 5);
//                this->firmware_SNSTR.push_back((dstr >> 8) & 0xff);
//                this->firmware_SNSTR.push_back(dstr & 0xff);
//                dstr = *(data + 6);
//                this->firmware_SNSTR.push_back((dstr >> 8) & 0xff);
//                this->firmware_SNSTR.push_back(dstr & 0xff);
//                dstr = *(data + 7);
//                this->firmware_SNSTR.push_back((dstr >> 8) & 0xff);
//            }
//            if (this->firmware_SNSTR[0]==' ' || this->firmware_SNSTR.length() < 16) {
//                typedef struct {
//                    uint32_t id[3]; // 存储96位ID
//                } ChipID;
//                ChipID chipid = {0};
//                chipid.id[0] = *(__I uint32_t *)(0x1FFF7A10);
//                chipid.id[1] = *(__I uint32_t *)(0x1FFF7A14);
//                chipid.id[2] = *(__I uint32_t *)(0x1FFF7A18);
//                firmware_SNSTR.clear();
//                sprintf((char*)firmware_SNSTR.data(), "%lu%lu%lu", chipid.id[2], chipid.id[1], chipid.id[0]);
//            }
//            this->env.firmware_version = 2;
//            this->env.PH = 67;
//            this->env.EC = 15;
//            get_version_flag = true;
//        }
//    break;
    case 3:
        this->set_id(3);
        if(this->modbus_03_send(00, 1) == modbus::modbus_success) {
            uint16_t *data = this->data_BUS.to_u16;
            this->env.EC=*data;
        }
    break;

    case 4:
        this->set_id(4);
        if(this->modbus_03_send(00, 1) == modbus::modbus_success) {
            uint16_t *data = this->data_BUS.to_u16;
            this->env.PH=*data;
            queue_len=3;
        }
    default:
        queue_num=0;
    break;
    }
    queue_num++;
    if(queue_num>=queue_len){queue_num=0;}
}

// string Communication::data_to_json() const {
//     string buf;
//     buf.append("{\"water_flowrate\":"+to_string(this->env.water_flow)+",");
//     buf.append("\"water_totalflow\":"+to_string(this->env.water_flow_total)+",");
//     buf.append("\"pressrate\":"+to_string(this->env.pressure)+",");
//     buf.append("\"fertilizer_flowrate\":"+to_string(this->env.fertilizer_flow)+",");
//     buf.append("\"fertilizer_totalflow\":"+to_string(this->env.water_flow_total)+",");
//     buf.append("\"EC\":"+to_string(this->env.EC/10.0)+",");
//     buf.append("\"PH\":"+to_string(this->env.PH/10.0)+",");
//     buf.append("\"water_openstatus\":"+to_string(this->env.water_pump_inverter_state)+",");
//     buf.append("\"water_workstatus\":"+to_string(this->env.water_pump_inverter_fault_code)+",");
//     buf.append("\"fertilizer_openstatus\":"+to_string(this->env.fertilizer_pump_inverter_state)+",");
//     buf.append("\"fertilizer_workstatus\":"+to_string(this->env.fertilizer_pump_inverter_fault_code)+",");
//     buf.append("\"firmware_version\":"+to_string(this->env.firmware_version)+",");
//     buf.append("\"firmware_SN\":"+to_string(this->env.firmware_SN)+",");
//     //判断fertilizer_flow_total是否大于500
//     buf.append("\"warn_fertilizer\":"+to_string((this->env.fertilizer_flow_total>=500)?1:0)+",");
//     buf.append("\"water_run_time\":"+to_string(this->env.water_run_time)+",");
//     buf.append("\"fertilizer_run_time\":"+to_string(this->env.fertilizer_run_time)+"}");
//     return buf;
// }

string Communication::data_to_json(const string& db,const string& str) const {
    string buf;
    buf.append("{\"water_flowrate\":"+to_string(this->env.water_flow)+",");
    buf.append("\"water_totalflow\":"+to_string(this->env.water_flow_total)+",");
    buf.append("\"pressrate\":"+to_string(this->env.pressure)+",");
    buf.append("\"fertilizer_flowrate\":"+to_string(this->env.fertilizer_flow)+",");
    buf.append("\"fertilizer_totalflow\":"+to_string(this->env.fertilizer_flow_total)+",");
    buf.append("\"EC\":"+to_string(this->env.EC/10.0)+",");
    buf.append("\"PH\":"+to_string(this->env.PH/100.0)+",");
    buf.append("\"water_openstatus\":"+to_string(this->env.water_pump_state>0)+",");
    buf.append("\"water_workstatus\":"+to_string(this->env.water_pump_inverter_fault_code)+",");
    buf.append("\"fertilizer_openstatus\":"+to_string(this->env.fertilizer_pump_state)+",");
    buf.append("\"fertilizer_workstatus\":"+to_string(this->env.fertilizer_pump_inverter_fault_code)+",");
    buf.append("\"firmware_version\":"+to_string(this->env.firmware_version)+",");
    buf.append(R"("firmware_SN":")"+firmware_SNSTR+"\",");
    //判断fertilizer_flow_total是否大于500
    buf.append("\"warn_fertilizer\":"+to_string((this->env.fertilizer_flow_total>=500)?1:0)+",");
    buf.append("\"water_run_time\":"+to_string(this->env.water_run_time)+",");
    buf.append("\"fertilizer_run_time\":"+to_string(this->env.fertilizer_run_time)+",");
    buf.append("\"db\":"+db+",");
    buf.append(R"("call":")"+str+"\"}");

    //结果示例
    //{"water_flowrate":0.0,"water_totalflow":0.0,"pressrate":0.0,"fertilizer_flowrate":0.0,"fertilizer_totalflow":0.0,"EC":0.0,"PH":0.0,"water_openstatus":1,"water_workstatus":1,"fertilizer_openstatus":1,"fertilizer_workstatus":1,"firmware_version":1,"firmware_SN":1,"warn_fertilizer":1}
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
    temp[1]=(uint16_t)(Press*100);
    this->data_set(1,0x303, temp,2);
    temp[1]=(uint16_t)(Flow*1000);
    this->data_set(1,0x307, temp,2);

}

void Communication::send_fertilizerpump(uint16_t state) {
    uint16_t temp[6];
    temp[0]=state;//肥泵状态 102
    temp[1]=0;//水清零 103
    temp[2]=1;//肥清零 104
    temp[3]=1;//水异常 105
    temp[4]=1;//肥异常 106
    temp[5]=state;//搅拌 107
    this->data_set(1,0x102, temp,6);
    this->env.fertilizer_run_time=0;
    // this->env.fertilizer_pump_state=state;
}

void Communication::send_waterpump(uint16_t state) {
    this->data_set(1,0x101, state);
    this->env.water_run_time=0;
    this->env.water_flow_total=0;
//    uint16_t temp[2];
//    temp[0]= this->env.water_flow_last&0x00ff;
//    temp[1]= (this->env.water_flow_last>>16)&0x00ff;
//    this->data_set(1,0x316,temp,2);
    // this->env.water_pump_state=state;
}

void Communication::set_id_PHEC(string id, uint16_t PH, uint16_t EC) {
    if(id.empty())
    {
        typedef struct {
            uint32_t id[3]; // 存储96位ID
        } ChipID;
        ChipID chipid = {0};
        chipid.id[0] = *(__I uint32_t *)(0x1FFF7A10);
        chipid.id[1] = *(__I uint32_t *)(0x1FFF7A14);
        chipid.id[2] = *(__I uint32_t *)(0x1FFF7A18);
        id.clear();
        id.append(std::to_string(chipid.id[2]));
        id.append(std::to_string(chipid.id[1]));
        id.append(std::to_string(chipid.id[0]));
    }
    this->firmware_SNSTR = std::move(id);
    this->env.PH = PH;
    this->env.EC = EC;
}



