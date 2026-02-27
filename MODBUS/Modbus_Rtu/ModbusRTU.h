/**
* @Author kokirika
 * @Name ModbusRTU
 * @Date 2026-02-27
 */

#ifndef KOKIRIKA_MODBUS_RTU_H
#define KOKIRIKA_MODBUS_RTU_H

#pragma once

#include <cstdint>
#include "ModbusBase.h"

/**
 * @brief Modbus RTU（基于 ModbusBase）
 *
 * - 负责：地址匹配、CRC、RTU 帧封装/解析
 * - 不负责：功能码逻辑（全部在 ModbusBase）
 */
class ModbusRTU : public ModbusBase {
public:
    explicit ModbusRTU(uint8_t slaveId = 1);

    void setSlaveId(uint8_t id);
    uint8_t getSlaveId() const;

    /**
     * @brief 处理一帧 RTU 报文
     *
     * @param rxBuf   接收缓冲（完整 RTU 帧）
     * @param rxLen   接收长度
     * @param txBuf   发送缓冲（输出 RTU 帧）
     * @param txLen   输出长度
     *
     * @return true   生成了响应帧（txBuf/txLen 有效）
     * @return false  不需要响应（地址不匹配、CRC 错、广播、或报文无效）
     */
    bool handleFrame(const uint8_t* rxBuf, uint16_t rxLen,
                     uint8_t* txBuf, uint16_t& txLen);

private:
    uint8_t slaveId_;

    static uint16_t crc16_modbus(const uint8_t* data, uint16_t len);
    static bool checkCrc(const uint8_t* frame, uint16_t len);
    static void appendCrc(uint8_t* frame, uint16_t lenWithoutCrc);
};

#endif // KOKIRIKA_MODBUS_RTU_H