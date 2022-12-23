/**
* @Author kokirika
* @Name STM_FLASH
* @Date 2022-12-23
**/

#ifndef KOKIRIKA_STM_FLASH_H
#define KOKIRIKA_STM_FLASH_H

#define STM32_FLASH_BASE 0x08000000 	//STM32 FLASH的起始地址

#include "sys.h"

class STM_FLASH {
private:
    static uint16_t GetFlashSector(uint32_t addr);
public:
    static uint32_t ReadWord(uint32_t faddr);
    static void Write(uint32_t WriteAddr,uint32_t *pBuffer,uint32_t NumToWrite);
    static void Read(uint32_t ReadAddr,uint32_t *pBuffer,uint32_t NumToRead);

};


#endif //KOKIRIKA_STM_FLASH_H
