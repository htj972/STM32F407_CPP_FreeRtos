/**
* @Author kokirika
* @Name TCP_CLASS
* @Date 2023-06-09
*/

#include <cstdarg>
#include "TCP_Client_Class.h"


string TCP_Client_Class::recvbuf[TCP_NUM];	//TCP�������ݻ�����
string TCP_Client_Class::sendbuf[TCP_NUM];	//TCP�������ݻ�����
uint8_t TCP_Client_Class::link_flag[TCP_NUM];
struct tcp_pcb *TCP_Client_Class::tcppcb[TCP_NUM];  	//����һ��TCP���������ƿ�
uint8_t TCP_Client_Class::TCP_link_num=0;

TCP_Client_Class::TCP_Client_Class(uint16_t port) {
    this->tcp_port=port;
    this->tcp_Num=TCP_link_num;
    link_flag[this->tcp_Num]=0;
    TCP_link_num++;
}

TCP_Client_Class::TCP_Client_Class() {
    this->tcp_Num=TCP_link_num;
    link_flag[this->tcp_Num]=0;
    TCP_link_num++;
}

//lwIP TCP���ӽ�������ûص�����
err_t TCP_Client_Class::connected(void *arg, struct tcp_pcb *tpcb, err_t err)
{
    struct tcp_client_struct *es;
    if(err==ERR_OK)
    {
        uint8_t ii=0;
        for (; ii < TCP_NUM; ii++) {
            if(tcppcb[ii]==tpcb)
                break;
        }
        if (ii  >= TCP_NUM) {
            return ERR_MEM;
        }
        es=(struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));  //�����ڴ�
        if(es) //�ڴ�����ɹ�
        {
            es->state=ES_TCPCLIENT_CONNECTED;//״̬Ϊ���ӳɹ�
            es->pcb=tpcb;
            es->p=nullptr;
            tcp_arg(tpcb,es);        			//ʹ��es����tpcb��callback_arg
            tcp_recv(tpcb,recv);  	//��ʼ��LwIP��tcp_recv�ص�����
            tcp_err(tpcb,error); 	//��ʼ��tcp_err()�ص�����
            tcp_sent(tpcb,sent);		//��ʼ��LwIP��tcp_sent�ص�����
            tcp_poll(tpcb,poll,1); 	//��ʼ��LwIP��tcp_poll�ص�����
            link_flag[ii]|=1 << 5; 				//������ӵ���������
            err=ERR_OK;
        }else
        {
            connection_close(tpcb,es);//�ر�����
            err=ERR_MEM;	//�����ڴ�������
        }
    }else
    {
        connection_close(tpcb,nullptr);//�ر�����
    }
    return err;
}

