/**
* @Author kokirika
* @Name USER_REGISTER
* @Date 2026-1-30
**/

#include "USER_Register.h"
#include <cstring>

USER_Register::USER_Register(uint8_t memx)
    : memx(memx),
      coil_count(0),
      discrete_count(0),
      input_count(0),
      holding_count(0)
{
}

USER_Register::~USER_Register()
{
    if (Coils) {
        myfree(memx, Coils);
        Coils = nullptr;
    }
    if (Discrete) {
        myfree(memx, Discrete);
        Discrete = nullptr;
    }
    if (Input) {
        myfree(memx, Input);
        Input = nullptr;
    }
    if (Holding) {
        myfree(memx, Holding);
        Holding = nullptr;
    }
}

USER_Register& USER_Register::instance(uint8_t mem)
{
    static USER_Register inst(mem);
    return inst;
}

bool USER_Register::init(uint16_t coil_num,
                         uint16_t discrete_num,
                         uint16_t input_num,
                         uint16_t holding_num)
{

    if (inited) return false;   // 防止重复 init
    coil_count     = coil_num;
    discrete_count = discrete_num;
    input_count    = input_num;
    holding_count  = holding_num;

    uint32_t coil_bytes     = (coil_num + 7) / 8;
    uint32_t discrete_bytes = (discrete_num + 7) / 8;

    Coils    = (uint8_t*)mymalloc(memx, coil_bytes);
    Discrete = (uint8_t*)mymalloc(memx, discrete_bytes);
    Input    = (uint16_t*)mymalloc(memx, input_num * sizeof(uint16_t));
    Holding  = (uint16_t*)mymalloc(memx, holding_num * sizeof(uint16_t));

    if (!Coils || !Discrete || !Input || !Holding) {
        return false;
    }

    memset(Coils,    0, coil_bytes);
    memset(Discrete, 0, discrete_bytes);
    memset(Input,    0, input_num * sizeof(uint16_t));
    memset(Holding,  0, holding_num * sizeof(uint16_t));

    inited = true;
    return true;
}

/* ===== getter ===== */

uint8_t* USER_Register::getCoils() {
    return Coils;
}

uint8_t* USER_Register::getDiscrete() {
    return Discrete;
}

uint16_t* USER_Register::getInput() {
    return Input;
}

uint16_t* USER_Register::getHolding() {
    return Holding;
}

/* ===== count ===== */

uint16_t USER_Register::getCoilCount() const {
    return coil_count;
}

uint16_t USER_Register::getDiscreteCount() const {
    return discrete_count;
}

uint16_t USER_Register::getInputCount() const {
    return input_count;
}

uint16_t USER_Register::getHoldingCount() const {
    return holding_count;
}

void USER_Register::Callback(int, char **data) {
    if(data[0][0]!=Call_Back::Name::exit)
        return;
    uint8_t index;
    for (index = 0; index < this->pulse_index; index++) {
        if (data[1][0] == this->input_ptr[index]->get_pin_num()) {
            uint8_t pin = index + 16;
            if(this->Discrete!= nullptr && pin < this->discrete_count) {
                uint8_t byte_index = pin / 8;
                uint8_t bit_index  = pin % 8;
                if(this->input_ptr[index]->get_state()) {
                    this->Discrete[byte_index] |= (1 << bit_index);
                } else {
                    this->Discrete[byte_index] &= ~(1 << bit_index);
                }
            }
            pin = (index + 4)*2;
            if(this->Input!= nullptr && pin+1 < this->input_count) {
                /*将输入寄存器转换成Uint32_t
                 * 再进行++处理*/
                auto* input32 = reinterpret_cast<uint32_t*>(this->Input + pin);
                (*input32)++;
            }
            break;
        }
    }
}

void USER_Register::Add_Pulse_Pin(_InPut_ *Pin) {
    if (this->pulse_index < Pulse_MAX_Num) {
        this->input_ptr[this->pulse_index] = Pin;
        this->input_ptr[this->pulse_index]->set_EXTI();//设置外部中断
        this->input_ptr[this->pulse_index]->set_Trigger(EXTI_Trigger_Rising_Falling);//上升沿触发
        this->input_ptr[this->pulse_index]->upload_extern_fun(this);
        this->pulse_index++;
    }
}
