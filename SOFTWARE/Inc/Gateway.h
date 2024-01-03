/**
* @Author kokirika
* @Name Gateway
* @Date 2023-11-24
*/
#ifndef Kokirika_Gateway_H
#define Kokirika_Gateway_H

#include "Device_Node_Def.h"
#include "FM24Cxx.h"
#include "ThingsBoard.h"
#include "WH_COM.h"
#include "Out_In_Put.h"
#include "Communication.h"

class Gateway: public Device_Node_Def,private Software_IIC,FM24Cxx {
public:
    uint8_t ip[4]{};//IP地址 0-3
    uint16_t port{};//端口号 4-5
    std::string clientID;//客户端ID 6-42
    std::string username;//用户名  43-83
    std::string password;//密码   84-124
private:
    uint16_t cmd_length{};//总指令长度    128
    uint16_t cmd_len{};//指令长度
protected:
    ThingsBoard *thingsBoard{};
    WH_COM *lora{};
    _OutPut_ *OUT{};
    Communication *rs485{};
public:
    Gateway(GPIO_Pin SCL,GPIO_Pin SDA,uint16_t TYPE):
            Software_IIC(SCL,SDA),FM24Cxx(this,TYPE){}
    bool inital();
    void link_thingsBoard(ThingsBoard *thingsBoard_t);
    void link_OUT(_OutPut_ *OUT_t);
    void Link_lora(WH_COM *lorax);
    void Link_rs485(Communication *rs485x);
    void save_config();
    void set_ip(const uint8_t *ip_t);
    void set_port(uint16_t port_t);
    void set_clientID(std::string clientID_t);
    void set_username(std::string username_t);
    void set_password(std::string password);
    void save_cmd(const std::string &cmd);
    std::string read_next_cmd();
    void set_reread();
    bool run_cmd(_USART_ *Debug);
    std::string Command(const std::string &COM,bool save=true);

    void print_env(_USART_ *Debug);

};


#endif //Kokirika_Gateway_H
