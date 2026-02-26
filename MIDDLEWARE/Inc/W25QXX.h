/**
* @Author kokirika
* @Name W25QXX
* @Date 2022-10-18
**/

#ifndef KOKIRIKA_W25QXX_H
#define KOKIRIKA_W25QXX_H

#include "Storage_BASE.h"
#include "GPIO.h"
#include "SPI.h"
#include "Out_In_Put.h"

class W25QXX: public Storage_BASE{
private:
    uint16_t W25QXX_TYPE{};
    uint16_t wait_times{};
    uint16_t BaudRatex{};
    uint32_t Sectosize{};

    uint16_t ReadID();
    uint8_t  ReadSR();
    void     Write_SR(uint8_t SR);
    void     Write_Enable();
    void     Write_Disable();
    void     Write_Page(uint32_t Addr, uint8_t* pBuffer, uint16_t NumByte);
    void     Write_NoCheck(uint32_t Addr,uint8_t* pBuffer,uint16_t NumByte);
    void     Wait_Busy();
protected:
    _OutPut_   CSPin;
    SPI      *spix{};
    void     Erase_Chip();
    void     PowerDown();
    void     WAKEUP();
public:
    enum TYPE{
        W25Q80 	=0XEF13,
        W25Q16 	=0XEF14,
        W25Q32 	=0XEF15,
        W25Q64 	=0XEF16,
        W25Q128	=0XEF17,
    };

    W25QXX(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx,uint16_t BaudRate=SPI_BaudRatePrescaler_2,Queue mode=Queue::OWN_Queue);
    W25QXX(SPI *SPIx,uint8_t CSpin,uint16_t BaudRate=SPI_BaudRatePrescaler_2,Queue mode=Queue::OWN_Queue);
    explicit W25QXX(Queue mode=Queue::OWN_Queue);
    ~W25QXX()=default;
    void init();
    void init(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx,uint16_t BaudRate=0);
    void init(SPI *SPIx,uint8_t CSpin,uint16_t BaudRate=0);

    void     Erase_Sector(uint32_t Dst_Addr);
    uint16_t GetID() const;
    void     set_wait_time(uint16_t time);
    void     set_FAT_Sectosize(uint32_t size);

    bool FAT_init() override;
    uint32_t GetSectorCount() override;
    uint16_t write(uint32_t addr ,uint8_t data) override;
    uint16_t write(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte) override;
    uint8_t  read(uint32_t addr) override;
    void     read(uint32_t addr,uint8_t *data) override;
    void     read (uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte) override;
    bool DISwrite(uint32_t sector, uint8_t* buf, uint32_t count) override;
    bool DISread(uint32_t sector, uint8_t* buf, uint32_t count) override;
};


#endif //KOKIRIKA_W25QXX_H
