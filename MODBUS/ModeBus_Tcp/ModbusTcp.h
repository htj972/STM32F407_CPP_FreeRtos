/**
* @Author kokirika
* @Name ModbusTcp
* @Date 2026-1-26
**/

#ifndef KOKIRIKA_MODBUSTCP_H
#define KOKIRIKA_MODBUSTCP_H

#pragma once
#include <cstdint>
#include <string>
#include "ModbusBase.h"

class ModbusTCP : public ModbusBase {
public:
    ModbusTCP() = default;

    /**
     * @brief  处理一帧 Modbus TCP
     * @param  rxBuf  TCP收到的数据
     * @param  rxLen  接收长度
     * @param  txBuf  发送缓冲区
     * @param  txLen  返回发送长度
     * @return true   成功生成响应
     * @return false  报文非法，不响应
     */
    bool handleFrame(const uint8_t* rxBuf, uint16_t rxLen,
                     uint8_t* txBuf, uint16_t& txLen);
    bool handleFrame(const uint8_t* rxBuf, uint16_t rxLen,
                                std::string& txStr);
    bool handleFrame(const std::string& rxStr, std::string& txStr);

private:
#pragma pack(push,1)
    struct MBAP {
        uint16_t transactionId;
        uint16_t protocolId;
        uint16_t length;
        uint8_t  unitId;
    };
#pragma pack(pop)
};



#endif //KOKIRIKA_MODBUSTCP_H
