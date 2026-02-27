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
    uint16_t* Holding = nullptr;
    uint8_t Holding_num = 0;
    bool queue_flag[COM_queue_num]{};
    uint16_t addx[COM_queue_num][3]{};
    uint16_t datax[COM_queue_num][10]{};
    uint8_t  queue_num=0;
    uint8_t  queue_len=3;
public:
    Communication(USART_TypeDef* USARTx,uint8_t DE,TIM_TypeDef *TIMx, uint16_t frq);
    void initial(uint16_t* baseAddr,uint16_t len);
    uint16_t find_address(const float *data);
    uint16_t find_address(const uint16_t *data);
    void     data_set(uint8_t id,uint16_t address,uint16_t data);
    void     data_set(uint8_t id,uint16_t address,const uint16_t* data,uint8_t num);
    void     data_sync();
    void    sensordata_sync();
    string data_to_json(const string& db,const string& str) const;
};




#endif //KOKIRIKA_COMMUNICATION_H
