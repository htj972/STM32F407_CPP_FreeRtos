#ifndef __FONTUPD_H__
#define __FONTUPD_H__	 
#include <sys.h>

typedef struct{
    void (*write)(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte);
    void (*Erase)(uint32_t Dst_Addr);
    void (*read) (uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte) ;
}Font_lib;
//extern uint32_t FONTINFOADDR;
//extern _font_info ftinfo;	//字库信息结构体

uint32_t fupd_prog(uint16_t x,uint16_t y,uint8_t size,uint32_t fsize,uint32_t pos);	//显示更新进度
uint8_t  updata_fontx(uint16_t x,uint16_t y,uint8_t size,uint8_t *fxpath,uint8_t fx);	//更新指定字库
uint8_t  update_font(uint16_t x,uint16_t y,uint8_t size,uint8_t* src);			//更新全部字库
void     font_read(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte);
uint32_t get_font_gbk_size();
uint32_t get_font_gbk_ADD();
uint8_t  font_init(Font_lib *Font_fun);										//初始化字库
#endif





