//lwIP tcp_recv()�����Ļص�����
err_t TCP_Client_Class::recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct pbuf *q;
    struct tcp_client_struct *es;
    err_t ret_err;
    es=(struct tcp_client_struct *)arg;
    if(p==nullptr)//����ӷ��������յ��յ�����֡�͹ر�����
    {
        es->state=ES_TCPCLIENT_CLOSING;//��Ҫ�ر�TCP ������
        es->p=p;
        ret_err=ERR_OK;
    }else if(err!= ERR_OK)//�����յ�һ���ǿյ�����֡,����err!=ERR_OK
    {
        pbuf_free(p);//�ͷŽ���pbuf
        ret_err=err;
    }else if(es->state==ES_TCPCLIENT_CONNECTED)	//����������״̬ʱ
    {
        uint8_t ii;
        for (ii=0;ii<TCP_NUM;ii++)
        {
            if(tpcb==tcppcb[ii])break;
        }
        if(ii>=TCP_NUM)return ERR_ARG;//û���ҵ���Ӧ��UDP����,ֱ�ӷ���
        recvbuf[ii].clear();
        for(q=p;q!=nullptr;q=q->next)  //����������pbuf����
        {
            recvbuf[ii].append(reinterpret_cast<char *>(q->payload),q->len);
        }
        extern_upset(ii);
        link_flag[ii]|=1 << 6;		//��ǽ��յ�������
        tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
        pbuf_free(p);  	//�ͷ��ڴ�
        ret_err=ERR_OK;
    }else  //���յ����ݵ��������Ѿ��ر�,
    {
        tcp_recved(tpcb,p->tot_len);//���ڻ�ȡ��������,֪ͨLWIP���Ի�ȡ��������
        es->p=nullptr;
        pbuf_free(p); //�ͷ��ڴ�
        ret_err=ERR_OK;
    }
    return ret_err;
}
//lwIP tcp_err�����Ļص�����
void TCP_Client_Class::error(void *arg, err_t err)
{
    //�ж����Ǹ����ӳ�����
    struct tcp_client_struct *es;//����һ���ṹ��ָ��
    es=(struct tcp_client_struct*)arg;//��ȡ���ӵĽṹ��
    if(es!=nullptr)  //���Ӵ��ڿ��п��Է�������
    {
        uint8_t ii;
        for (ii=0;ii<TCP_NUM;ii++)
        {
            if(es->pcb==tcppcb[ii])break;
        }
        if(ii>=TCP_NUM)return;//û���ҵ���Ӧ��TCP����,ֱ�ӷ���
        link_flag[ii]&=~(1 << 5);
    }
}
//lwIP tcp_poll�Ļص�����
err_t TCP_Client_Class::poll(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct tcp_client_struct *es;
    es=(struct tcp_client_struct*)arg;
    if(es!=nullptr)  //���Ӵ��ڿ��п��Է�������
    {
        uint8_t ii;
        for (ii=0;ii<TCP_NUM;ii++)
        {
            if(tpcb==tcppcb[ii])break;
        }
        if(ii>=TCP_NUM)return ERR_ARG;//û���ҵ���Ӧ��UDP����,ֱ�ӷ���
        if(link_flag[ii]&(1 << 7))	//�ж��Ƿ�������Ҫ����
        {
            es->p=pbuf_alloc(PBUF_TRANSPORT, sendbuf[ii].length(),PBUF_POOL);	//�����ڴ�
            //tcp_client_data[]�е����ݿ�����es->p_tx��
            pbuf_take(es->p,sendbuf[ii].data(),sendbuf[ii].length());
            send(tpcb,es);//��tcp_client_sentbuf[]���渴�Ƹ�pbuf�����ݷ��ͳ�ȥ
            link_flag[ii]&=~(1 << 7);	//������ݷ��ͱ�־
            if(es->p)pbuf_free(es->p);	//�ͷ��ڴ�
        }else if(es->state==ES_TCPCLIENT_CLOSING)
        {
            connection_close(tpcb,es);//�ر�TCP����
        }
        ret_err=ERR_OK;
    }else
    {
        tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
        ret_err=ERR_ABRT;
    }
    return ret_err;
}
//lwIP tcp_sent�Ļص�����(����Զ���������յ�ACK�źź�������)
err_t TCP_Client_Class::sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct tcp_client_struct *es;
    LWIP_UNUSED_ARG(len);
    es=(struct tcp_client_struct*)arg;
    if(es->p)send(tpcb,es);//��������
    return ERR_OK;
}
//�˺���������������
void TCP_Client_Class::send(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
    struct pbuf *ptr;
    err_t wr_err=ERR_OK;
    while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb))) //��Ҫ���͵����ݼ��뵽���ͻ��������
    {
        ptr=es->p;
        wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
        if(wr_err==ERR_OK)
        {
            es->p=ptr->next;			//ָ����һ��pbuf
            if(es->p)pbuf_ref(es->p);	//pbuf��ref��һ
            pbuf_free(ptr);				//�ͷ�ptr
        }else if(wr_err==ERR_MEM)es->p=ptr;
        tcp_output(tpcb);		//�����ͻ�������е������������ͳ�ȥ
    }
}
//�ر��������������
void TCP_Client_Class::connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
    //�Ƴ��ص�
    tcp_abort(tpcb);//��ֹ����,ɾ��pcb���ƿ�
    tcp_arg(tpcb,nullptr);
    tcp_recv(tpcb,nullptr);
    tcp_sent(tpcb,nullptr);
    tcp_err(tpcb,nullptr);
    tcp_poll(tpcb,nullptr,0);
    if(es)mem_free(es);
    uint8_t ii;
    for (ii=0;ii<TCP_NUM;ii++)
    {
        if(tpcb==tcppcb[ii])break;
    }
    if(ii>=TCP_NUM)return;
    tcppcb[ii] = nullptr;
    link_flag[ii]&=~(1 << 5);//������ӶϿ���
}

bool TCP_Client_Class::connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4,uint16_t port) {
    struct ip_addr rmtipaddr{};  	//Զ��ip��ַ
    IP4_ADDR(&rmtipaddr, ip1, ip2, ip3, ip4);
    return connect(rmtipaddr,port);
}

bool TCP_Client_Class::connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4) {
    struct ip_addr rmtipaddr{};  	//Զ��ip��ַ
    IP4_ADDR(&rmtipaddr, ip1, ip2, ip3, ip4);
    return connect(rmtipaddr);
}

bool TCP_Client_Class::connect(ip_addr rmtipaddr,uint16_t port) {
    uint8_t ret;
    //������Ӳ�����
    if(this->mpcb== nullptr){
        this->mpcb = tcp_new();
        tcppcb[this->tcp_Num] = this->mpcb;
    }
    this->tcp_port=port;
    ret=tcp_connect(this->mpcb,&rmtipaddr,tcp_port,connected);  //���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
    if(ret==ERR_OK)return true;
    else return false;
}

