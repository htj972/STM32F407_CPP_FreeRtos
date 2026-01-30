/**
* @Author kokirika
* @Name ModbusBase
* @Date 2026-1-26
**/

#include "ModbusBase.h"
#include <cstring>

/* ================= bit helper ================= */

static inline bool bit_read(const uint8_t* buf, uint16_t bitIndex)
{
    return (buf[bitIndex >> 3] >> (bitIndex & 0x07)) & 0x01;
}

static inline void bit_write(uint8_t* buf, uint16_t bitIndex, bool val)
{
    uint8_t& b = buf[bitIndex >> 3];
    uint8_t  m = (uint8_t)(1u << (bitIndex & 0x07));
    if (val) b |= m;
    else     b &= (uint8_t)~m;
}

/* ================= optimized bit rw ================= */

// 读连续 bit → 已按 Modbus bit 顺序写入 dst
static void readBitsOptimized(const uint8_t* src,
                              uint16_t addr,
                              uint16_t num,
                              uint8_t* dst)
{
    memset(dst, 0, (num + 7) / 8);

    uint16_t bitPos   = addr;
    uint16_t bitsLeft = num;
    uint16_t outBit   = 0;

    /* 1?? 头部：直到 byte 对齐 */
    while ((bitPos & 0x07) && bitsLeft) {
        if ((src[bitPos >> 3] >> (bitPos & 0x07)) & 0x01) {
            dst[outBit >> 3] |= (1u << (outBit & 0x07));
        }
        bitPos++;
        outBit++;
        bitsLeft--;
    }

    /* 2?? 中间整 byte memcpy */
    if (bitsLeft >= 8) {
        uint16_t byteCnt = bitsLeft >> 3;
        memcpy(&dst[outBit >> 3],
               &src[bitPos >> 3],
               byteCnt);
        bitPos   += byteCnt * 8;
        outBit   += byteCnt * 8;
        bitsLeft -= byteCnt * 8;
    }

    /* 3?? 尾部零散 bit */
    while (bitsLeft) {
        if ((src[bitPos >> 3] >> (bitPos & 0x07)) & 0x01) {
            dst[outBit >> 3] |= (1u << (outBit & 0x07));
        }
        bitPos++;
        outBit++;
        bitsLeft--;
    }
}

// 写连续 bit（来自 Modbus 请求数据）
static void writeBitsOptimized(uint8_t* dst,
                               uint16_t addr,
                               uint16_t num,
                               const uint8_t* src)
{
    uint16_t bitPos   = addr;
    uint16_t bitsLeft = num;
    uint16_t inBit    = 0;

    /* 1?? 头部 */
    while ((bitPos & 0x07) && bitsLeft) {
        bool v = (src[inBit >> 3] >> (inBit & 0x07)) & 0x01;
        uint8_t& b = dst[bitPos >> 3];
        auto  m = (uint8_t)(1u << (bitPos & 0x07));
        if (v) b |= m;
        else   b &= (uint8_t)~m;

        bitPos++;
        inBit++;
        bitsLeft--;
    }

    /* 2?? 中间整 byte */
    if (bitsLeft >= 8) {
        uint16_t byteCnt = bitsLeft >> 3;
        memcpy(&dst[bitPos >> 3],
               &src[inBit >> 3],
               byteCnt);
        bitPos   += byteCnt * 8;
        inBit    += byteCnt * 8;
        bitsLeft -= byteCnt * 8;
    }

    /* 3?? 尾部 */
    while (bitsLeft) {
        bool v = (src[inBit >> 3] >> (inBit & 0x07)) & 0x01;
        uint8_t& b = dst[bitPos >> 3];
        uint8_t  m = (uint8_t)(1u << (bitPos & 0x07));
        if (v) b |= m;
        else   b &= (uint8_t)~m;

        bitPos++;
        inBit++;
        bitsLeft--;
    }
}


/* ================= ctor ================= */

ModbusBase::ModbusBase()
{
    coils    = nullptr;   // 线圈（bit）
    discrete = nullptr;   // 离散输入（bit）
    input    = nullptr;   // 输入寄存器（uint16）

    coilSize = discSize = inputSize = 0;
    holdSegNum = 0;
}

/* ================= bind ================= */

void ModbusBase::bindCoils(uint8_t *coils, uint16_t len)
{
    this->coils = coils;
    coilSize = len;   // len = 点数(bit)
}

