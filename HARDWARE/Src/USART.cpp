/**
* @Author kokirika
* @Name UASRT
* @Date 2022-09-14
**/

#include "USART.h"
#include <cstdarg>
#include <sstream>
#include "DMA.h"
#include "malloc.h"

#define UART_Max_num 6
struct UART_STRUCT_{
    bool extern_flag= false;
    Call_Back::MODE run_mode[UART_Max_num]{};
    void (*funC[UART_Max_num])(){};
    void (*funC_r[UART_Max_num])(uint8_t){};
    std::function<void()> funCPP[UART_Max_num+1];
    Call_Back *ext[UART_Max_num]{};
    string RX_buffer[UART_Max_num];
    uint16_t RX_MAX_LEN[UART_Max_num];
}UART_STRUCT;
void UART_RUN_VOID() {}

char _USART_::Interrupt_name[2];
char _USART_::Interrupt_data[2];
char _USART_::Interrupt_channel[2];
char* _USART_::Interrupt_ret[3];

_USART_::_USART_(USART_TypeDef* USARTx,int32_t bound){
    this->init(USARTx,bound);
    this->DMA_Enable= false;
    DMA_send_flag= false;
}

_USART_::_USART_(){
    this->DMA_Enable= false;
    DMA_send_flag= false;
}

void _USART_::GPIO_AF_config(){
    if(this->USART==USART1){
        this->TX_GPIO.set_AFConfig(GPIO_AF_USART1);
        this->RX_GPIO.set_AFConfig(GPIO_AF_USART1);
    }
    else if(this->USART==USART2) {
        this->TX_GPIO.set_AFConfig(GPIO_AF_USART2);
        this->RX_GPIO.set_AFConfig(GPIO_AF_USART2);
    }
    else if(this->USART==USART3) {
        this->TX_GPIO.set_AFConfig(GPIO_AF_USART3);
        this->RX_GPIO.set_AFConfig(GPIO_AF_USART3);
    }
    else if(this->USART==UART4) {
        this->TX_GPIO.set_AFConfig(GPIO_AF_UART4);
        this->RX_GPIO.set_AFConfig(GPIO_AF_UART4);
    }
    else if(this->USART==UART5) {
        this->TX_GPIO.set_AFConfig(GPIO_AF_UART5);
        this->RX_GPIO.set_AFConfig(GPIO_AF_UART5);
    }
    else if(this->USART==USART6) {
        this->TX_GPIO.set_AFConfig(GPIO_AF_USART6);
        this->RX_GPIO.set_AFConfig(GPIO_AF_USART6);
    }
}

void _USART_::config(GPIO_TypeDef *PORT_Tx, uint32_t Pin_Tx, GPIO_TypeDef *PORT_Rx, uint32_t Pin_Rx) {
    this->TX_GPIO.init(PORT_Tx,Pin_Tx,GPIO_Mode_AF);
    this->RX_GPIO.init(PORT_Rx,Pin_Rx,GPIO_Mode_AF);
    this->config_flag=1;
    this->GPIO_AF_config();
}

void _USART_::config(uint8_t Pin_Tx, uint8_t Pin_Rx) {
    this->TX_GPIO.init(Pin_Tx,GPIO_Mode_AF);
    this->RX_GPIO.init(Pin_Rx,GPIO_Mode_AF);
    this->config_flag=1;
    this->GPIO_AF_config();
}

void _USART_::default_config() {
    if(this->USART==USART1){
        this->USART_Num=0;
        if(this->config_flag==0)
            this->config(GPIOA9,GPIOA10);
    }
    else if(this->USART==USART2) {
        this->USART_Num = 1;
        if(this->config_flag==0)
            this->config(GPIOA2, GPIOA3);
    }
    else if(this->USART==USART3) {
        this->USART_Num = 2;
        if(this->config_flag==0)
            this->config(GPIOB10, GPIOB11);
    }
    else if(this->USART==UART4) {
        this->USART_Num = 3;
        if(this->config_flag==0)
            this->config(GPIOC10, GPIOC11);
    }
    else if(this->USART==UART5) {
        this->USART_Num = 4;
        if(this->config_flag==0)
            this->config(GPIOC12, GPIOD2);
    }
    else if(this->USART==USART6) {
        this->USART_Num = 5;
        if(this->config_flag==0)
            this->config(GPIOC6, GPIOC7);
    }
}

