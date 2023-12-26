/**
* @Author kokirika
* @Name WH_COM
* @Date 2023-12-12
*/
#ifndef Kokirika_WH_COM_H
#define Kokirika_WH_COM_H

#include <cstring>
#include "WH_L101.h"

#define COM_queue_num 5

class WH_COM : public _USART_, private Timer, public WH_L101{
private:
    typedef struct K_USER_data{
        float sun;
    }USER_data;

    typedef union K_POWER_DATA_{
        USER_data to_float;
        uint16_t  to_u16[sizeof(USER_data)/2]{};
        uint8_t   to_u8t[sizeof(USER_data)];
        K_POWER_DATA_() {
            memset(this->to_u8t,0,sizeof(this->to_u8t));
        };
    }K_POWER_DATA;
    bool queue_flag[COM_queue_num]{};
    bool queue_flag_r[COM_queue_num]{};
    float *datax[COM_queue_num]{};
    float valuex[COM_queue_num]{};
    uint16_t *datau[COM_queue_num]{};
    uint16_t valueu[COM_queue_num]{};
public:
    WH_COM(USART_TypeDef* USART,TIM_TypeDef *TIMx, uint16_t frq);
    K_POWER_DATA data_BUS;
    void initial();
    uint16_t find_address(const float *data);
    uint16_t find_address(const uint16_t *data);
    void     data_set(float *data,float value);
    void     data_set(uint16_t *data,uint16_t value);
    bool     data_sync();
};


#endif //Kokirika_WH_COM_H
