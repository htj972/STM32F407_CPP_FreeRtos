/**
* @Author kokirika
* @Name ${PACKAGE_NAME}
* @Date 2023-06-07
*/

#include "UDP_Class.h"
#include "delay.h"
#include <cstring>
#include <cstdio>
#include <cstdarg>

string UDP_Class::udp_recvbuf[UDP_NUM];	//UDP�������ݻ�����
uint8_t UDP_Class::link_flag[UDP_NUM];
struct udp_pcb *UDP_Class::udppcb[UDP_NUM];  	//����һ��TCP���������ƿ�

void UDP_Class::receive(void *arg, struct udp_pcb *upcb, struct pbuf *p, struct ip_addr *addr, u16_t port) {

    struct pbuf *q;

    uint8_t ii;
    for (ii=0;ii<UDP_NUM;ii++)
    {
        if(upcb==udppcb[ii])break;
    }
    if(ii>=UDP_NUM)return ;	//û���ҵ���Ӧ��UDP����,ֱ�ӷ���

    if(p!=nullptr)	//���յ���Ϊ�յ�����ʱ
    {
        udp_recvbuf[ii].clear();//���ݽ��ջ���������
        for(q=p;q!=nullptr;q=q->next)  //����������pbuf����
        {
            udp_recvbuf[ii].append(reinterpret_cast<char *>(q->payload),q->len);
        }
        upcb->remote_ip=*addr; 				//��¼Զ��������IP��ַ
        upcb->remote_port=port;  			//��¼Զ�������Ķ˿ں�
        link_flag[ii]|=1<<6;	//��ǽ��յ�������
        pbuf_free(p);//�ͷ��ڴ�
    }
}

void UDP_Class::close() {
    udp_disconnect(this->mpcb);
    udp_remove(this->mpcb);		//�Ͽ�UDP����
    this->mpcb=nullptr;
}

ip_addr UDP_Class::get_remote_ip() const {
    return this->mpcb->remote_ip;
}

void UDP_Class::udp_senddata(char* data) {
    struct pbuf *ptr;
    ptr=pbuf_alloc(PBUF_TRANSPORT,strlen(data),PBUF_POOL); //�����ڴ�
    if(ptr)
    {
        ptr->payload=(void*)data;
        udp_send(this->mpcb,ptr);	//udp��������
        pbuf_free(ptr);//�ͷ��ڴ�
    }
}

void UDP_Class::loop() {
    while (upset()){
        delay_ms(2);
    }
}

bool UDP_Class::upset(bool link) {
//    lwip_periodic_handle();//����LWIPЭ��Ķ�ʱ��
    if (link_flag[this->udp_Num] & 1 << 6)  //�Ƿ��յ�����?
    {
        if((link_flag[this->udp_Num]&1<<5)&&(link))//�Ƿ��Ѿ�����
        {
            this->connect(this->mpcb->remote_ip);
            link_flag[this->udp_Num]&=~(1<<5);//����Ѿ�����
        }
    } else
        return false;
    return true;
}

bool UDP_Class::bind() {
    //��ѯ���е�UDP����
    uint8_t udp_link_num = UDP_NUM;
    for (uint8_t i = 0; i < UDP_NUM; i++) {
        if (udppcb[i] == nullptr) {
            udp_link_num = i;
            break;
        }
    }
    if(udp_link_num<UDP_NUM) {
        this->mpcb = udp_new();
        udppcb[udp_link_num] = this->mpcb;
    }
    this->udp_Num = udp_link_num;
    err_t err = udp_bind(this->mpcb, IP_ADDR_ANY, udp_port);//�󶨱���IP��ַ��˿ں�
    if (err == ERR_OK)    //�����
    {
        udp_recv(this->mpcb, receive, nullptr);//ע����ջص�����
    } else return false;
    return true;
}

bool UDP_Class::connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4) {
    struct ip_addr rmtipaddr{};  	//Զ��ip��ַ
    IP4_ADDR(&rmtipaddr, ip1, ip2, ip3, ip4);//����Զ��IP��ַ
    //UDP�ͻ������ӵ�ָ��IP��ַ�Ͷ˿ںŵķ�����
    if(ERR_OK == udp_connect(this->mpcb, &rmtipaddr, udp_port))
        return true;
    return false;
}

bool UDP_Class::connect(ip_addr rmtipaddr) {
    if(ERR_OK == udp_connect(this->mpcb, &rmtipaddr, udp_port))
        return true;
    return false;
}


uint16_t UDP_Class::available() const {
    return  udp_recvbuf[this->udp_Num].length();
}

void UDP_Class::write(const char *str, uint16_t len) {
    struct pbuf *ptr;
    ptr=pbuf_alloc(PBUF_TRANSPORT,len,PBUF_POOL); //�����ڴ�
    if(ptr)
    {
        ptr->payload=(void*)str;
        udp_send(this->mpcb,ptr);	//udp��������
        pbuf_free(ptr);//�ͷ��ڴ�
    }
}

void UDP_Class::write(uint8_t *str, uint16_t len) {
    this->write((char*)str,len);
}

void UDP_Class::write(string String) {
    const char* p=String.c_str();
    this->write(p,String.length());
}

uint16_t UDP_Class::print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    string r;
    r.resize(static_cast<size_t>(len) + 1);
    va_start(args, fmt);
    vsnprintf(&r.front(), len + 1, fmt, args);
    va_end(args);
    r.resize(static_cast<size_t>(len));
    this->write(r);
    return r.length();
}

uint16_t UDP_Class::print(const string &String) {
    this->write(String);
    return String.length();
}

uint16_t UDP_Class::print(const char *s) {
    return this->print("%s",s);
}

uint16_t UDP_Class::print(char s) {
    return this->print("%c",s);
}

uint16_t UDP_Class::print(int integer) {
    return this->print("%d",integer);
}

uint16_t UDP_Class::println(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    string r;
    r.resize(static_cast<size_t>(len) + 1);
    va_start(args, fmt);
    vsnprintf(&r.front(), len + 1, fmt, args);
    va_end(args);
    r.resize(static_cast<size_t>(len));
    r.append("\r\n");
    this->write(r);
    return r.length();
}

uint16_t UDP_Class::println(const string& String) {
    return this->print(String+"\r\n");
}

uint16_t UDP_Class::println(const char *s) {
    return this->print("%s\r\n",s);
}

uint16_t UDP_Class::println(int integer) {
    return this->print("%d\r\n",integer);
}

string UDP_Class::read_data() const {
    string ret=udp_recvbuf[this->udp_Num];
    udp_recvbuf[this->udp_Num].clear();
    return ret;
}

string UDP_Class::read_data(uint8_t len) const {
    string  ret=udp_recvbuf[this->udp_Num].substr(0,len);
    udp_recvbuf[this->udp_Num].erase(0,len);
    return ret;
}

string UDP_Class::read_data(char c) const {
    string  ret=udp_recvbuf[this->udp_Num].substr(0,udp_recvbuf[this->udp_Num].find(c));
    udp_recvbuf[this->udp_Num].erase(0,udp_recvbuf[this->udp_Num].find(c));
    return ret;
}

string UDP_Class::read_data(const string &str) const {
    string  ret=udp_recvbuf[this->udp_Num].substr(0,udp_recvbuf[this->udp_Num].find(str));
    udp_recvbuf[this->udp_Num].erase(0,udp_recvbuf[this->udp_Num].find(str));
    return ret;
}

