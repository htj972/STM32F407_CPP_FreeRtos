/**
* @Author kokirika
* @Name UASRT
* @Date 2022-09-14
**/

#include "USART.h"
#include <cstdarg>
#include <sstream>
#include "DMA.h"

static string RX_buffer[6];
static uint16_t RX_MAX_LEN[6]={1024,1024,1024,1024,1024,1024};
static uint8_t  extern_flag=0;
void USART_RUN_VOID(uint8_t channel,uint8_t data) {}
struct _IRQ_STRUCT_ {
    void (*Usart_IRQ_link)(uint8_t channel,uint8_t data);
    void (*extern_IRQ_link)(uint8_t channel,uint8_t data);
}HARD_IQR;

_USART_::_USART_(USART_TypeDef* USARTx,int32_t bound){
    this->init(USARTx,bound);
}


void _USART_::config(GPIO_TypeDef *PORT_Tx, uint32_t Pin_Tx, GPIO_TypeDef *PORT_Rx, uint32_t Pin_Rx) {
    this->TX_GPIO.init(PORT_Tx,Pin_Tx,GPIO_Mode_AF);
    this->RX_GPIO.init(PORT_Rx,Pin_Rx,GPIO_Mode_AF);
    this->config_flag=1;
}

void _USART_::config(uint8_t Pin_Tx, uint8_t Pin_Rx) {
    this->TX_GPIO.init(Pin_Tx,GPIO_Mode_AF);
    this->RX_GPIO.init(Pin_Rx,GPIO_Mode_AF);
    this->config_flag=1;
}

void _USART_::default_config() {
    if(this->USART==USART1){
        this->USART_Num=0;
        if(this->config_flag==0)
            this->config(GPIOA9,GPIOA10);
        this->TX_GPIO.set_AFConfig(GPIO_AF_USART1);
        this->RX_GPIO.set_AFConfig(GPIO_AF_USART1);
    }
    else if(this->USART==USART2) {
        this->USART_Num = 1;
        if(this->config_flag==0)
            this->config(GPIOA2, GPIOA3);
        this->TX_GPIO.set_AFConfig(GPIO_AF_USART2);
        this->RX_GPIO.set_AFConfig(GPIO_AF_USART2);
    }
    else if(this->USART==USART3) {
        this->USART_Num = 2;
        if(this->config_flag==0)
            this->config(GPIOB10, GPIOB11);
        this->TX_GPIO.set_AFConfig(GPIO_AF_USART3);
        this->RX_GPIO.set_AFConfig(GPIO_AF_USART3);
    }
    else if(this->USART==UART4) {
        this->USART_Num = 3;
        if(this->config_flag==0)
            this->config(GPIOC10, GPIOC11);
        this->TX_GPIO.set_AFConfig(GPIO_AF_UART4);
        this->RX_GPIO.set_AFConfig(GPIO_AF_UART4);
    }
    else if(this->USART==UART5) {
        this->USART_Num = 4;
        if(this->config_flag==0)
            this->config(GPIOC12, GPIOD2);
        this->TX_GPIO.set_AFConfig(GPIO_AF_UART5);
        this->RX_GPIO.set_AFConfig(GPIO_AF_UART5);
    }
    else if(this->USART==USART6) {
        this->USART_Num = 5;
        if(this->config_flag==0)
            this->config(GPIOC6, GPIOC7);
        this->TX_GPIO.set_AFConfig(GPIO_AF_USART6);
        this->RX_GPIO.set_AFConfig(GPIO_AF_USART6);
    }
}

void _USART_::init(USART_TypeDef* USARTx,int32_t bound) {
    this->Bound=bound;
    this->USART=USARTx;
    if(extern_flag==0) {
        _USART_::upload_extern_fun(USART_RUN_VOID);
    }
    HARD_IQR.Usart_IRQ_link=_USART_::send_to_fifo;
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
    if(RX_MAX_LEN[this->USART_Num]>size)
    {
        uint16_t len_t=RX_MAX_LEN[this->USART_Num]-size;
        if(RX_buffer[this->USART_Num].length()>size)
            RX_buffer[this->USART_Num].erase(0,len_t);
    }
    RX_MAX_LEN[this->USART_Num]=size;
}

