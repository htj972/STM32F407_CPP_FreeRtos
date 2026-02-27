/**
* @Author kokirika
* @Name ModbusTcp
* @Date 2026-1-26
**/

#include "ModbusTcp.h"



/* ======== 字节序转换 ======== */
// Modbus TCP 协议要求网络字节序（big-endian）
static inline uint16_t ntoh16(uint16_t v)
{
    return (v >> 8) | (v << 8);
}

static inline uint16_t hton16(uint16_t v)
{
    return (v >> 8) | (v << 8);
}

bool ModbusTCP::handleFrame(const uint8_t* rxBuf, uint16_t rxLen,
                            uint8_t* txBuf, uint16_t& txLen)
{
    txLen = 0; // 初始化输出长度为0


    /* ====== 1. 最小长度校验 ====== */
    // MBAP头 + 最少一个功能码字节
    if (rxLen < sizeof(MBAP) + 1) {
        return false; // 收到非法长度报文
    }

    /* ====== 2. 提取 MBAP 头 ====== */
    const MBAP* rxMbap = reinterpret_cast<const MBAP*>(rxBuf);

    /* ====== 3. 协议ID校验 ====== */
    // Modbus TCP 协议ID 必须为 0x0000
    if (ntoh16(rxMbap->protocolId) != 0x0000) {
        return false; // 非 Modbus TCP 报文忽略
    }

    /* ====== 4. PDU长度校验 ====== */
    uint16_t pduLen = ntoh16(rxMbap->length); // MBAP中length字段：UnitID+PDU长度
    if (pduLen < 2) { // 至少要有UnitID+功能码
        return false;
    }

    /* ====== 5. 提取 PDU ====== */
    const uint8_t* reqPdu = rxBuf + sizeof(MBAP); // PDU起始地址
    uint16_t reqPduLen = pduLen - 1;             // 去掉UnitID长度

    uint8_t* respPdu = txBuf + sizeof(MBAP);    // 响应PDU缓冲区
    uint16_t respPduLen = 0;

    /* ====== 6. 调用 Base 层处理 PDU ====== */
    // processPDU() 内部会处理 FC01~FC16
    // 如果报文非法，会返回 false
    if (!processPDU(const_cast<uint8_t*>(reqPdu), reqPduLen, respPdu, respPduLen)) {
        // 生成标准异常响应
        // 功能码最高位置1，异常码默认0x01(Illegal Function)
        uint8_t func = reqPdu[0];
        exception(func, 0x01, respPdu, respPduLen);
    }

    /* ====== 7. 组装 MBAP 响应头 ====== */
    MBAP* txMbap = reinterpret_cast<MBAP*>(txBuf);
    txMbap->transactionId = rxMbap->transactionId; // 保持事务ID一致
    txMbap->protocolId    = 0x0000;               // Modbus TCP固定
    txMbap->unitId        = rxMbap->unitId;       // 目标单元ID
    txMbap->length        = hton16(respPduLen + 1); // PDU长度+UnitID

    /* ====== 8. 输出最终长度 ====== */
    txLen = sizeof(MBAP) + respPduLen;

    return true; // 成功生成响应
}

bool ModbusTCP::handleFrame(const uint8_t* rxBuf, uint16_t rxLen,
                            std::string& txStr)
{
    // 估算最大长度 = MBAP + PDU最大长度
    // Modbus TCP 最大PDU长度 = 253字节（含UnitID）
    const uint16_t maxLen = sizeof(MBAP) + 253;

    // 临时缓冲区
    uint8_t temp[maxLen];
    uint16_t tempLen = 0;

    bool result = handleFrame(rxBuf, rxLen, temp, tempLen);

    if (!result) {
        txStr.clear();
        return false;
    }

    // 将生成的响应复制到 std::string
    txStr.assign(reinterpret_cast<char*>(temp), tempLen);
    return true;
}

bool ModbusTCP::handleFrame(const std::string& rxStr, std::string& txStr)
{
    // 1?? 将 std::string 数据转换为 uint8_t 指针
    const auto* rxBuf = reinterpret_cast<const uint8_t*>(rxStr.data());
    uint16_t rxLen = rxStr.size();

    // 2?? 最大 Modbus TCP PDU 长度 = MBAP + 253（PDU最大长度）
    const uint16_t maxLen = sizeof(MBAP) + 253;
    if (rxLen > maxLen) {
        txStr.clear();  // 超长报文直接丢弃
        return false;
    }

    // 3?? 临时缓冲区处理响应
    uint8_t temp[maxLen];
    uint16_t tempLen = 0;

    // 4?? 调用原有 uint8_t* 接口
    bool result = handleFrame(rxBuf, rxLen, temp, tempLen);
    if (!result) {
        txStr.clear();
        return false;
    }

    // 5?? 将响应拷贝到 std::string
    txStr.assign(reinterpret_cast<char*>(temp), tempLen);
    return true;
}



