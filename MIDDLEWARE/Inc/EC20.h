/**
* @Author kokirika
* @Name EC20
* @Date 2023-07-18
*/
#ifndef KOKIRIKA_EC20_H
#define KOKIRIKA_EC20_H

#include "USART.h"

class EC20 {
private:
    _USART_ *USART{};
    bool  new_flag{};
    uint8_t delay_time{};
    static bool Compare(const string& target,const string& data);
public:
    explicit EC20(USART_TypeDef* USARTx,int32_t bound=115200);
    EC20()=default;
    ~EC20();
    void init(USART_TypeDef* USARTx,int32_t bound=115200);
    void init(_USART_* USARTx);
    string init();
    void set_delay_times(uint8_t delay_times);

//    void write(const char *str,uint16_t len) override;
//    void write(uint8_t *str,uint16_t len) override;
//    void write(string String) override;
};


#endif //KOKIRIKA_EC20_H
