/**
* @Author kokirika
* @Name TCP_CLASS
* @Date 2023-06-09
*/

#include <cstdarg>
#include "TCP_Client_Class.h"


string TCP_Client_Class::recvbuf[TCP_NUM];	//TCP接收数据缓冲区
string TCP_Client_Class::sendbuf[TCP_NUM];	//TCP接收数据缓冲区
uint8_t TCP_Client_Class::link_flag[TCP_NUM];
struct tcp_pcb *TCP_Client_Class::tcppcb[TCP_NUM];  	//定义一个TCP服务器控制块
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

//lwIP TCP连接建立后调用回调函数
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
        es=(struct tcp_client_struct*)mem_malloc(sizeof(struct tcp_client_struct));  //申请内存
        if(es) //内存申请成功
        {
            es->state=ES_TCPCLIENT_CONNECTED;//状态为连接成功
            es->pcb=tpcb;
            es->p=nullptr;
            tcp_arg(tpcb,es);        			//使用es更新tpcb的callback_arg
            tcp_recv(tpcb,recv);  	//初始化LwIP的tcp_recv回调功能
            tcp_err(tpcb,error); 	//初始化tcp_err()回调函数
            tcp_sent(tpcb,sent);		//初始化LwIP的tcp_sent回调功能
            tcp_poll(tpcb,poll,1); 	//初始化LwIP的tcp_poll回调功能
            link_flag[ii]|=1 << 5; 				//标记连接到服务器了
            err=ERR_OK;
        }else
        {
            connection_close(tpcb,es);//关闭连接
            err=ERR_MEM;	//返回内存分配错误
        }
    }else
    {
        connection_close(tpcb,nullptr);//关闭连接
    }
    return err;
}

//lwIP tcp_recv()函数的回调函数
err_t TCP_Client_Class::recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err)
{
    struct pbuf *q;
    struct tcp_client_struct *es;
    err_t ret_err;
    es=(struct tcp_client_struct *)arg;
    if(p==nullptr)//如果从服务器接收到空的数据帧就关闭连接
    {
        es->state=ES_TCPCLIENT_CLOSING;//需要关闭TCP 连接了
        es->p=p;
        ret_err=ERR_OK;
    }else if(err!= ERR_OK)//当接收到一个非空的数据帧,但是err!=ERR_OK
    {
        pbuf_free(p);//释放接收pbuf
        ret_err=err;
    }else if(es->state==ES_TCPCLIENT_CONNECTED)	//当处于连接状态时
    {
        uint8_t ii;
        for (ii=0;ii<TCP_NUM;ii++)
        {
            if(tpcb==tcppcb[ii])break;
        }
        if(ii>=TCP_NUM)return ERR_ARG;//没有找到对应的UDP连接,直接返回
        recvbuf[ii].clear();
        for(q=p;q!=nullptr;q=q->next)  //遍历完整个pbuf链表
        {
            recvbuf[ii].append(reinterpret_cast<char *>(q->payload),q->len);
        }
        extern_upset(ii);
        link_flag[ii]|=1 << 6;		//标记接收到数据了
        tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
        pbuf_free(p);  	//释放内存
        ret_err=ERR_OK;
    }else  //接收到数据但是连接已经关闭,
    {
        tcp_recved(tpcb,p->tot_len);//用于获取接收数据,通知LWIP可以获取更多数据
        es->p=nullptr;
        pbuf_free(p); //释放内存
        ret_err=ERR_OK;
    }
    return ret_err;
}
//lwIP tcp_err函数的回调函数
void TCP_Client_Class::error(void *arg, err_t err)
{
    //判断是那个连接出错了
    struct tcp_client_struct *es;//定义一个结构体指针
    es=(struct tcp_client_struct*)arg;//获取连接的结构体
    if(es!=nullptr)  //连接处于空闲可以发送数据
    {
        uint8_t ii;
        for (ii=0;ii<TCP_NUM;ii++)
        {
            if(es->pcb==tcppcb[ii])break;
        }
        if(ii>=TCP_NUM)return;//没有找到对应的TCP连接,直接返回
        link_flag[ii]&=~(1 << 5);
    }
}
//lwIP tcp_poll的回调函数
err_t TCP_Client_Class::poll(void *arg, struct tcp_pcb *tpcb)
{
    err_t ret_err;
    struct tcp_client_struct *es;
    es=(struct tcp_client_struct*)arg;
    if(es!=nullptr)  //连接处于空闲可以发送数据
    {
        uint8_t ii;
        for (ii=0;ii<TCP_NUM;ii++)
        {
            if(tpcb==tcppcb[ii])break;
        }
        if(ii>=TCP_NUM)return ERR_ARG;//没有找到对应的UDP连接,直接返回
        if(link_flag[ii]&(1 << 7))	//判断是否有数据要发送
        {
            es->p=pbuf_alloc(PBUF_TRANSPORT, sendbuf[ii].length(),PBUF_POOL);	//申请内存
            //tcp_client_data[]中的数据拷贝到es->p_tx中
            pbuf_take(es->p,sendbuf[ii].data(),sendbuf[ii].length());
            send(tpcb,es);//将tcp_client_sentbuf[]里面复制给pbuf的数据发送出去
            link_flag[ii]&=~(1 << 7);	//清除数据发送标志
            if(es->p)pbuf_free(es->p);	//释放内存
        }else if(es->state==ES_TCPCLIENT_CLOSING)
        {
            connection_close(tpcb,es);//关闭TCP连接
        }
        ret_err=ERR_OK;
    }else
    {
        tcp_abort(tpcb);//终止连接,删除pcb控制块
        ret_err=ERR_ABRT;
    }
    return ret_err;
}
//lwIP tcp_sent的回调函数(当从远端主机接收到ACK信号后发送数据)
err_t TCP_Client_Class::sent(void *arg, struct tcp_pcb *tpcb, u16_t len)
{
    struct tcp_client_struct *es;
    LWIP_UNUSED_ARG(len);
    es=(struct tcp_client_struct*)arg;
    if(es->p)send(tpcb,es);//发送数据
    return ERR_OK;
}
//此函数用来发送数据
void TCP_Client_Class::send(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
    struct pbuf *ptr;
    err_t wr_err=ERR_OK;
    while((wr_err==ERR_OK)&&es->p&&(es->p->len<=tcp_sndbuf(tpcb))) //将要发送的数据加入到发送缓冲队列中
    {
        ptr=es->p;
        wr_err=tcp_write(tpcb,ptr->payload,ptr->len,1);
        if(wr_err==ERR_OK)
        {
            es->p=ptr->next;			//指向下一个pbuf
            if(es->p)pbuf_ref(es->p);	//pbuf的ref加一
            pbuf_free(ptr);				//释放ptr
        }else if(wr_err==ERR_MEM)es->p=ptr;
        tcp_output(tpcb);		//将发送缓冲队列中的数据立即发送出去
    }
}
//关闭与服务器的连接
void TCP_Client_Class::connection_close(struct tcp_pcb *tpcb, struct tcp_client_struct * es)
{
    //移除回调
    tcp_abort(tpcb);//终止连接,删除pcb控制块
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
    link_flag[ii]&=~(1 << 5);//标记连接断开了
}

