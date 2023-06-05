/**
* @Author kokirika
* @Name Modbus
* @Date 2022-09-20
**/

#ifndef Kokirika_MODBUS_H
#define Kokirika_MODBUS_H

#include "CRC.h"
#include "USART.h"
#include "Timer.h"

class modbus: public CRC16_Modbus,public Call_Back{
public:
    enum {
        HOST=1,
        SLAVE=0,
    };
private:
    _USART_* USARTx{};
    Timer  * TIMERX{};
    enum {
        modbus_success =0x00,
        modbus_03_error=0x03,
        modbus_06_error=0x06,
        modbus_10_error=0x10,
    };
    bool init_flag{};
    uint8_t run_mode{};
    uint8_t own_id{};
    uint8_t slave_id{};
    uint16_t timeout{};
    uint16_t freetime{};
    uint16_t freetime_t{};
    uint8_t send_flag{};
    uint16_t slave_address{};
    uint16_t *data_list{};
    uint16_t data_start_end[2]{};

    uint16_t read_data(uint16_t address);
    bool  write_data(uint16_t address,const uint8_t* data);
    void  send_data_fun(string str);
    bool modbus_wait_rec() const;
protected:
    string  modbus_receive_data;
    string  modbus_send_data;
    uint16_t reveive_len{};
public:
    explicit modbus(_USART_* USART,uint8_t mode =SLAVE,uint8_t id=1,uint16_t stimeout=500,uint16_t sfreetime=20);
    modbus()=default;
    ~modbus();
    void init(_USART_* USART,uint8_t mode =SLAVE,uint8_t id=1,uint16_t stimeout=500,uint16_t sfreetime=20);
    void config(uint16_t* address,uint16_t len,uint16_t start=0);
    void set_id(uint8_t id=1);
    void set_mode(uint8_t mode =0);
    void set_freetime(uint16_t sfreetime);
    void set_timeout(uint16_t stimeout);
    void modbus_Slave_03_uncoding();
    void modbus_Host_03_coding(uint8_t ID,uint16_t address,uint16_t num);
    void modbus_Host_03_uncoding();
    void modbus_Slave_06_uncoding();
    void modbus_Host_06_coding(uint8_t ID,uint16_t address,uint16_t data);
    void modbus_Host_06_uncoding();
    void modbus_Slave_16_uncoding();
    void modbus_Host_16_coding(uint8_t ID,uint16_t address,const uint16_t *data,uint16_t num);
    void modbus_Host_16_uncoding();

    uint8_t modbus_03_send(uint16_t address,uint16_t num);
    uint8_t modbus_06_send(uint16_t address,uint16_t data);
    uint8_t modbus_16_send(uint16_t address,uint16_t *data,uint16_t num);

    void modbus_receive_upset();
    void receive_data_channel();

    void Link_UART_CALLback();
    void Link_TIMER_CALLback(Timer *TIMX);
    void Callback(int  ,char** gdata ) override;
};


#endif //Kokirika_MODBUS_H
