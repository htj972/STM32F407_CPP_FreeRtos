/**
* @Author kokirika
* @Name TCP_CLASS
* @Date 2023-06-09
*/
#ifndef KOKIRIKA_TCP_CLASS_H
#define KOKIRIKA_TCP_CLASS_H

#include "sys.h"
#include "lwip/debug.h"
#include "lwip/stats.h"
#include "lwip/tcp.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip_comm/lwip_comm.h"
#include <string>
#include "HARD_BASE.h"
#include "TCP_BASE.h"

using namespace std;

#define TCP_NUM		        3

class TCP_Client_Class: public TCP_BASE{
private:
    //tcp����������״̬
    enum tcp_client_states
    {
        ES_TCPCLIENT_NONE = 0,		//û������
        ES_TCPCLIENT_CONNECTED,		//���ӵ���������
        ES_TCPCLIENT_CLOSING,		//�ر�����
    };
//LWIP�ص�����ʹ�õĽṹ��
    struct tcp_client_struct
    {
        u8 state;               //��ǰ����״
        struct tcp_pcb *pcb;    //ָ��ǰ��pcb
        struct pbuf *p;         //ָ�����/�����pbuf
    };
    //bit7:0,û������Ҫ����;1,������Ҫ����
    //bit6:0,û���յ�����;1,�յ�������.
    //bit5:0,û�������Ϸ�����;1,�����Ϸ�������.
    //bit4~0:����
    static uint8_t link_flag[TCP_NUM];
    static std::string recvbuf[TCP_NUM];
    static std::string sendbuf[TCP_NUM];
    static struct tcp_pcb *tcppcb[TCP_NUM];  	//����һ��TCP���������ƿ�

    static err_t recv(void *arg,struct tcp_pcb *tpcb,struct pbuf *p,err_t err);//tcp_recv()�����Ļص�����
    static void error(void *arg,err_t err);//tcp_err()�ص�����
    static err_t poll(void *arg, struct tcp_pcb *tpcb);//tcp_poll()�ص�����
    static err_t sent(void *arg, struct tcp_pcb *tpcb, u16_t len);//tcp_sent()�ص�����
    static void  send(struct tcp_pcb *tpcb, struct tcp_client_struct * es);//TCP Client��������
    static void connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es );//�ر�TCP Client����
    static err_t connected(void *arg, struct tcp_pcb *tpcb, err_t err);//TCP���ӽ�������ûص�����
protected:
    uint8_t tcp_Num{};
    uint16_t tcp_port{};
    struct tcp_pcb *mpcb{};
public:
    explicit TCP_Client_Class(uint16_t port):tcp_port(port){this->tcp_Num=TCP_NUM;};
    TCP_Client_Class(){this->tcp_Num=TCP_NUM;};
    ~TCP_Client_Class()=default;
    bool connect(uint8_t ip1,uint8_t ip2,uint8_t ip3,uint8_t ip4,uint16_t port) override;
    bool connect(ip_addr rmtipaddr,uint16_t port) override;
    bool connect(ip_addr rmtipaddr);
    void close() override;//�ر�TCP Client����
    bool upset() const;//����LWIPЭ��Ķ�ʱ��
    bool islink() override;//�ж��Ƿ������Ϸ�����

    void upload_extern_fun(Call_Back *extx) override;
    static void extern_upset(uint8_t num);

    void send_data(char *data,uint16_t len) override;
    uint16_t available() const;
    void write(const char *str,uint16_t len);
    void write(uint8_t *str,uint16_t len);
    void write(const string& String);
    uint16_t print(const char *fmt, ...);
    uint16_t print(const string& String);
    uint16_t print(const char *s);
    uint16_t print(char s);
    uint16_t print(int integer);
    uint16_t println(const char *fmt, ...);
    uint16_t println(const string& String);
    uint16_t println(const char *s);
    uint16_t println(int integer);

    string read_data() const;
    string read_data(uint8_t len) const;
    string read_data(char c) const ;
    string read_data(const string& str) const ;


    TCP_Client_Class& operator<<(const int integer)
    {
        this->print("%d", integer);
        return *this;
    }
    TCP_Client_Class& operator<<(const char *s)
    {
        this->print("%s", s);
        return *this;
    }
    TCP_Client_Class& operator<<(const std::string& Str)
    {
        this->print(Str);
        return *this;
    }

    TCP_Client_Class& operator>>(std::string& Str)
    {
        Str.append(this->read_data());
        return *this;
    }

};


#endif //KOKIRIKA_TCP_CLASS_H
