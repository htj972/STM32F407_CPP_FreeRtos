/**
* @Author kokirika
* @Name STORAGE_BASE
* @Date 2022-10-18
**/

#ifndef KOKIRIKA_STORAGE_BASE_H
#define KOKIRIKA_STORAGE_BASE_H

#include "sys.h"
#include "string"
#include "HARD_BASE.h"

class Storage_BASE: public HARD_BASE {
protected:
    bool init_flag{};
    uint32_t SECTOR_SIZE{};
    uint32_t BLOCK_SIZE{};
public:
    uint32_t Get_Sector_Size() const;
    uint32_t Get_Block_Size() const;
    void Set_Sector_Size(uint32_t SECTOR);
    void Set_Block_Size(uint32_t BLOCK);
    bool get_init_flag() const;
    bool to_init();
    virtual bool FAT_init();
    virtual bool WaitReady();
    virtual uint32_t GetSectorCount();
    virtual uint16_t write(uint32_t addr ,uint8_t data);
    virtual uint16_t write(uint32_t addr ,uint8_t *data,uint16_t len);
    uint16_t writestr(uint32_t addr ,const std::string& data);
    virtual uint8_t  read(uint32_t addr);
    virtual void     read(uint32_t addr,uint8_t *data);
    virtual void     read(uint32_t addr,uint8_t *data,uint16_t len);
    void    readstr(uint32_t addr,std::string *data,uint16_t len);
};


#endif //KOKIRIKA_STORAGE_BASE_H
