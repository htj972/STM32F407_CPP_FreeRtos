/**
* @Author kokirika
* @Name SD_CARD
* @Date 2022-11-05
**/

#ifndef KOKIRIKA_SD_CARD_H
#define KOKIRIKA_SD_CARD_H

#include "SD_BASE.h"
#include "SPI.h"

class SD_SPI: public SD_BASE{
private:
    uint8_t  SD_Type{};
    void SpeedLow();
    void SpeedHigh();
    void DisSelect();
    bool Select();
    bool Wait_OWN_Call;
    bool WaitReady() override;
    uint8_t GetResponse(uint8_t Response);
    bool RecvData(uint8_t *buf,uint16_t len);
    bool SendBlock(uint8_t *buf,uint8_t cmd);
    uint8_t SendCmd(uint8_t cmd, uint32_t arg, uint8_t crc);
    bool GetCID(uint8_t *cid_data);
    bool GetCSD(uint8_t *csd_data);
    uint8_t Initialize();
    uint8_t ReadDisk(uint8_t *buf,uint32_t sector,uint8_t cnt);
    uint8_t WriteDisk(uint8_t *buf,uint32_t sector,uint8_t cnt);
protected:
    _GPIO_ CSPinx;
    SPI    *spix{};
public:
    SD_SPI(SPI *SPIx, GPIO_TypeDef* PORTx, uint32_t Pinx,Queue mode=Queue::OWN_Queue);
    SD_SPI(SPI *SPIx, uint8_t CSpin,Queue mode=Queue::OWN_Queue);
    explicit SD_SPI(Queue mode=Queue::OWN_Queue);
    ~SD_SPI()=default;
    uint8_t init(SPI *SPIx,uint8_t CSpin);
    uint8_t init(SPI *SPIx,GPIO_TypeDef* PORTx,uint32_t Pinx);
    uint8_t init();
    uint32_t GetSectorCount() override;

    bool FAT_init() override;
    uint16_t write(uint32_t addr ,uint8_t data) override;
    uint16_t write(uint32_t addr ,uint8_t *data,uint16_t len) override;
    uint8_t  read(uint32_t addr) override;
    void     read(uint32_t addr,uint8_t *data) override;
    void     read(uint32_t addr,uint8_t *data,uint16_t len) override;
};


#endif //KOKIRIKA_SD_CARD_H