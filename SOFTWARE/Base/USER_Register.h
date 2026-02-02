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
#include "HARD_BASE.h"
#include "Out_In_Put.h"

class USER_Register:public Call_Back
{
#define Pulse_MAX_Num 2
private:
    _InPut_ *input_ptr[Pulse_MAX_Num];
    uint8_t pulse_index = 0;
    explicit USER_Register(uint8_t mem);
    ~USER_Register();
    // Y (bit-packed)
    uint8_t*  Coils = nullptr;
    // X (bit-packed)
    uint8_t*  Discrete = nullptr;
    // M
    uint16_t* Input = nullptr;
    // D
    uint16_t* Holding = nullptr;

    uint16_t coil_count;
    uint16_t discrete_count;
    uint16_t input_count;
    uint16_t holding_count;

    uint8_t memx = 0;   // 内存区编号

    bool inited;

public:
    /* 获取全局唯一实例 */
    static USER_Register& instance(uint8_t mem);
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

    /* ===== 数量获取（点数 / 寄存器数） ===== */
    uint16_t getCoilCount() const;
    uint16_t getDiscreteCount() const;
    uint16_t getInputCount() const;
    uint16_t getHoldingCount() const;

    void Add_Pulse_Pin(_InPut_ *Pin);
    void Callback(int ,char** data) override;
};

#endif // KOKIRIKA_USER_REGISTER_H
