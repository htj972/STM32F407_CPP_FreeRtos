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
    bool Wait_OWN_Call{};
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
    _GPIO_ CSPinx{};
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
/*
    SPI             spi2(SPI2);
    SD_SPI          SD1(&spi2, GPIOB12);
    Storage_Link SD_diak(&SD1);

    spi2.config(GPIOB13,GPIOB14,GPIOB15);
    spi2.init(SPI2);
    uint8_t ret=SD1.init();
    while(ret)//检测不到SD卡
    {
        ret=SD1.init();
        delay_ms(500);
        MOLED.Print(0,0,"ret:%x",ret);
        delay_ms(500);
        MOLED.Print(0,0,(uint8_t*)"       ");
    }
    MOLED.Print(0,0,(uint8_t*)"init OK!");


    FIL fp;//文件指针
    FIL fp2;//文件指针
    UINT plen;	//文件长度临时数据
    DIR dr;
    FRESULT rrt;
    char text_data[10]="123456";
    char gext_data[10]="qwerty";

    MOLED.Print(0,0,"%s",
                SD_diak.init()?"true":"false");
//    f_mkfs("0:",1,4096);
    f_mkdir(SD_diak.setdir("qwe"));

    rrt=f_open(&fp,SD_diak.setdir("qwe/23.txt"),FA_WRITE|FA_OPEN_ALWAYS);
    if(rrt==FR_OK)
    {
        f_lseek(&fp,fp.fsize);//移动光标到文件尾
        f_write(&fp,text_data,strlen((char*)text_data),&plen);//写入数据
        f_close(&fp);
        MOLED.Print(0,4,(char*)"true");//关闭文件
    }
    else
     MOLED.Print(0,4,(char*)"false");//关闭文件

    f_open(&fp2,SD_diak.setdir("qwe/23.txt"),FA_READ);
    f_read(&fp2,gext_data,strlen((char*)text_data),&plen);
    f_close(&fp2);
    MOLED.Print(0,6,gext_data);//关闭文件
*/

#endif //KOKIRIKA_SD_CARD_H
