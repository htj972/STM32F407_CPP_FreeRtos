#ifndef __FONTUPD_H__
#define __FONTUPD_H__

#include "Storage_BASE.h"
#include "ff.h"

#define Inflash_mode 0
#define Exflash_mode 1

#define GBK_mode Inflash_mode

#if GBK_mode ==  Exflash_mode
#include "W25QXX.h"
#endif


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
#if GBK_mode ==  Exflash_mode
    static W25QXX *W25Q_Link;
    static void     font_read(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte);
    static uint32_t get_font_gbk_size();
    static uint32_t get_font_gbk_ADD();
    /*字库操作*/
    static bool     update_font(uint8_t size,uint8_t* src);			//更新全部字库
private:
    static bool     font_init(W25QXX *Storage);
    static bool     font_init();
public:
    /*字库对外接口*/
    static uint32_t fupd_prog(uint32_t fsize, uint32_t pos);	//显示更新进度
    static bool     updata_fontx(uint8_t *fxpath, uint8_t fx);	//更新指定字库	    //初始化字库
    static bool exfuns_init(const char *name,W25QXX *SDx,bool Font= false);

    static bool exfuns_init(W25QXX *SDx,bool Font= false);
#endif
    static Storage_BASE *Storage_L[_VOLUMES];
    static FATFS *fs[_VOLUMES];
    static char disk_name[_VOLUMES][3];
private:
    static uint8_t  disk_num;
    static bool exfuns_init(char *name);
    static bool exfuns_init(const char *name,Storage_BASE *Storage);
    std::string dir_str;
    uint8_t disk_n{};
    bool mount_ret{};
public:
    Storage_Link();
    explicit Storage_Link(Storage_BASE *Storage);
    /*系统挂载操作*/
    bool init(Storage_BASE *Storage);
    bool init();
    FIL fp;
    UINT plen;
    DIR dr;
    bool get_mount_ret() const;
    char* get_name() const;
    char * setdir(const std::string& dir);
    char * setdir(uint8_t* dir);
    /*文件操作*/
    static uint8_t  exf_typetell(uint8_t *fname);
    static uint8_t  exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free);
    /*文件系统操作*/
    static bool disk_init  (BYTE name);
    static bool disk_write (BYTE name, const BYTE *buff, DWORD sector, UINT count);
    static bool disk_read  (BYTE name,BYTE *buff,DWORD sector,UINT count);
    static bool disk_ioctl (BYTE name,BYTE cmd,void *buff);
};

/*
  USB_MSC      usb;
  Storage_Link USB_diak(&usb);

    usb.init();
    usb.wait_Linked();

    MOLED.Print(0,0,(uint8_t*)"init OK!");


    FIL fp;//文件指针
    FIL fp2;//文件指针
    UINT plen;	//文件长度临时数据
    DIR dr;
    FRESULT rrt;
    char text_data[10]="123456";
    char gext_data[10]="qwerty";

    MOLED.Print(0,0,"%s",
                USB_diak.init()?"true":"false");

//    f_mkfs("0:",1,4096);
    f_mkdir(USB_diak.setdir("qwe"));

    rrt=f_open(&fp,USB_diak.setdir("qwe/23.txt"),FA_WRITE|FA_OPEN_ALWAYS);
    if(rrt==FR_OK)
    {
        f_lseek(&fp,fp.fsize);//移动光标到文件尾
        f_write(&fp,text_data,strlen((char*)text_data),&plen);//写入数据
        f_close(&fp);
        MOLED.Print(0,4,(char*)"true");//关闭文件
    }
    else
     MOLED.Print(0,4,(char*)"false");//关闭文件

    f_open(&fp2,USB_diak.setdir("qwe/23.txt"),FA_READ);
    f_read(&fp2,gext_data,strlen((char*)text_data),&plen);
    f_close(&fp2);
    MOLED.Print(0,6,gext_data);//关闭文件
 */


#endif





















