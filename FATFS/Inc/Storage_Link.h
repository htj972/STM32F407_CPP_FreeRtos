#ifndef __FONTUPD_H__
#define __FONTUPD_H__

#include "W25QXX.h"
#include "SD_BASE.h"
#include "ff.h"

class Storage_Link{
public:
    enum TYPE:uint8_t{
    BIN	=   0X00,	//bin文件
    LRC	=   0X10,	//lrc文件
    NES	=   0X20,	//nes文件

    TEXT=   0X30,	//.txt文件
    C	=   0X31,	//.c文件
    CPP	=   0X32,	//.c文件
    H	=   0X33,    //.h文件

    WAV	=   0X40,	//WAV文件
    MP3	=   0X41,	//MP3文件
    APE	=   0X42,	//APE文件
    FLAC=   0X43,	//FLAC文件

    BMP	=   0X50,	//bmp文件
    JPG	=   0X51,	//jpg文件
    JPEG=   0X52,	//jpeg文件
    GIF	=   0X53,	//gif文件

    AVI	=   0X60,	//avi文件

    ERR0R = 0xff,
    };
public:
    static W25QXX *W25Q_Link;
    static Storage_BASE *Storage_L[_VOLUMES];
    static FATFS *fs[_VOLUMES];
    static char disk_name[_VOLUMES][3];
private:
    static uint32_t fupd_prog(uint32_t fsize, uint32_t pos);	//显示更新进度
    static bool     updata_fontx(uint8_t *fxpath, uint8_t fx);	//更新指定字库	    //初始化字库
    static uint8_t  disk_num;
    static bool exfuns_init(char *name);
public:
    /*字库对外接口*/
    static void     font_read(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte);
    static uint32_t get_font_gbk_size();
    static uint32_t get_font_gbk_ADD();
    /*字库操作*/
    static bool     update_font(uint8_t size,uint8_t* src);			//更新全部字库
    static bool     font_init(W25QXX *Storage);
    static bool     font_init();
    /*文件操作*/
    static uint8_t  f_typetell(uint8_t *fname);
    static uint8_t  exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free);
    /*系统挂载操作*/
    static bool exfuns_init(const char *name,Storage_BASE *Storage);
    static bool exfuns_init(const char *name,W25QXX *SDx,bool Font= false);
    /*文件系统操作*/
    static bool disk_init  (BYTE name);
    static bool disk_write (BYTE name, const BYTE *buff, DWORD sector, UINT count);
    static bool disk_read  (BYTE name,BYTE *buff,DWORD sector,UINT count);
    static bool disk_ioctl (BYTE name,BYTE cmd,void *buff);
};



#endif





















