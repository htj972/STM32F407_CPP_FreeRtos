/**
* @Author kokirika
* @Name W25QXX
* @Date 2022-10-18
**/

#ifndef KOKIRIKA_W25QXX_H
#define KOKIRIKA_W25QXX_H

#include "../BASE_CLASS/Inc/Storage_BASE.h"
#include "GPIO.h"
#include "SPI.h"

class W25QXX: public Storage_BASE{
private:
    uint16_t W25QXX_TYPE{};
    uint16_t wait_times=500;
    uint16_t BaudRatex{};
    std::string get_data;
public:
    enum TYPE{
        W25Q80 	=0XEF13,
        W25Q16 	=0XEF14,
        W25Q32 	=0XEF15,
        W25Q64 	=0XEF16,
        W25Q128	=0XEF17,
    };
    enum COMMON{
        W25X_WriteStatusReg		=0x01,
        W25X_PageProgram		=0x02,
        W25X_ReadData			=0x03,
        W25X_WriteDisable		=0x04,
        W25X_ReadStatusReg		=0x05,
        W25X_WriteEnable		=0x06,
        W25X_FastReadData		=0x0B,
        W25X_SectorErase		=0x20,
        W25X_FastReadDual		=0x3B,
        W25X_ManufactDeviceID	=0x90,
        W25X_JedecDeviceID		=0x9F,
        W25X_ReleasePowerDown	=0xAB,
        W25X_DeviceID			=0xAB,
        W25X_PowerDown			=0xB9,
        W25X_ChipErase			=0xC7,
        W25X_BlockErase			=0xD8,
    };
    _GPIO_ CSPin;
    SPI    *spix{};

    W25QXX(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx,uint16_t BaudRate=0);
    W25QXX(SPI *SPIx,uint8_t CSpin,uint16_t BaudRate=0);
    W25QXX()=default;
    ~W25QXX()=default;
    void init();
    void init(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx,uint16_t BaudRate=0);
    void init(SPI *SPIx,uint8_t CSpin,uint16_t BaudRate=0);
    uint16_t ReadID() const;
    uint8_t  ReadSR() const;
    void     Write_SR(uint8_t SR) const;
    void     Write_Enable() const;
    void     Write_Disable() const;
    void     Wait_Busy() const;
    void     Erase_Chip() const;
    void     Erase_Sector(uint32_t Dst_Addr) const;
    void     PowerDown() const;
    void     WAKEUP() const;
    uint16_t GetID() const;

    void     Write_Page(uint32_t Addr, uint8_t* pBuffer, uint16_t NumByte) const;
    void     Write_NoCheck(uint32_t Addr,uint8_t* pBuffer,uint16_t NumByte) const;
    uint16_t write(uint32_t addr ,uint8_t data) override;
    uint16_t write(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte) override;
    uint8_t  read(uint32_t addr) override;
    void     read(uint32_t addr,uint8_t *data) override;
    void     read (uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte) override;
};


#endif //KOKIRIKA_W25QXX_H