void _USART_::init(USART_TypeDef* USARTx,int32_t bound) {
    this->Bound=bound;
    this->USART=USARTx;
    _USART_::extern_init();
    this->default_config();
    if(USARTx==USART1)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
    else if(USARTx==USART2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
    else if(USARTx==USART3)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
    else if(USARTx==UART4)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
    else if(USARTx==UART5)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
    else if(USARTx==USART6)
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART6,ENABLE);

    this->USART_InitStructure.USART_BaudRate = this->Bound;//波特率设置
    this->USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    this->USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    this->USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    this->USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    this->USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(this->USART, &USART_InitStructure); //初始化串口
    USART_Cmd(this->USART, ENABLE);  //使能串口
    USART_ITConfig(this->USART, USART_IT_RXNE, ENABLE);//开启相关中断
    USART_ClearFlag(this->USART, USART_FLAG_TC);

    if(USARTx==USART1) {
        this->NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;//串口1中断通道
    }
    else if(USARTx==USART2){
        NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;//串口2中断通道
    }
    else if(USARTx==USART3){
        NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//串口3中断通道
    }
    else if(USARTx==UART4){
        NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//串口4中断通道
    }
    else if(USARTx==UART5){
        NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;//串口5中断通道
    }
    else if(USARTx==USART6){
        NVIC_InitStructure.NVIC_IRQChannel = USART6_IRQn;//串口6中断通道
    }

    this->NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3; //抢占优先级3
    this->NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
    this->NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&this->NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
}

void _USART_::setBound(int32_t bound){
    USART_Cmd(this->USART, DISABLE);  //使能串口
    this->USART_InitStructure.USART_BaudRate = this->Bound;//波特率设置
    USART_Init(this->USART, &USART_InitStructure); //初始化串口
    USART_Cmd(this->USART, ENABLE);  //使能串口
}

void _USART_::setNVIC(uint8_t Priority, uint8_t SubPriority, bool EnAble) {
    this->NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=Priority;
    this->NVIC_InitStructure.NVIC_IRQChannelSubPriority=SubPriority;
    NVIC_Init(&this->NVIC_InitStructure);
    this->setNVIC_ENABLE(EnAble);
}

void _USART_::setNVIC_ENABLE(bool EnAble) {
    if(!EnAble)
        USART_ITConfig(this->USART, USART_IT_RXNE, DISABLE);
    else
        USART_ITConfig(this->USART, USART_IT_RXNE, ENABLE);
}

void _USART_::setCmd_ENABLE(bool EnAble) {
    if(!EnAble)
        USART_Cmd(this->USART, DISABLE);  //失能串口
    else
        USART_Cmd(this->USART, ENABLE);  //使能串口
}

void _USART_::setStruct(uint16_t WordLength, uint16_t StopBits, uint16_t Parity) {
    this->USART_InitStructure.USART_WordLength = WordLength;//字长为8位数据格式
    this->USART_InitStructure.USART_StopBits = StopBits;//一个停止位
    this->USART_InitStructure.USART_Parity = Parity;//无奇偶校验位
    USART_Init(this->USART, &this->USART_InitStructure); //初始化串口
}

void _USART_::set_fifo_size(uint16_t size) {
    if(UART_STRUCT.RX_MAX_LEN[this->USART_Num]>size)
    {
        uint16_t len_t=UART_STRUCT.RX_MAX_LEN[this->USART_Num]-size;
        if(UART_STRUCT.RX_buffer[this->USART_Num].length()>size)
            UART_STRUCT.RX_buffer[this->USART_Num].erase(0,len_t);
    }
    UART_STRUCT.RX_MAX_LEN[this->USART_Num]=size;
}

uint8_t _USART_::get_USART_Num() {
    return this->USART_Num;
}

static char Interrupt_data[2];
static char Interrupt_name[2];
static char* Interrupt_ret[2];

void _USART_::extern_init() {
    if(!UART_STRUCT.extern_flag) {
        UART_STRUCT.extern_flag = true;
        for(auto & ii : UART_STRUCT.funC)
            ii=UART_RUN_VOID;
        for(auto & ii : UART_STRUCT.run_mode)
            ii=Call_Back::MODE::C_fun;
        for(auto & ii : UART_STRUCT.RX_MAX_LEN)
            ii=1024;
        _USART_::Interrupt_ret[0]=_USART_::Interrupt_name;
        _USART_::Interrupt_ret[1]=_USART_::Interrupt_channel;
        _USART_::Interrupt_ret[2]=_USART_::Interrupt_data;
        _USART_::Interrupt_name[0]=Call_Back::Name::uart;
    }
}

