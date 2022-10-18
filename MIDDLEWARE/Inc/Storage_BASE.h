/**
* @Author kokirika
* @Name STORAGE_BASE
* @Date 2022-10-18
**/

#ifndef KOKIRIKA_STORAGE_BASE_H
#define KOKIRIKA_STORAGE_BASE_H

#include "sys.h"
#include "string"

class Storage_BASE {
public:
    virtual uint16_t write(uint32_t addr ,uint8_t data);
    virtual uint16_t write(uint32_t addr ,uint8_t *data,uint16_t len);
    uint16_t writestr(uint32_t addr ,const std::string& data);
    virtual uint8_t  read(uint32_t addr);
    virtual void     read(uint32_t addr,uint8_t *data);
    virtual void     read(uint32_t addr,uint8_t *data,uint16_t len);
    void    readstr(uint32_t addr,std::string *data,uint16_t len);
};


#endif //KOKIRIKA_STORAGE_BASE_H
