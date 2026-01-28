/**
* @Author kokirika
* @Name ModbusBase
* @Date 2026-1-26
**/

#ifndef KOKIRIKA_MODBUSBASE_H
#define KOKIRIKA_MODBUSBASE_H

#pragma once
#include "sys.h"
#include <cstdint>
#include <cstring>

class ModbusBase {
public:
    ModbusBase();

    void bindCoils(uint8_t*, uint16_t);   // ? 绑定线圈
    void bindDiscrete(uint8_t*, uint16_t);// ? 绑定离散输入
    void bindHolding(uint16_t*, uint16_t);// ? 绑定保持寄存器
    void bindInput(uint16_t*, uint16_t);  // ? 绑定输入寄存器

    // ? PDU 处理（RTU / TCP 共用）
    bool processPDU(uint8_t *req, uint16_t reqLen,
                    uint8_t *resp, uint16_t &respLen);

protected:
    uint8_t  *coils;
    uint8_t  *discrete;
    uint16_t *holding;
    uint16_t *input;

    uint16_t coilSize, discSize, holdSize, inputSize;

    // 功能码
    void FC01(const uint8_t*, uint8_t*, uint16_t&);
    void FC02(const uint8_t*, uint8_t*, uint16_t&);
    void FC03(const uint8_t*, uint8_t*, uint16_t&);
    void FC04(const uint8_t*, uint8_t*, uint16_t&);
    void FC05(uint8_t*, uint8_t*, uint16_t&);
    void FC06(uint8_t*, uint8_t*, uint16_t&);
    void FC15(uint8_t*, uint8_t*, uint16_t&);
    void FC16(uint8_t*, uint8_t*, uint16_t&);

    static void exception(uint8_t func, uint8_t code,
                   uint8_t *resp, uint16_t &len);

    static bool checkRange(uint16_t addr, uint16_t num, uint16_t max);
};


#endif //KOKIRIKA_MODBUSBASE_H
