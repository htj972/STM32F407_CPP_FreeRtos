/**
* @Author kokirika
* @Name SD_BASE
* @Date 2022-11-09
**/

#ifndef KOKIRIKA_SD_BASE_H
#define KOKIRIKA_SD_BASE_H

#include "Storage_BASE.h"

class SD_BASE: public Storage_BASE {
public:
    virtual uint8_t FAT_init();
    uint16_t write(uint32_t addr ,uint8_t data) override;
    uint16_t write(uint32_t addr ,uint8_t *data,uint16_t len) override;
    uint8_t  read(uint32_t addr) override;
    void     read(uint32_t addr,uint8_t *data) override;
    void     read(uint32_t addr,uint8_t *data,uint16_t len) override;
};


#endif //KOKIRIKA_SD_BASE_H
