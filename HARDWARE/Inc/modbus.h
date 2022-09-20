/**
* @Author kokirika
* @Name Modbus
* @Date 2022-09-20
**/

#ifndef Kokirika_MODBUS_H
#define Kokirika_MODBUS_H

#include "CRC.h"
#include "USART.h"

class modbus: public CRC16_Modbus{
private:
    _USART_* USARTx;
    const uint8_t HOST=1;
    const uint8_t SLAVE=0;
    uint8_t init_flag=0;
    uint8_t run_mode=modbus::SLAVE;
    uint8_t own_id=1;
    uint8_t slave_id=1;
    uint16_t slave_address;
    uint16_t *data_list{};
    uint16_t data_start_end[2]{};
    uint8_t * modbus_receive_data{};
    uint16_t read_data(const uint8_t *address);
    uint8_t  write_data(uint8_t *address,uint8_t* data);
    void  send_data_fun(uint8_t* data,uint16_t len);
public:
    modbus();
    ~modbus();
    void init(uint16_t* address,uint16_t len,uint16_t start=0);
    void modbus_Slave_03_uncoding();
    void modbus_Host_03_coding(uint8_t ID,uint16_t address,uint16_t num);
    void modbus_03_uncoding();

};


#endif //Kokirika_MODBUS_H