void _USART_::extern_upset(uint8_t num,uint8_t data)
{
    if(UART_STRUCT.run_mode[num%UART_Max_num]==Call_Back::MODE::C_fun) {
        UART_STRUCT.funC[num % UART_Max_num]();
        UART_STRUCT.RX_buffer[num%UART_Max_num] += data;
    }
    else if(UART_STRUCT.run_mode[num%UART_Max_num]==Call_Back::MODE::C_fun_r)
        UART_STRUCT.funC_r[num%UART_Max_num](data);
    else if(UART_STRUCT.run_mode[num%UART_Max_num]==Call_Back::MODE::CPP_fun) {
        UART_STRUCT.funCPP[num % UART_Max_num]();
        UART_STRUCT.RX_buffer[num%UART_Max_num] += data;
    }
    else if(UART_STRUCT.run_mode[num%UART_Max_num]==Call_Back::MODE::class_fun)
    {
        _USART_::Interrupt_data[0] = data;
        _USART_::Interrupt_channel[0] = num;
        UART_STRUCT.ext[num%UART_Max_num]->Callback(2,_USART_::Interrupt_ret);
    }
}

void _USART_::upload_extern_fun(void (*fun)()) {
    UART_STRUCT.funC[this->USART_Num%UART_Max_num]=fun;
    UART_STRUCT.run_mode[this->USART_Num%UART_Max_num]=Call_Back::MODE::C_fun;
    this->extern_IRQ_link=fun;
}

void _USART_::upload_extern_fun(void (*fun)(uint8_t)) {
    UART_STRUCT.funC_r[this->USART_Num%UART_Max_num]=fun;
    UART_STRUCT.run_mode[this->USART_Num%UART_Max_num]=Call_Back::MODE::C_fun_r;
    this->extern_IRQ_link_r=fun;
}

void _USART_::upload_extern_fun(std::function<void()> fun) {
    this->localfunxx=fun;
    UART_STRUCT.funCPP[this->USART_Num%UART_Max_num]=std::move(fun);
    UART_STRUCT.run_mode[this->USART_Num%UART_Max_num]=Call_Back::MODE::CPP_fun;
}

void _USART_::upload_extern_fun(Call_Back *extx) const {
    UART_STRUCT.ext[this->USART_Num%UART_Max_num]=extx;
    UART_STRUCT.run_mode[this->USART_Num%UART_Max_num]=Call_Back::MODE::class_fun;
}

void _USART_::send_re_data() {
    this->write(UART_STRUCT.RX_buffer[this->USART_Num]);
}

uint16_t _USART_::available() {
    return  UART_STRUCT.RX_buffer[this->USART_Num].length();
}

void _USART_::clear() {
    UART_STRUCT.RX_buffer[this->USART_Num].clear();
}

string _USART_::read_data() {
    string  ret=UART_STRUCT.RX_buffer[this->USART_Num];
    UART_STRUCT.RX_buffer[this->USART_Num].clear();
    return ret;
}

string _USART_::read_data(uint8_t len) const {
    string  ret=UART_STRUCT.RX_buffer[this->USART_Num].substr(0,len);
    UART_STRUCT.RX_buffer[this->USART_Num].erase(0,len);
    return ret;
}

string _USART_::read_data(char c) const {
    uint16_t len_t;
    string  ret;
    len_t=UART_STRUCT.RX_buffer[this->USART_Num].find(c);
    if(len_t!=65535)
        len_t+=1;
    else
        len_t=0;
    ret=UART_STRUCT.RX_buffer[this->USART_Num].substr(0,len_t);
    UART_STRUCT.RX_buffer[this->USART_Num].erase(0,len_t);
    return ret;
}

string _USART_::read_data(const string& str) const {
    uint16_t len_t;
    string  ret;
    len_t=UART_STRUCT.RX_buffer[this->USART_Num].find(str);
    if(len_t!=65535)
        len_t+=str.length();
    else
        len_t=0;
    ret=UART_STRUCT.RX_buffer[this->USART_Num].substr(0,len_t);
    UART_STRUCT.RX_buffer[this->USART_Num].erase(0,len_t);
    return ret;
}

void _USART_::write(const char *str, uint16_t len) {
    if(!DMA_Enable) {
        uint16_t ii = 0;
        while (ii != len) {
            while ((this->USART->SR & 0X40) == 0);
//        while(USART_GetFlagStatus(this->USART, USART_FLAG_TXE)==RESET);
            this->USART->DR = *str;
            str++;
            ii++;
        }
    }
    else {
        if(!this->DMA_send_flag)
            this->DMA_send_flag = true;
        else if(DMA_GetFlagStatus(this->DMAy_Streamx,this->DMA_FLAG)!=RESET)//等待DMA2_Steam7传输完成
            {
                DMA_ClearFlag(this->DMAy_Streamx, this->DMA_FLAG);//清除DMA2_Steam7传输完成标志
            }
        DMA_send(this->DMAy_Streamx,this->DMA_CHANNEL,\
        (u32)&this->USART->DR,(uint32_t)str,len,\
        DMA_DIR_MemoryToPeripheral,8);
    }
}

