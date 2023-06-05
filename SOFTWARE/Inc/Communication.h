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

typedef struct K_USER_data{
    float sun;
}USER_data;

typedef union K_POWER_DATA_{
    USER_data to_float;
    uint16_t  to_u16[sizeof(USER_data)/2]{};
    uint8_t   to_u8t[sizeof(USER_data)];
    K_POWER_DATA_() {
        memset(this->to_u8t,0,sizeof(this->to_u8t));
    };
}K_POWER_DATA;
#define COM_queue_num 5
class Communication: public RS485, private Timer, public modbus {
private:
    bool queue_flag[COM_queue_num]{};
    float *datax[COM_queue_num]{};
    float valuex[COM_queue_num]{};
public:
    Communication(USART_TypeDef* USARTx,uint8_t DE,TIM_TypeDef *TIMx, uint16_t frq);
    K_POWER_DATA data_BUS;
    void initial();
    uint16_t find_address(const float *data);
    void     data_set(float *data,float value);
    void     data_sync();
};




#endif //KOKIRIKA_COMMUNICATION_H
