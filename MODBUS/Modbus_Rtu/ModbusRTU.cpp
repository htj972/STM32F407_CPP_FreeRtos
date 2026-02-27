/**
* @Author kokirika
* @Name ModbusTcp
* @Date 2026-1-26
**/

#include "ModbusRTU.h"
#include <cstring>

ModbusRTU::ModbusRTU(uint8_t slaveId)
: slaveId_(slaveId)
{}

void ModbusRTU::setSlaveId(uint8_t id) { slaveId_ = id; }
uint8_t ModbusRTU::getSlaveId() const { return slaveId_; }

/* ================= CRC16(Modbus) =================
 * 多项式：0xA001，初值：0xFFFF，低字节先传
 */
uint16_t ModbusRTU::crc16_modbus(const uint8_t* data, uint16_t len)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t b = 0; b < 8; b++) {
            if (crc & 0x0001) crc = (crc >> 1) ^ 0xA001;
            else              crc = (crc >> 1);
        }
    }
    return crc;
}

bool ModbusRTU::checkCrc(const uint8_t* frame, uint16_t len)
{
    if (len < 4) return false; // addr + fc + crc(2) 最小
    uint16_t calc = crc16_modbus(frame, len - 2);
    uint16_t recv = (uint16_t)frame[len - 2] | ((uint16_t)frame[len - 1] << 8);
    return calc == recv;
}

void ModbusRTU::appendCrc(uint8_t* frame, uint16_t lenWithoutCrc)
{
    uint16_t crc = crc16_modbus(frame, lenWithoutCrc);
    frame[lenWithoutCrc + 0] = (uint8_t)(crc & 0xFF);       // CRC_L
    frame[lenWithoutCrc + 1] = (uint8_t)((crc >> 8) & 0xFF);// CRC_H
}

/* ================= RTU 帧处理 ================= */

bool ModbusRTU::handleFrame(const uint8_t* rxBuf, uint16_t rxLen,
                            uint8_t* txBuf, uint16_t& txLen)
{
    txLen = 0;

    // 1) 基本长度检查
    if (!rxBuf || rxLen < 4) return false;

    // 2) CRC 校验失败：不响应（常规从站行为）
    if (!checkCrc(rxBuf, rxLen)) return false;

    // 3) 地址匹配
    uint8_t addr = rxBuf[0];
    bool isBroadcast = (addr == 0);
    if (!isBroadcast && addr != slaveId_) {
        return false; // 不是我的地址，不响应
    }

    // 4) 提取 PDU：从 rxBuf[1] 开始，长度 = rxLen - 1(addr) - 2(crc)
    const uint8_t* reqPdu = &rxBuf[1];
    uint16_t reqPduLen = rxLen - 3;

    // 5) 广播只允许写类功能码（标准约定）
    //    广播写：执行但不响应；广播读：直接忽略
    if (isBroadcast) {
        uint8_t fc = reqPdu[0];
        bool isWrite =
            (fc == 0x05) || (fc == 0x06) || (fc == 0x0F) || (fc == 0x10);
        if (!isWrite) return false;

        uint8_t dummyResp[260];
        uint16_t dummyLen = 0;
        (void)processPDU(const_cast<uint8_t*>(reqPdu), reqPduLen, dummyResp, dummyLen);
        return false; // 广播永远不回包
    }

    // 6) 调 Base 处理 PDU，得到响应 PDU
    uint8_t* respPdu = &txBuf[1];
    uint16_t respPduLen = 0;

    // 为了安全：限制响应最大长度（RTU 典型最大 256 左右）
    // 你也可以按你的 txBuf 实际尺寸在外面保证
    bool ok = processPDU(const_cast<uint8_t*>(reqPdu), reqPduLen, respPdu, respPduLen);
    if (!ok || respPduLen == 0) return false;

    // 7) 组 RTU 响应帧：addr + PDU + CRC
    txBuf[0] = slaveId_;
    txLen = 1 + respPduLen;
    appendCrc(txBuf, txLen);
    txLen += 2;

    return true;
}