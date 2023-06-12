/**
* @Author kokirika
* @Name ${PACKAGE_NAME}
* @Date 2023-06-07
*/

#ifndef STM32F407_CPP_FREERTOS_UDP_CLASS_H
#define STM32F407_CPP_FREERTOS_UDP_CLASS_H

#include <string>
#include "sys.h"
#include "lwip_comm/lwip_comm.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"

using namespace std;

#define UDP_NUM		        3		//定义udp最大连接数量

class UDP_Class {
public:

private:
    //bit7:没有用到
    //bit6:0,没有收到数据;1,收到数据了.
    //bit5:0,没有连接上;1,连接上了.
    //bit4~0:保留
    static uint8_t link_flag[UDP_NUM];
    static std::string udp_recvbuf[UDP_NUM];
    static struct udp_pcb *udppcb[UDP_NUM];  	//定义一个TCP服务器控制块

    uint8_t udp_Num{};
    uint16_t udp_port{};
    struct udp_pcb *mpcb{};
    static void receive(void *arg,struct udp_pcb *upcb,struct pbuf *p,struct ip_addr *addr,u16_t port);
public:
    explicit UDP_Class(uint16_t port=8080):udp_port(port){};
    UDP_Class()=default;
    ~UDP_Class()=default;

    bool upset(bool link= false);
    void loop();
    bool bind();
    bool connect(uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4);
    bool connect(ip_addr rmtipaddr);
    void close();

    ip_addr get_remote_ip() const;

    void udp_senddata(char* data);
    uint16_t available() const;
    virtual void write(const char *str,uint16_t len);
    virtual void write(uint8_t *str,uint16_t len);
    virtual void write(string String);
    virtual uint16_t print(const char *fmt, ...);
    virtual uint16_t print(const string& String);
    virtual uint16_t print(const char *s);
    virtual uint16_t print(char s);
    virtual uint16_t print(int integer);
    uint16_t println(const char *fmt, ...);
    uint16_t println(const string& String);
    uint16_t println(const char *s);
    uint16_t println(int integer);

    string read_data() const;
    string read_data(uint8_t len) const;
    string read_data(char c) const ;
    string read_data(const string& str) const ;


    UDP_Class& operator<<(const int integer)
    {
        this->print("%d", integer);
        return *this;
    }
    UDP_Class& operator<<(const char *s)
    {
        this->print("%s", s);
        return *this;
    }
    UDP_Class& operator<<(const std::string& Str)
    {
        this->print(Str);
        return *this;
    }

    UDP_Class& operator>>(std::string& Str)
    {
        Str.append(this->read_data());
        return *this;
    }

};


#endif //STM32F407_CPP_FREERTOS_UDP_CLASS_H