void ModbusBase::bindDiscrete(uint8_t *discrete, uint16_t len)
{
    this->discrete = discrete;
    discSize = len;   // len = 点数(bit)
}

void ModbusBase::bindInput(uint16_t *input, uint16_t len)
{
    this->input = input;
    inputSize = len;
}

bool ModbusBase::bindHoldingRam(uint16_t start,
                                uint16_t* buf,
                                uint16_t len)
{
    if (holdSegNum >= MAX_HOLD_SEG) return false;

    holdSeg[holdSegNum++] = {
        start, len, buf, nullptr, 0
    };
    return true;
}

bool ModbusBase::bindHoldingStorage(uint16_t start,
                                    Storage_BASE* dev,
                                    uint32_t baseAddr,
                                    uint16_t len)
{
    if (holdSegNum >= MAX_HOLD_SEG) return false;

    holdSeg[holdSegNum++] = {
        start, len, nullptr, dev, baseAddr
    };
    return true;
}

/* ================= PDU ================= */

bool ModbusBase::processPDU(uint8_t *req, uint16_t,
                            uint8_t *resp, uint16_t &len)
{
    len = 0;
    switch (req[0]) {
        case 0x01: FC01(req, resp, len); break;
        case 0x02: FC02(req, resp, len); break;
        case 0x03: FC03(req, resp, len); break;
        case 0x04: FC04(req, resp, len); break;
        case 0x05: FC05(req, resp, len); break;
        case 0x06: FC06(req, resp, len); break;
        case 0x0F: FC15(req, resp, len); break;
        case 0x10: FC16(req, resp, len); break;
        default:
            exception(req[0], 0x01, resp, len);
            break;
    }
    return len > 0;
}

/* ================= FC01 ================= */

void ModbusBase::FC01(const uint8_t *req, uint8_t *resp, uint16_t &len)
{
    if (!coils) {
        exception(0x01, 0x02, resp, len);
        return;
    }

    uint16_t addr = (req[1] << 8) | req[2];
    uint16_t num  = (req[3] << 8) | req[4];

    if (num < 1 || num > 2000 || !checkRange(addr, num, coilSize)) {
        exception(0x01, 0x03, resp, len);
        return;
    }

    uint8_t byteCnt = (num + 7) / 8;

    resp[0] = 0x01;
    resp[1] = byteCnt;

    readBitsOptimized(coils, addr, num, &resp[2]);

    len = 2 + byteCnt;
}


/* ================= FC02 ================= */

void ModbusBase::FC02(const uint8_t *req, uint8_t *resp, uint16_t &len)
{
    if (!discrete) {
        exception(0x02, 0x02, resp, len);
        return;
    }

    uint16_t addr = (req[1] << 8) | req[2];
    uint16_t num  = (req[3] << 8) | req[4];

    if (num < 1 || num > 2000 || !checkRange(addr, num, discSize)) {
        exception(0x02, 0x03, resp, len);
        return;
    }

    uint8_t byteCnt = (num + 7) / 8;

    resp[0] = 0x02;
    resp[1] = byteCnt;

    readBitsOptimized(discrete, addr, num, &resp[2]);

    len = 2 + byteCnt;
}


/* ================= FC03 ================= */

void ModbusBase::FC03(const uint8_t* req,
                      uint8_t* resp, uint16_t& len)
{
    uint16_t addr=(req[1]<<8)|req[2];
    uint16_t num =(req[3]<<8)|req[4];

    if(num<1||num>125){
        exception(0x03,0x03,resp,len); return;
    }

    auto* seg=findHoldingSeg(addr,num);
    if(!seg){
        exception(0x03,0x02,resp,len); return;
    }

    resp[0]=0x03;
    resp[1]=num*2;

    uint16_t off=addr-seg->start;

    if(seg->ram){
        for(uint16_t i=0;i<num;i++){
            uint16_t v=seg->ram[off+i];
            resp[2+i*2]=v>>8;
            resp[3+i*2]=v;
        }
    }else{
        seg->storage->read(
            seg->baseAddr+off*2,&resp[2],num*2);
    }
    len=2+num*2;
}

/* ================= FC04 ================= */

