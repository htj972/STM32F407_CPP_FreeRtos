/**
* @Author kokirika
* @Name ONE_WIRE
* @Date 2022-10-25
**/

#include "ONE_Wire.h"
#include "delay.h"

void ONE_Wire::config(uint8_t Pinx,uint16_t err_t) {
    this->config_f= true;
    this->err_times=err_t;
    this->IO.init(Pinx,GPIO_Mode_OUT);
    IO_mode_flag = ONE_Wire::MODE::OUT;
    //this->IO.set_output(HIGH);
}

void ONE_Wire::config(GPIO_TypeDef *PORT_csl,uint32_t Pin_csl,uint16_t err_t) {
    this->config_f= true;
    this->err_times=err_t;
    this->IO.init(PORT_csl,Pin_csl,GPIO_Mode_OUT);
    IO_mode_flag = ONE_Wire::MODE::OUT;
    this->IO.set_output(HIGH);
}

void ONE_Wire::SET_IN()
{
    if(IO_mode_flag!=ONE_Wire::MODE::IN) {
        this->IO.set_pinmode(GPIO_Mode_IN);
        IO_mode_flag = ONE_Wire::MODE::IN;
    }
}

void ONE_Wire::SET_OUT(uint8_t Hi_Lo)
{
    if(IO_mode_flag!=ONE_Wire::MODE::OUT) {
        this->IO.set_pinmode(GPIO_Mode_OUT);
        IO_mode_flag = ONE_Wire::MODE::OUT;
    }
    this->IO.set_output(Hi_Lo);
}

uint16_t ONE_Wire::get_err() const
{
    return this->err_times;
}

bool ONE_Wire::check_err(uint16_t err) const {
    return err<this->err_times;
}

uint8_t ONE_Wire::get_in() {
    return this->IO.get_input();
}

bool ONE_Wire::Check()
{
    u8 retry=0;
    this->SET_IN();
    while (this->get_in()&&this->check_err(retry))
    {
        retry++;
        delay_us(1);
    };
    if(retry>=this->err_times)return false;
    else retry=0;
    while (!this->get_in()&&this->check_err(retry))
    {
        retry++;
        delay_us(1);
    };
    if(retry>=this->err_times)return false;
    return true;
}