bool TCP_Client_Class::connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4,uint16_t port) {
    struct ip_addr rmtipaddr{};  	//远端ip地址
    IP4_ADDR(&rmtipaddr, ip1, ip2, ip3, ip4);
    return connect(rmtipaddr,port);
}

bool TCP_Client_Class::connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4) {
    struct ip_addr rmtipaddr{};  	//远端ip地址
    IP4_ADDR(&rmtipaddr, ip1, ip2, ip3, ip4);
    return connect(rmtipaddr);
}

bool TCP_Client_Class::connect(ip_addr rmtipaddr,uint16_t port) {
    uint8_t ret;
    //这个连接不存在
    if(this->mpcb== nullptr){
        this->mpcb = tcp_new();
        tcppcb[this->tcp_Num] = this->mpcb;
    }
    this->tcp_port=port;
    ret=tcp_connect(this->mpcb,&rmtipaddr,tcp_port,connected);  //连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
    if(ret==ERR_OK)return true;
    else return false;
}

bool TCP_Client_Class::connect(ip_addr rmtipaddr) {
    uint8_t ret;
    if(this->mpcb== nullptr){
        this->mpcb = tcp_new();
        tcppcb[this->tcp_Num] = this->mpcb;
    }
    ret=tcp_connect(this->mpcb,&rmtipaddr,tcp_port,connected);  //连接到目的地址的指定端口上,当连接成功后回调tcp_client_connected()函数
    if(ret==ERR_OK)return true;
    else return false;
}

void TCP_Client_Class::close() {
    if(tcppcb[this->tcp_Num]!= nullptr)
    {
        tcp_close(tcppcb[this->tcp_Num]);
    }
    link_flag[this->tcp_Num]=0;//标记连接断开了
    this->mpcb = nullptr;
}

bool TCP_Client_Class::upset() const {
//    lwip_periodic_handle();//处理LWIP协议的定时器
    if (link_flag[this->tcp_Num] & 1 << 6)  //是否收到数据?
    {
        link_flag[this->tcp_Num] &= ~(1 << 6);  //清除接收到数据标志
        return true;
    } else
        return false;
}

bool TCP_Client_Class::islink() {
    //判断是否断开
    if(this->mpcb->state==CLOSED)
    {
        link_flag[this->tcp_Num]&=~(1 << 5);//标记连接断开了
        return false;
    }
    if (link_flag[this->tcp_Num] & (1 << 5))  //是否连接成功?
        return true;
    else
        return false;
}

void TCP_Client_Class::send_data(char *data,uint16_t len) {
    sendbuf[this->tcp_Num].clear();
    sendbuf[this->tcp_Num].append(data,len);
    link_flag[this->tcp_Num]|=1 <<  7;		//标记有数据要发送
}

uint16_t TCP_Client_Class::available() const {
    return  recvbuf[this->tcp_Num].length();
}

void TCP_Client_Class::write(const char *str, uint16_t len) {
    send_data((char *)str,len);	//TCP发送数据
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















