/**
* @Author kokirika
* @Name CRC
* @Date 2022-09-20
**/

#ifndef Kokirika_CRC_H
#define Kokirika_CRC_H

#include "sys.h"
#include "string"

class CRC32_MPEG2 {
private:
    static uint8_t init_flag;
public:
    CRC32_MPEG2();
    ~CRC32_MPEG2();
    static void clear();
    static uint32_t CalcCRC(uint32_t Data);
    static uint32_t CalcBlockCRC(uint32_t pBuffer[], uint32_t BufferLength);
};

class CRC16_Modbus{
public:
    static uint16_t Compute(uint8_t *puchMsg, uint16_t usDataLen);
    static uint16_t Compute(const std::string& str, uint16_t Len);
    static uint16_t Compute(const std::string& str);
};


#endif //Kokirika_CRC_H
