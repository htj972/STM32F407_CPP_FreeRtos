/**
* @Author kokirika
* @Name STORAGE_BASE
* @Date 2022-10-18
**/

#include "Storage_BASE.h"

//bool Storage_BASE::init() {
//    return false;
//}

uint16_t Storage_BASE::write(uint32_t addr, uint8_t data) {
    return 1;
}

uint16_t Storage_BASE::write(uint32_t addr, uint8_t *data, uint16_t len) {
    for (uint16_t ii=0;ii<len;ii++)
        this->write(addr+ii,data[ii]);
    return len;
}

uint16_t Storage_BASE::writestr(uint32_t addr, const std::string& data) {
    return this->write(addr,(uint8_t *)data.data(),data.length());
}

uint8_t Storage_BASE::read(uint32_t addr) {
    return 0;
}

void Storage_BASE::read(uint32_t addr, uint8_t *data) {
    *data= this->read(addr);
}

void Storage_BASE::read(uint32_t addr, uint8_t *data, uint16_t len) {
    for (uint16_t ii=0;ii<len;ii++)
        data[ii]=this->read(addr+ii);
}

void Storage_BASE::readstr(uint32_t addr, std::string *data, uint16_t len) {
    this->read(addr,(uint8_t *)data->data(),len);
}

bool Storage_BASE::get_init_flag() const {
    return this->init_flag;
}

bool Storage_BASE::FAT_init() {
    this->init_flag= true;
    return true;
}

bool Storage_BASE::to_init() {
    if(!this->init_flag)
        return this->FAT_init();
    return true;
}

uint32_t Storage_BASE::GetSectorCount() {
    return 0;
}


