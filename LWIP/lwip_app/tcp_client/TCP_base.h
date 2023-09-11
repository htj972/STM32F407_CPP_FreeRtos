/**
* @Author kokirika
* @Name TCP_BASE
* @Date 2023-06-12
*/
#ifndef kokirika_TCP_BASE_H
#define kokirika_TCP_BASE_H

#include "sys.h"
#include "HARD_BASE.h"
#include "lwip/ip_addr.h"

class TCP_BASE {
    protected:
    uint16_t tcp_port{};
    public:
    virtual void send_data(char *data,uint16_t len){};
    virtual void upload_extern_fun(Call_Back *extx){};
    virtual bool connect(uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4,uint16_t port)
                {return true;}
    virtual bool connect(ip_addr rmtipaddr,uint16_t port){return true;}
    virtual void close(){};//关闭TCP Client连接
    virtual bool islink(){return true;}//判断是否连接上服务器
};

#endif //kokirika_TCP_BASE_H
