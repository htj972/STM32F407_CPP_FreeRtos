/**
* @Author kokirika
* @Name COMMUNICATION
* @Date 2022-12-17
**/

#ifndef KOKIRIKA_COMMUNICATION_H
#define KOKIRIKA_COMMUNICATION_H


#include "RS485.h"
#include "Timer.h"
#include "modbus.h"
#include <cstring>


#define COM_queue_num 10
class Communication: public RS485, private Timer, public modbus {
private:
    typedef struct K_USER_data{
        float sun[32];
    }USER_data;

    typedef union K_POWER_DATA_{
        USER_data to_float;
        uint16_t  to_u16[sizeof(USER_data)/2]{};
        uint8_t   to_u8t[sizeof(USER_data)];
        K_POWER_DATA_() {
            memset(this->to_u8t,0,sizeof(this->to_u8t));
        };
    }K_POWER_DATA;

    typedef struct CData{
        //0x101
        //水泵状态
        uint16_t water_pump_state;
        //肥泵状态
        uint16_t fertilizer_pump_state;
        //水流量累计清零
        uint16_t water_flow_clear;
        //肥流量累计清零
        uint16_t fertilizer_flow_clear;
        //复位供水变频异常
        uint16_t water_supply_reset;
        //复位肥变频异常
        uint16_t fertilizer_reset;
        //0x201
        //PLC运行状态
        uint16_t PLC_run_state;
        //压力值
        float pressure;
        //水实时流速
        float water_flow=0;
        //水累计流量
        float water_flow_total;
        //肥实时流速
        float fertilizer_flow;
        //肥累计流量
        float fertilizer_flow_total;
//        //水泵变频器状态
        uint16_t water_pump_inverter_state;
        //水泵变频器故障码
        uint16_t water_pump_inverter_fault_code;
        //肥泵变频器状态
        uint16_t fertilizer_pump_inverter_state;
        //肥泵变频器故障码
        uint16_t fertilizer_pump_inverter_fault_code;
        //0x301
//        //PLC站号
//        uint16_t PLC_station_number;
//        //通信速率
//        uint16_t communication_rate;
//        //水泵工作模式
//        uint16_t water_pump_working_mode;
//        //水泵工作参数
//        uint16_t water_pump_working_parameter;
//        //肥泵工作模式
//        uint16_t fertilizer_pump_working_mode;
//        //肥泵工作参数
//        uint16_t fertilizer_pump_working_parameter;
//        //流量计1-口径
//        uint16_t flowmeter1_caliber;
//        //流量计1-脉冲数
//        uint16_t flowmeter1_pulse;
//        //流量计2-口径
//        uint16_t flowmeter2_caliber;
//        //流量计2-脉冲数
//        uint16_t flowmeter2_pulse;
        //0x401
        //固件版本
        uint16_t firmware_version;
        //固件SN
//        uint16_t firmware_SN;
        //运行时间
        uint16_t water_run_time;
        uint16_t fertilizer_run_time;
        uint32_t water_flow_last;
        uint16_t PH;
        uint16_t EC;
    }CData;
    bool get_version_flag=false;
    string firmware_SNSTR;
    float flow_sum[5]={0,0,0,0,0};

    bool queue_flag[COM_queue_num]{};
    uint16_t addx[COM_queue_num][3]{};
    uint16_t datax[COM_queue_num][10]{};
    uint8_t  queue_num=0;
public:
    Communication(USART_TypeDef* USARTx,uint8_t DE,TIM_TypeDef *TIMx, uint16_t frq);
    K_POWER_DATA data_BUS;
    CData env{};
    void initial();
    uint16_t find_address(const float *data);
    uint16_t find_address(const uint16_t *data);
    void     data_set(uint8_t id,uint16_t address,uint16_t data);
    void     data_set(uint8_t id,uint16_t address,const uint16_t* data,uint8_t num);
    void     data_sync();
    void    sensordata_sync();
    void    run_time_sync();
    // string data_to_json() const;
    string data_to_json(const string& db,const string& str) const;
    void   send_fertilizermach(float Press,float Flow);
    void   send_fertilizerpump(uint16_t state);
    void   send_waterpump(uint16_t state);
};




#endif //KOKIRIKA_COMMUNICATION_H
