#ifndef __FONTUPD_H__
#define __FONTUPD_H__

#include "W25QXX.h"
#include "SD_BASE.h"
#include "ff.h"

class Storage_Link{
public:
    enum TYPE:uint8_t{
    BIN	=   0X00,	//bin�ļ�
    LRC	=   0X10,	//lrc�ļ�
    NES	=   0X20,	//nes�ļ�

    TEXT=   0X30,	//.txt�ļ�
    C	=   0X31,	//.c�ļ�
    CPP	=   0X32,	//.c�ļ�
    H	=   0X33,    //.h�ļ�

    WAV	=   0X40,	//WAV�ļ�
    MP3	=   0X41,	//MP3�ļ�
    APE	=   0X42,	//APE�ļ�
    FLAC=   0X43,	//FLAC�ļ�

    BMP	=   0X50,	//bmp�ļ�
    JPG	=   0X51,	//jpg�ļ�
    JPEG=   0X52,	//jpeg�ļ�
    GIF	=   0X53,	//gif�ļ�

    AVI	=   0X60,	//avi�ļ�

    ERR0R = 0xff,
    };
public:
    static W25QXX *W25Q_Link;
    static Storage_BASE *Storage_L[_VOLUMES];
    static FATFS *fs[_VOLUMES];
    static char disk_name[_VOLUMES][3];
private:
    static uint32_t fupd_prog(uint32_t fsize, uint32_t pos);	//��ʾ���½���
    static bool     updata_fontx(uint8_t *fxpath, uint8_t fx);	//����ָ���ֿ�	    //��ʼ���ֿ�
    static uint8_t  disk_num;
    static bool exfuns_init(char *name);
public:
    /*�ֿ����ӿ�*/
    static void     font_read(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte);
    static uint32_t get_font_gbk_size();
    static uint32_t get_font_gbk_ADD();
    /*�ֿ����*/
    static bool     update_font(uint8_t size,uint8_t* src);			//����ȫ���ֿ�
    static bool     font_init(W25QXX *Storage);
    static bool     font_init();
    /*�ļ�����*/
    static uint8_t  f_typetell(uint8_t *fname);
    static uint8_t  exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free);
    /*ϵͳ���ز���*/
    static bool exfuns_init(const char *name,Storage_BASE *Storage);
    static bool exfuns_init(const char *name,W25QXX *SDx,bool Font= false);
    /*�ļ�ϵͳ����*/
    static bool disk_init  (BYTE name);
    static bool disk_write (BYTE name, const BYTE *buff, DWORD sector, UINT count);
    static bool disk_read  (BYTE name,BYTE *buff,DWORD sector,UINT count);
    static bool disk_ioctl (BYTE name,BYTE cmd,void *buff);
};



#endif





















