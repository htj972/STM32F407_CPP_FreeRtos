/**
* @Author kokirika
* @Name USER_REGISTER
* @Date 2026-1-30
**/

#ifndef KOKIRIKA_USER_REGISTER_H
#define KOKIRIKA_USER_REGISTER_H

#pragma once
#include "sys.h"
#include "malloc.h"

class USER_Register
{
private:
    // Y
    uint8_t*  Coils = nullptr;
    // X
    uint8_t*  Discrete = nullptr;
    // M
    uint16_t* Input = nullptr;
    // D
    uint16_t* Holding = nullptr;

    uint16_t coil_count;
    uint16_t discrete_count;
    uint16_t input_count;
    uint16_t holding_count;
    uint8_t memx={};   // ★ 内存区编号
public:
    explicit USER_Register(uint8_t mem);
    ~USER_Register();

    /* ===== 初始化（申请内存） ===== */
    bool init(uint16_t coil_num,
              uint16_t discrete_num,
              uint16_t input_num,
              uint16_t holding_num);

    /* ===== 指针获取 ===== */
    uint8_t*  getCoils();
    uint8_t*  getDiscrete();
    uint16_t* getInput();
    uint16_t* getHolding();

    /* ===== 数量获取 ===== */
    uint16_t getCoilCount() const;
    uint16_t getDiscreteCount() const;
    uint16_t getInputCount() const;
    uint16_t getHoldingCount() const;
};



#endif //KOKIRIKA_USER_REGISTER_H