void _USART_::upload_extern_fun(void (*fun)(uint8_t, uint8_t)) {
    HARD_IQR.extern_IRQ_link=fun;
    extern_flag=1;
}

void _USART_::send_to_fifo(uint8_t channel,uint8_t data) {
    RX_buffer[channel]+=data;
    if(RX_buffer[channel].length()>RX_MAX_LEN[channel])
        RX_buffer[channel].erase(0,1);
}

void _USART_::send_data() {
    this->write(RX_buffer[this->USART_Num]);
}

uint16_t _USART_::available() {
    return  RX_buffer[this->USART_Num].length();
}

string _USART_::read_data() const {
    string  ret=RX_buffer[this->USART_Num];
    RX_buffer[this->USART_Num].clear();
    return ret;
}

string _USART_::read_data(uint8_t len) const {
    string  ret=RX_buffer[this->USART_Num].substr(0,len);
    RX_buffer[this->USART_Num].erase(0,len);
    return ret;
}

string _USART_::read_data(char c) const {
    uint16_t len_t;
    string  ret;
    len_t=RX_buffer[this->USART_Num].find(c);
    if(len_t!=65535)
        len_t+=1;
    else
        len_t=0;
    ret=RX_buffer[this->USART_Num].substr(0,len_t);
    RX_buffer[this->USART_Num].erase(0,len_t);
    return ret;
}

string _USART_::read_data(const string& str) const {
    uint16_t len_t;
    string  ret;
    len_t=RX_buffer[this->USART_Num].find(str);
    if(len_t!=65535)
        len_t+=str.length();
    else
        len_t=0;
    ret=RX_buffer[this->USART_Num].substr(0,len_t);
    RX_buffer[this->USART_Num].erase(0,len_t);
    return ret;
}

void _USART_::write(const char *str, uint16_t len) {
    if(DMA_Enable==OFF) {
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
    this->DMA_Enable=(enable==ENABLE)?ON:OFF;
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
        HARD_IQR.Usart_IRQ_link(0,Res);
        HARD_IQR.extern_IRQ_link(0,Res);
    }
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

extern "C" void USART2_IRQHandler() {               	//串口2中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART2);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(1,Res);
        HARD_IQR.extern_IRQ_link(1,Res);
    }
    USART_ClearFlag(USART2,USART_IT_RXNE);
}

extern "C" void USART3_IRQHandler()  {              	//串口3中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART3);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(2,Res);
        HARD_IQR.extern_IRQ_link(2,Res);
    }
    USART_ClearFlag(USART3,USART_IT_RXNE);
}

extern "C" void UART4_IRQHandler() {              	//串口4中断服务程序
    u8 Res;
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(UART4);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(3,Res);
        HARD_IQR.extern_IRQ_link(3,Res);
    }
    USART_ClearFlag(UART4,USART_IT_RXNE);
}

extern "C" void UART5_IRQHandler() {                	//串口5中断服务程序
    u8 Res;
    if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(UART5);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(4,Res);
        HARD_IQR.extern_IRQ_link(4,Res);
    }
    USART_ClearFlag(UART5,USART_IT_RXNE);
}

extern "C" void USART6_IRQHandler() {                	//串口6中断服务程序
    u8 Res;
    if(USART_GetITStatus(USART6, USART_IT_RXNE) != RESET){
        Res =USART_ReceiveData(USART6);	//读取接收到的数据
        HARD_IQR.Usart_IRQ_link(5,Res);
        HARD_IQR.extern_IRQ_link(5,Res);
    }
    USART_ClearFlag(USART6,USART_IT_RXNE);
}