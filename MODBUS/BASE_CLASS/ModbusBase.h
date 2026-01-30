/**
* @Author kokirika
* @Name ModbusBase
* @Date 2026-1-26
**/

#ifndef KOKIRIKA_MODBUSBASE_H
#define KOKIRIKA_MODBUSBASE_H

#pragma once
#include <cstdint>
#include <cstring>
#include "Storage_BASE.h"

/**
 * @brief Modbus 协议核心（Storage 模式）
 *
 * 特点：
 * - 只处理 Modbus PDU
 * - RTU / TCP 共用
 * - Holding Register 直接映射到 Storage_BASE
 * - FRAM / Flash / EEPROM 通用
 */
class ModbusBase {
public:
    ModbusBase();

    /* ================= 绑定接口 ================= */

    // ---- Coil / Discrete / Input：RAM 映像 ----
    void bindCoils(uint8_t* coils, uint16_t len);
    void bindDiscrete(uint8_t* discrete, uint16_t len);
    void bindInput(uint16_t* input, uint16_t len);

    // ---- Holding：RAM 映像 ----
    bool bindHoldingRam(uint16_t startAddr,
                        uint16_t* buf,
                        uint16_t len);

    /**
     * @brief 绑定 Holding Register 到存储设备
     * @param dev       Storage 设备（FRAM / Flash / EEPROM）
     * @param baseAddr  存储中的起始字节地址
     * @param regCount  寄存器数量（16bit 为单位）
     */
    bool bindHoldingStorage(uint16_t startAddr,
                            Storage_BASE* dev,
                            uint32_t baseAddr,
                            uint16_t len);



    // ? PDU 处理（RTU / TCP 共用）
    bool processPDU(uint8_t *req, uint16_t reqLen,
                    uint8_t *resp, uint16_t &respLen);

protected:
    uint8_t*  coils     = nullptr;
    uint8_t*  discrete  = nullptr;
    uint16_t* input     = nullptr;

    uint16_t coilSize  = 0;
    uint16_t discSize  = 0;
    uint16_t inputSize = 0;

    /* ================= Holding（两种模式） ================= */
    // Storage（FRAM）模式
    struct HoldingSegment {
        uint16_t      start;     // Modbus 起始地址
        uint16_t      length;    // 寄存器数量
        uint16_t*     ram;       // RAM 映像（可空）
        Storage_BASE* storage;   // FRAM（可空）
        uint32_t      baseAddr;  // Storage 起始字节地址
    };

    static constexpr uint8_t MAX_HOLD_SEG = 4;
    HoldingSegment holdSeg[MAX_HOLD_SEG]{};
    uint8_t holdSegNum;

    /* ================= 内部工具 ================= */
    HoldingSegment* findHoldingSeg(uint16_t addr, uint16_t num);


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