bool TCP_Client_Class::connect(ip_addr rmtipaddr) {
    uint8_t ret;
    if(this->mpcb== nullptr){
        this->mpcb = tcp_new();
        tcppcb[this->tcp_Num] = this->mpcb;
    }
    ret=tcp_connect(this->mpcb,&rmtipaddr,tcp_port,connected);  //���ӵ�Ŀ�ĵ�ַ��ָ���˿���,�����ӳɹ���ص�tcp_client_connected()����
    if(ret==ERR_OK)return true;
    else return false;
}

void TCP_Client_Class::close() {
    if(tcppcb[this->tcp_Num]!= nullptr)
    {
        tcp_close(tcppcb[this->tcp_Num]);
    }
    link_flag[this->tcp_Num]=0;//������ӶϿ���
    this->mpcb = nullptr;
}

bool TCP_Client_Class::upset() const {
//    lwip_periodic_handle();//����LWIPЭ��Ķ�ʱ��
    if (link_flag[this->tcp_Num] & 1 << 6)  //�Ƿ��յ�����?
    {
        link_flag[this->tcp_Num] &= ~(1 << 6);  //������յ����ݱ�־
        return true;
    } else
        return false;
}

bool TCP_Client_Class::islink() {
    //�ж��Ƿ�Ͽ�
    if(this->mpcb->state==CLOSED)
    {
        link_flag[this->tcp_Num]&=~(1 << 5);//������ӶϿ���
        return false;
    }
    if (link_flag[this->tcp_Num] & (1 << 5))  //�Ƿ����ӳɹ�?
        return true;
    else
        return false;
}

void TCP_Client_Class::send_data(char *data,uint16_t len) {
    sendbuf[this->tcp_Num].clear();
    sendbuf[this->tcp_Num].append(data,len);
    link_flag[this->tcp_Num]|=1 <<  7;		//���������Ҫ����
}

uint16_t TCP_Client_Class::available() const {
    return  recvbuf[this->tcp_Num].length();
}

void TCP_Client_Class::write(const char *str, uint16_t len) {
    send_data((char *)str,len);	//TCP��������
}

void TCP_Client_Class::write(uint8_t *str, uint16_t len) {
    this->write((char*)str,len);
}

void TCP_Client_Class::write(const string& String) {
    const char* p=String.c_str();
    this->write(p,String.length());
}

uint16_t TCP_Client_Class::print(const char *fmt, ...) {
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

uint16_t TCP_Client_Class::print(const string &String) {
    this->write(String);
    return String.length();
}

uint16_t TCP_Client_Class::print(const char *s) {
    return this->print("%s",s);
}

uint16_t TCP_Client_Class::print(char s) {
    return this->print("%c",s);
}

uint16_t TCP_Client_Class::print(int integer) {
    return this->print("%d",integer);
}

uint16_t TCP_Client_Class::println(const char *fmt, ...) {
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

uint16_t TCP_Client_Class::println(const string& String) {
    return this->print(String+"\r\n");
}

uint16_t TCP_Client_Class::println(const char *s) {
    return this->print("%s\r\n",s);
}

uint16_t TCP_Client_Class::println(int integer) {
    return this->print("%d\r\n",integer);
}

string TCP_Client_Class::read_data() const {
    string ret=recvbuf[this->tcp_Num];
    recvbuf[this->tcp_Num].clear();
    return ret;
}

string TCP_Client_Class::read_data(uint8_t len) const {
    string  ret=recvbuf[this->tcp_Num].substr(0,len);
    recvbuf[this->tcp_Num].erase(0,len);
    return ret;
}

string TCP_Client_Class::read_data(char c) const {
    string  ret=recvbuf[this->tcp_Num].substr(0,recvbuf[this->tcp_Num].find(c));
    recvbuf[this->tcp_Num].erase(0,recvbuf[this->tcp_Num].find(c));
    return ret;
}

string TCP_Client_Class::read_data(const string &str) const {
    string  ret=recvbuf[this->tcp_Num].substr(0,recvbuf[this->tcp_Num].find(str));
    recvbuf[this->tcp_Num].erase(0,recvbuf[this->tcp_Num].find(str));
    return ret;
}

struct TCP_STRUCT_{
    bool extern_flag[TCP_NUM]{};
    Call_Back *ext[TCP_NUM]{};
    TCP_STRUCT_(){
        for(bool & i : extern_flag)
            i=false;
    }
}TCP_STRUCT;

void TCP_Client_Class::upload_extern_fun(Call_Back *extx) {
    TCP_STRUCT.ext[this->tcp_Num%TCP_NUM]=extx;
    TCP_STRUCT.extern_flag[this->tcp_Num%TCP_NUM] = true;
}

void TCP_Client_Class::extern_upset(uint8_t num)
{
    if(TCP_STRUCT.extern_flag[num]){
        TCP_STRUCT.ext[num % TCP_NUM]->Callback(recvbuf[num]);
        recvbuf[num].clear();
    }
}















