/**
* @Author kokirika
* @Name SD_BASE
* @Date 2022-11-09
**/

#include "SD_BASE.h"

uint8_t SD_BASE::FAT_init() {
    return 0;
}

uint16_t SD_BASE::write(uint32_t addr, uint8_t data) {
    return Storage_BASE::write(addr, data);
}

uint16_t SD_BASE::write(uint32_t addr, uint8_t *data, uint16_t len) {
    return Storage_BASE::write(addr, data, len);
}

uint8_t SD_BASE::read(uint32_t addr) {
    return Storage_BASE::read(addr);
}

void SD_BASE::read(uint32_t addr, uint8_t *data) {
    Storage_BASE::read(addr, data);
}

void SD_BASE::read(uint32_t addr, uint8_t *data, uint16_t len) {
    Storage_BASE::read(addr, data, len);
}