void ModbusBase::FC04(const uint8_t *req, uint8_t *resp, uint16_t &len)
{
    if (!input) {
        exception(0x04, 0x02, resp, len);
        return;
    }

    uint16_t addr = (req[1] << 8) | req[2];
    uint16_t num  = (req[3] << 8) | req[4];

    if (num < 1 || num > 125 || !checkRange(addr, num, inputSize)) {
        exception(0x04, 0x03, resp, len);
        return;
    }

    resp[0] = 0x04;
    resp[1] = num * 2;

    for (uint16_t i = 0; i < num; i++) {
        uint16_t v = input[addr + i];
        resp[2 + i * 2] = v >> 8;
        resp[3 + i * 2] = v & 0xFF;
    }

    len = 2 + num * 2;
}

/* ================= FC05 ================= */

void ModbusBase::FC05(uint8_t *req, uint8_t *resp, uint16_t &len)
{
    if (!coils) {
        exception(0x05, 0x02, resp, len);
        return;
    }

    uint16_t addr = (req[1] << 8) | req[2];
    uint16_t val  = (req[3] << 8) | req[4];

    if (!checkRange(addr, 1, coilSize) ||
        (val != 0xFF00 && val != 0x0000)) {
        exception(0x05, 0x03, resp, len);
        return;
    }

    bit_write(coils, addr, (val == 0xFF00));

    memcpy(resp, req, 5);
    len = 5;
}

/* ================= FC06 ================= */

void ModbusBase::FC06(uint8_t* req,
                      uint8_t* resp, uint16_t& len)
{
    uint16_t addr=(req[1]<<8)|req[2];
    uint16_t val =(req[3]<<8)|req[4];

    auto* seg=findHoldingSeg(addr,1);
    if(!seg){
        exception(0x06,0x02,resp,len); return;
    }

    uint16_t off=addr-seg->start;
    if(seg->ram){
        seg->ram[off]=val;
    }else{
        uint8_t b[2]={uint8_t(val>>8),uint8_t(val)};
        seg->storage->write(seg->baseAddr+off*2,b,2);
    }

    memcpy(resp,req,5);
    len=5;
}

/* ================= FC15 ================= */

void ModbusBase::FC15(uint8_t *req, uint8_t *resp, uint16_t &len)
{
    if (!coils) {
        exception(0x0F, 0x02, resp, len);
        return;
    }

    uint16_t addr  = (req[1] << 8) | req[2];
    uint16_t num   = (req[3] << 8) | req[4];
    uint8_t  bytes = req[5];

    if (num < 1 || num > 1968 ||
        bytes != (num + 7) / 8 ||
        !checkRange(addr, num, coilSize)) {
        exception(0x0F, 0x03, resp, len);
        return;
        }

    writeBitsOptimized(coils, addr, num, &req[6]);

    memcpy(resp, req, 5);
    len = 5;
}


/* ================= FC16 ================= */

void ModbusBase::FC16(uint8_t* req,
                      uint8_t* resp, uint16_t& len)
{
    uint16_t addr=(req[1]<<8)|req[2];
    uint16_t num =(req[3]<<8)|req[4];
    uint8_t bytes=req[5];

    if(num<1||num>123||bytes!=num*2){
        exception(0x10,0x03,resp,len); return;
    }

    auto* seg=findHoldingSeg(addr,num);
    if(!seg){
        exception(0x10,0x02,resp,len); return;
    }

    uint16_t off=addr-seg->start;

    if(seg->ram){
        const uint8_t* p=&req[6];
        for(uint16_t i=0;i<num;i++){
            seg->ram[off+i]=(p[0]<<8)|p[1];
            p+=2;
        }
    }else{
        seg->storage->write(
            seg->baseAddr+off*2,&req[6],bytes);
    }

    memcpy(resp,req,5);
    len=5;
}

/* ================= utils ================= */

void ModbusBase::exception(uint8_t func, uint8_t code,
                           uint8_t *resp, uint16_t &len)
{
    resp[0] = func | 0x80;
    resp[1] = code;
    len = 2;
}

bool ModbusBase::checkRange(uint16_t addr, uint16_t num, uint16_t max)
{
    if (num == 0) return false;
    if (addr >= max) return false;
    if (addr + num > max) return false;
    return true;
}

ModbusBase::HoldingSegment*
ModbusBase::findHoldingSeg(uint16_t addr, uint16_t num)
{
    for (uint8_t i = 0; i < holdSegNum; i++) {
        auto& s = holdSeg[i];
        if (addr >= s.start &&
            addr + num <= s.start + s.length) {
            return &s;
        }
    }
    return nullptr;
}
