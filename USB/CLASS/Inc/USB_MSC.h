/**
* @Author kokirika
* @Name USB_MSC
* @Date 2022-11-25
**/

#ifndef KOKIRIKA_USB_MSC_H
#define KOKIRIKA_USB_MSC_H

#include "Storage_BASE.h"
//#include "Timer.h"

#include "usbh_core.h"
#include "usb_conf.h"
#include "usbh_msc_core.h"
#include "usb_hcd_int.h"

class USB_MSC: public Storage_BASE {
public:
    enum SATA:uint8_t{
        UnLink  =   0,
        Linking =   1,
        Linked  =   2,
        UnLinking=  3,
    };
private:
    static uint8_t UDISK_Read(uint8_t* buf,uint32_t sector,uint16_t cnt);
    static uint8_t UDISK_Write(uint8_t* buf,uint32_t sector,uint16_t cnt);
    SATA   device_sata{};
//    Timer  to_wait();
public:
    void init();
    void Upset();
    SATA Get_device_sata() const;
    void wait_Linked();
    bool FAT_init() override;
    uint32_t GetSectorCount() override;
    uint16_t write(uint32_t addr ,uint8_t data) override;
    uint16_t write(uint32_t addr ,uint8_t *data,uint16_t len) override;

    uint8_t  read(uint32_t addr) override;
    void     read(uint32_t addr,uint8_t *data) override;
    void     read(uint32_t addr,uint8_t *data,uint16_t len) override;


};

#endif //KOKIRIKA_USB_MSC_H
