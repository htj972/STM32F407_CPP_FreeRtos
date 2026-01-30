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
    if (Coils)    myfree(memx, Coils);
    if (Discrete) myfree(memx, Discrete);
    if (Input)    myfree(memx, Input);
    if (Holding)  myfree(memx, Holding);
}

bool USER_Register::init(uint16_t coil_num,
                         uint16_t discrete_num,
                         uint16_t input_num,
                         uint16_t holding_num)
{
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
