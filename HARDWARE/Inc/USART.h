/**
* @Author kokirika
* @Name UASRT
* @Date 2022-09-14
**/
#ifndef Kokirika_USART_H_
#define Kokirika_USART_H_

#include "GPIO.h"
#include "string"
#include <iostream>
#include <functional>
#include "HARD_BASE.h"
using namespace std;

class _USART_ {
protected:
    _GPIO_ RX_GPIO;
    _GPIO_ TX_GPIO;
    USART_TypeDef* USART;
private:
    uint8_t  config_flag=0;
    uint32_t Bound;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_Stream_TypeDef* DMAy_Streamx;
    uint32_t DMA_FLAG;
    uint32_t DMA_CHANNEL;
    uint8_t USART_Num;
    bool DMA_Enable;
    bool DMA_send_flag= false;
    void default_config();
    void GPIO_AF_config();
    static void extern_init();
    static char Interrupt_data[2];
    static char Interrupt_channel[2];
    static char Interrupt_name[2];
    static char* Interrupt_ret[3];

public:
    explicit _USART_(USART_TypeDef* USARTx,int32_t bound=115200);
    _USART_();
    ~_USART_()=default;
    void init(USART_TypeDef* USARTx,int32_t bound=115200);
    void config(GPIO_TypeDef* PORT_Tx,uint32_t Pin_Tx,GPIO_TypeDef* PORT_Rx,uint32_t Pin_Rx);
    void config(uint8_t Pin_Tx,uint8_t Pin_Rx);
    void setBound(int32_t bound);
    void setNVIC(uint8_t Priority=3,uint8_t SubPriority=3,bool EnAble=true);
    void setNVIC_ENABLE(bool EnAble=true);
    void setCmd_ENABLE(bool EnAble=true);
    void setStruct(uint16_t WordLength,uint16_t StopBits,uint16_t Parity);
    void set_fifo_size(uint16_t size) const;

    uint8_t get_USART_Num() const;

    void (*extern_IRQ_link)(){};
    void upload_extern_fun(void(* fun)());
    void (*extern_IRQ_link_r)(uint8_t){};
    void upload_extern_fun(void(* fun)(uint8_t));
    std::function<void()> localfunxx;
    void upload_extern_fun(std::function<void()> fun);
    void upload_extern_fun(Call_Back *extx) const;
    void upload_extern_close() const;

    static void extern_upset(uint8_t num,uint8_t data);

    void   send_re_data();
    uint16_t available() const;
    void   clear() const;
    string read_data() const;
    string read_data(uint8_t len) const;
    string read_data(char c) const ;
    string read_data(const string& str) const ;

    virtual void write(const char *str,uint16_t len);
    virtual void write(uint8_t *str,uint16_t len);
    virtual void write(string String);
    virtual uint16_t print(const char *fmt, ...);
    virtual uint16_t print(const string& String);
    virtual uint16_t print(const char *s);
    virtual uint16_t print(char s);
    virtual uint16_t print(int integer);
    uint16_t println(const char *fmt, ...);
    uint16_t println(string String);
    uint16_t println(const char *s);
    uint16_t println(int integer);

    void set_send_DMA(FunctionalState enable=ENABLE);

    _USART_& operator<<(const int integer)
    {
        this->print("%d", integer);
        return *this;
    }
    _USART_& operator<<(const char *s)
    {
        this->print("%s", s);
        return *this;
    }
    _USART_& operator<<(const string& Str)
    {
        this->print(Str);
        return *this;
    }

    _USART_& operator>>(string& Str)
    {
        Str.append(this->read_data());
        return *this;
    }
};


#endif //Kokirika_USART_H_
