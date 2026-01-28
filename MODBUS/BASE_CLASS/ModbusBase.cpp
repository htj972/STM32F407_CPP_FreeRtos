/**
* @Author kokirika
* @Name ModbusBase
* @Date 2026-1-26
**/

#include "ModbusBase.h"

ModbusBase::ModbusBase()
{
    coils = discrete = nullptr;
    holding = input = nullptr;
    coilSize = discSize = holdSize = inputSize = 0;
}

void ModbusBase::bindCoils(uint8_t *p, uint16_t s){ coils=p; coilSize=s; }
void ModbusBase::bindDiscrete(uint8_t *p, uint16_t s){ discrete=p; discSize=s; }
void ModbusBase::bindHolding(uint16_t *p, uint16_t s){ holding=p; holdSize=s; }
void ModbusBase::bindInput(uint16_t *p, uint16_t s){ input=p; inputSize=s; }

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
    memset(&resp[2], 0, byteCnt);

    for (uint16_t i = 0; i < num; i++) {
        if (coils[addr + i]) {
            resp[2 + (i >> 3)] |= (1 << (i & 0x07));
        }
    }

    len = 2 + byteCnt;
}

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
    memset(&resp[2], 0, byteCnt);

    for (uint16_t i = 0; i < num; i++) {
        if (discrete[addr + i]) {
            resp[2 + (i >> 3)] |= (1 << (i & 0x07));
        }
    }

    len = 2 + byteCnt;
}

void ModbusBase::FC03(const uint8_t *req, uint8_t *resp, uint16_t &len)
{
    if (!holding) {
        exception(0x03, 0x02, resp, len);
        return;
    }

    uint16_t addr = (req[1] << 8) | req[2];
    uint16_t num  = (req[3] << 8) | req[4];

    if (num < 1 || num > 125 || !checkRange(addr, num, holdSize)) {
        exception(0x03, 0x03, resp, len);
        return;
    }

    resp[0] = 0x03;
    resp[1] = num * 2;

    for (uint16_t i = 0; i < num; i++) {
        uint16_t v = holding[addr + i];
        resp[2 + i * 2] = v >> 8;
        resp[3 + i * 2] = v & 0xFF;
    }

    len = 2 + num * 2;
}

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

    coils[addr] = (val == 0xFF00);
    memcpy(resp, req, 5);
    len = 5;
}

void ModbusBase::FC06(uint8_t *req, uint8_t *resp, uint16_t &len)
{
    if (!holding) {
        exception(0x06, 0x02, resp, len);
        return;
    }

    uint16_t addr = (req[1] << 8) | req[2];
    uint16_t val  = (req[3] << 8) | req[4];

    if (!checkRange(addr, 1, holdSize)) {
        exception(0x06, 0x02, resp, len);
        return;
    }

    holding[addr] = val;
    memcpy(resp, req, 5);
    len = 5;
}

void ModbusBase::FC15(uint8_t *req, uint8_t *resp, uint16_t &len)
{
    if (!coils) {
        exception(0x0F, 0x02, resp, len);
        return;
    }

    uint16_t addr = (req[1] << 8) | req[2];
    uint16_t num  = (req[3] << 8) | req[4];
    uint8_t  bytes = req[5];

    if (num < 1 || num > 1968 ||
        bytes != (num + 7) / 8 ||
        !checkRange(addr, num, coilSize)) {
        exception(0x0F, 0x03, resp, len);
        return;
    }

    uint8_t *p = &req[6];

    for (uint16_t i = 0; i < num; i++) {
        coils[addr + i] = (p[i >> 3] >> (i & 0x07)) & 0x01;
    }

    memcpy(resp, req, 5);
    len = 5;
}

void ModbusBase::FC16(uint8_t *req, uint8_t *resp, uint16_t &len)
{
    if (!holding) {
        exception(0x10, 0x02, resp, len);
        return;
    }

    uint16_t addr  = (req[1] << 8) | req[2];
    uint16_t num   = (req[3] << 8) | req[4];
    uint8_t  bytes = req[5];

    if (num < 1 || num > 123 ||
        bytes != num * 2 ||
        !checkRange(addr, num, holdSize)) {
        exception(0x10, 0x03, resp, len);
        return;
    }

    uint8_t *p = &req[6];
    for (uint16_t i = 0; i < num; i++) {
        holding[addr + i] = (p[0] << 8) | p[1];
        p += 2;
    }

    memcpy(resp, req, 5);
    len = 5;
}

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










