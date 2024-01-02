/**
* @Author kokirika
* @Name WH_L101
* @Date 2023-12-06
*/
#ifndef Kokirika_WH_L101_H
#define Kokirika_WH_L101_H

#include "USART.h"
#include "modbus.h"

class WH_L101: public modbus{
private:
    uint8_t dID[4]{};
    void send_cmd(const std::string& cmd);
    void get_id();
    void get_rssi(string str);
    uint8_t slave_id[4]{};
protected:
    uint8_t SNR{},RSSI{};
public:
    WH_L101()=default;
    explicit WH_L101(_USART_ *UARTx);
    void init(_USART_ *UARTx);
    void set_mode(uint16_t channel,uint16_t ADDR);

    void set_id(const uint8_t *id);


    void modbus_Host_03_coding(uint8_t *ID,uint16_t address,uint16_t num);
    void modbus_Host_03_uncoding();
    void modbus_Host_06_coding(uint8_t *ID,uint16_t address,uint16_t data);
    void modbus_Host_06_uncoding();
    void modbus_Host_16_coding(uint8_t *ID,uint16_t address,const uint16_t *data,uint16_t num);
    void modbus_Host_16_uncoding();

    uint8_t modbus_03_send(uint16_t address,uint16_t num);
    uint8_t modbus_06_send(uint16_t address,uint16_t data);
    uint8_t modbus_16_send(uint16_t address,uint16_t *data,uint16_t num);
    void receive_data_channel() override;
};


#endif //Kokirika_WH_L101_H