void _USART_::write(uint8_t *str, uint16_t len) {
    this->write((char*)str,len);
}

void _USART_::write(std::string String) {
    const char* p=String.c_str();
    this->write(p,String.length());
}

uint16_t _USART_::print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    std::string r;
    r.resize(static_cast<size_t>(len) + 1);
    va_start(args, fmt);
    vsnprintf(&r.front(), len + 1, fmt, args);
    va_end(args);
    r.resize(static_cast<size_t>(len));
    this->write(r);
    return r.length();
}

uint16_t _USART_::print(const std::string& String) {
    this->write(String);
    return String.length();
}

uint16_t _USART_::print(const char *s) {
    return this->print("%s",s);
}

uint16_t _USART_::print(char s) {
    return this->print("%c",s);
}

uint16_t _USART_::print(int integer) {
    return this->print("%d",integer);
}

uint16_t _USART_::println(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    std::string r;
    r.resize(static_cast<size_t>(len) + 1);
    va_start(args, fmt);
    vsnprintf(&r.front(), len + 1, fmt, args);
    va_end(args);
    r.resize(static_cast<size_t>(len));
    r.append("\r\n");
    this->write(r);
    return r.length();
}

uint16_t _USART_::println(std::string String) {
    String.append("\r\n");
    this->write(String);
    return String.length();
}

uint16_t _USART_::println(const char *s) {
    return this->println("%s",s);
}

uint16_t _USART_::println(int integer) {
    return this->println("%d",integer);
}

void  _USART_::set_send_DMA(FunctionalState enable) {
    USART_DMACmd(this->USART,USART_DMAReq_Tx,enable);  //使能串口1的DMA发送
    this->DMA_Enable= (enable == ENABLE);
    switch (this->USART_Num) {
        case 0:
            this->DMAy_Streamx=DMA2_Stream7;
            this->DMA_FLAG=DMA_FLAG_TCIF7;
            this->DMA_CHANNEL=DMA_Channel_4;
            break;
        case 1:
            this->DMAy_Streamx=DMA1_Stream6;
            this->DMA_FLAG=DMA_FLAG_TCIF6;
            this->DMA_CHANNEL=DMA_Channel_4;
            break;
        case 2:
            this->DMAy_Streamx=DMA1_Stream3;
            this->DMA_FLAG=DMA_FLAG_TCIF3;
            this->DMA_CHANNEL=DMA_Channel_4;
            break;
        case 3:
            this->DMAy_Streamx=DMA1_Stream4;
            this->DMA_FLAG=DMA_FLAG_TCIF4;
            this->DMA_CHANNEL=DMA_Channel_4;
            break;
        case 4:
            this->DMAy_Streamx=DMA1_Stream7;
            this->DMA_FLAG=DMA_FLAG_TCIF7;
            this->DMA_CHANNEL=DMA_Channel_4;
            break;
        case 5:
            this->DMAy_Streamx=DMA2_Stream6;
            this->DMA_FLAG=DMA_FLAG_TCIF6;
            this->DMA_CHANNEL=DMA_Channel_5;
            break;
    }
}

extern "C" void USART1_IRQHandler()  {              	//串口1中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART1);	//读取接收到的数据
        _USART_::extern_upset(0,Res);
    }
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

extern "C" void USART2_IRQHandler() {               	//串口2中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART2);	//读取接收到的数据
        _USART_::extern_upset(1,Res);
    }
    USART_ClearFlag(USART2,USART_IT_RXNE);
}

extern "C" void USART3_IRQHandler()  {              	//串口3中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART3);	//读取接收到的数据
        _USART_::extern_upset(2,Res);
    }
    USART_ClearFlag(USART3,USART_IT_RXNE);
}

extern "C" void UART4_IRQHandler() {              	//串口4中断服务程序
    u8 Res;
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(UART4);	//读取接收到的数据
        _USART_::extern_upset(3,Res);
    }
    USART_ClearFlag(UART4,USART_IT_RXNE);
}

extern "C" void UART5_IRQHandler() {                	//串口5中断服务程序
    u8 Res;
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(UART5);	//读取接收到的数据
        _USART_::extern_upset(4,Res);
    }
    USART_ClearFlag(UART5,USART_IT_RXNE);
}

extern "C" void USART6_IRQHandler() {                	//串口6中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART6);	//读取接收到的数据
        _USART_::extern_upset(5,Res);
    }
    USART_ClearFlag(USART6,USART_IT_RXNE);
}