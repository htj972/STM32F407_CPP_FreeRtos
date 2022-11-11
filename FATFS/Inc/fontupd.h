#ifndef __FONTUPD_H__
#define __FONTUPD_H__

#include "W25QXX.h"
#include <functional>

//extern uint32_t FONTINFOADDR;
//extern _font_info ftinfo;	//字库信息结构体
class Storage_Link{
protected:
//    static struct Font_lib{
//        std::function<void()> (*write)(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte);
//        std::function<void()> (*Erase)(uint32_t Dst_Addr);
//        std::function<void()> (*read) (uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte) ;
//    }FLASH_Interface;
    static W25QXX *Link;
private:
    static uint32_t fupd_prog(uint16_t x, uint16_t y, uint32_t fsize, uint32_t pos);	//显示更新进度
    static bool     updata_fontx(uint16_t x, uint16_t y, uint8_t *fxpath, uint8_t fx);	//更新指定字库
    static bool  font_init();										//初始化字库
public:
    static bool     update_font(uint16_t x,uint16_t y,uint8_t size,uint8_t* src);			//更新全部字库
    static void     font_read(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte);
    static uint32_t get_font_gbk_size();
    static uint32_t get_font_gbk_ADD();
    static bool  font_init(W25QXX *Storage);


};



#endif





















