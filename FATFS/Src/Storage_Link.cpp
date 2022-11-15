#include "Storage_Link.h"
#include "ff.h"
#include <cstring>
#include "malloc.h"
#include "delay.h"
#include "diskio.h"

//�ֿ�����ռ�õ�����������С(3���ֿ�+unigbk��+�ֿ���Ϣ=3238700�ֽ�,Լռ791��W25QXX����)
#define FONTSECSIZE	 	791
//�ֿ�����ʼ��ַ 
#define FONTINFOADDR 	(1024*1024*12) 					//Explorer STM32F4�Ǵ�12M��ַ�Ժ�ʼ����ֿ�
														//ǰ��12M��fatfsռ����.
														//12M�Ժ����3���ֿ�+UNIGBK.BIN,�ܴ�С3.09M,���ֿ�ռ����,���ܶ�!
														//15.10M�Ժ�,�û���������ʹ��.����������100K�ֽڱȽϺ�.

//�ֿ���Ϣ�ṹ�嶨��
//���������ֿ������Ϣ����ַ����С��
//__packed typedef struct
//������Ϣ�����ַ,ռ33���ֽ�,��1���ֽ����ڱ���ֿ��Ƿ����.����ÿ8���ֽ�һ��,�ֱ𱣴���ʼ��ַ���ļ���С
typedef struct __attribute__ ((packed)) font_info_
{
    uint8_t fontok;				//�ֿ���ڱ�־��0XAA���ֿ��������������ֿⲻ����
    uint32_t ugbkaddr; 			//unigbk�ĵ�ַ
    uint32_t ugbksize;			//unigbk�Ĵ�С
    uint32_t f12addr;			//gbk12��ַ
    uint32_t gbk12size;			//gbk12�Ĵ�С
    uint32_t f16addr;			//gbk16��ַ
    uint32_t gbk16size;			//gbk16�Ĵ�С
    uint32_t f24addr;			//gbk24��ַ
    uint32_t gkb24size;			//gbk24�Ĵ�С
}font_info;
font_info ftinfo;

//�ֿ����ڴ����е�·��
const char* GBK24_PATH="/SYSTEM/FONT/GBK24.FON";		//GBK24�Ĵ��λ��
const char* GBK16_PATH="/SYSTEM/FONT/GBK16.FON";		//GBK16�Ĵ��λ��
const char* GBK12_PATH="/SYSTEM/FONT/GBK12.FON";		//GBK12�Ĵ��λ��
const char* UNIGBK_PATH="/SYSTEM/FONT/UNIGBK.BIN";		//UNIGBK.BIN�Ĵ��λ��

#define FILE_MAX_TYPE_NUM		7	//���FILE_MAX_TYPE_NUM������
#define FILE_MAX_SUBT_NUM		4	//���FILE_MAX_SUBT_NUM��С��

//�ļ������б�
const char*  FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
        {
                {"BIN"},			//BIN�ļ�
                {"LRC"},			//LRC�ļ�
                {"NES"},			//NES�ļ�
                {"TXT","C","CPP","H"},	//�ı��ļ�
                {"WAV","MP3","APE","FLAC"},//֧�ֵ������ļ�
                {"BMP","JPG","JPEG","GIF"},//ͼƬ�ļ�
                {"AVI"},			//��Ƶ�ļ�
        };

//��ʾ��ǰ������½���
//x,y:����
//size:�����С
//fsize:�����ļ���С
//pos:��ǰ�ļ�ָ��λ��
uint32_t  Storage_Link::fupd_prog(uint32_t fsize, uint32_t pos)
{
	float prog;
    float t=0XFF;
	prog=(float)pos/(float)fsize;
	prog*=100;
	if(t!=prog)
	{
		//LCD_ShowString(x+3*size/2,y,240,320,size,"%");		
		t=prog;
	}
	return 0;					    
}

//����ĳһ��
//x,y:����
//size:�����С
//fxpath:·��
//fx:���µ����� 0,ungbk;1,gbk12;2,gbk16;3,gbk24;
bool  Storage_Link::updata_fontx(uint8_t *fxpath, uint8_t fx)
{
	uint32_t flashaddr=0;
 	uint8_t res;
	uint16_t bread;
	uint32_t offx=0;
	uint8_t rval=0;
    FIL *fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//�����ڴ�
	if(fftemp==nullptr)rval=1;
    auto *tempbuf=(uint8_t *)mymalloc(SRAMIN,4096);				//����4096���ֽڿռ�
	if(tempbuf==nullptr)rval=1;
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//���ļ�ʧ��  
 	if(rval==0)	 
	{
		switch(fx)
		{
			case 0:												//����UNIGBK.BIN
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//��Ϣͷ֮�󣬽���UNIGBKת�����
				ftinfo.ugbksize=fftemp->fsize;					//UNIGBK��С
				flashaddr=ftinfo.ugbkaddr;
				break;
			case 1:
				ftinfo.f12addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBK֮�󣬽���GBK12�ֿ�
				ftinfo.gbk12size=fftemp->fsize;					//GBK12�ֿ��С
				flashaddr=ftinfo.f12addr;						//GBK12����ʼ��ַ
				break;
			case 2:
				ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;	//GBK12֮�󣬽���GBK16�ֿ�
				ftinfo.gbk16size=fftemp->fsize;					//GBK16�ֿ��С
				flashaddr=ftinfo.f16addr;						//GBK16����ʼ��ַ
				break;
			case 3:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16֮�󣬽���GBK24�ֿ�
				ftinfo.gkb24size=fftemp->fsize;					//GBK24�ֿ��С
				flashaddr=ftinfo.f24addr;						//GBK24����ʼ��ַ
				break;
            default:
                break;
		} 
			
		while(res==FR_OK)//��ѭ��ִ��
		{
	 		res=f_read(fftemp,tempbuf,4096,(UINT *)&bread);		//��ȡ����	 
			if(res!=FR_OK)break;								//ִ�д���
            if(Storage_Link::W25Q_Link != nullptr)
                Storage_Link::W25Q_Link->write(offx + flashaddr, tempbuf, 4096);
	  		offx+=bread;
            fupd_prog(fftemp->fsize, offx);	 			//������ʾ
			if(bread!=4096)break;								//������.
	 	} 	
		f_close(fftemp);		
	}			 
	myfree(SRAMIN,fftemp);	//�ͷ��ڴ�
	myfree(SRAMIN,tempbuf);	//�ͷ��ڴ�
    if(res>0)return false;
    return true;
} 
//���������ļ�,UNIGBK,GBK12,GBK16,GBK24һ�����
//x,y:��ʾ��Ϣ����ʾ��ַ
//size:�����С
//src:�ֿ���Դ����."0:",SD��;"1:",FLASH��,"2:",U��.
//��ʾ��Ϣ�����С										  
//����ֵ:0,���³ɹ�;
//		 ����,�������.	  
bool  Storage_Link::update_font(uint8_t size,uint8_t *src)
{
 	uint16_t i,j;
	uint8_t rval=0;
	ftinfo.fontok=0XFF;
    auto *pname=(uint8_t *)mymalloc(SRAMIN,100);	//����100�ֽ��ڴ�
    auto * buf=(uint32_t *)mymalloc(SRAMIN,4096);	//����4K�ֽ��ڴ�
    FIL *fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//�����ڴ�
	if(buf==nullptr||pname==nullptr||fftemp==nullptr)
	{
		myfree(SRAMIN,fftemp);
		myfree(SRAMIN,pname);
		myfree(SRAMIN,buf);
		return false;	//�ڴ�����ʧ��
	}
	//�Ȳ����ļ��Ƿ����� 
	strcpy((char*)pname,(char*)src);	//copy src���ݵ�pname
	strcat((char*)pname,(char*)UNIGBK_PATH);
    uint8_t res=f_open(fftemp,(const TCHAR*)pname,FA_READ);
 	if(res)rval|=1<<4;//���ļ�ʧ��  
	strcpy((char*)pname,(char*)src);	//copy src���ݵ�pname
	strcat((char*)pname,(char*)GBK12_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<5;//���ļ�ʧ��  
	strcpy((char*)pname,(char*)src);	//copy src���ݵ�pname
	strcat((char*)pname,(char*)GBK16_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<6;//���ļ�ʧ��  
	strcpy((char*)pname,(char*)src);	//copy src���ݵ�pname
	strcat((char*)pname,(char*)GBK24_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<7;//���ļ�ʧ��   
	myfree(SRAMIN,fftemp);//�ͷ��ڴ�
	if(rval==0)//�ֿ��ļ�������.
	{  
//		LCD_ShowString(x,y,240,320,size,"Erasing sectors... ");//��ʾ���ڲ�������
		for(i=0;i<FONTSECSIZE;i++)	//�Ȳ����ֿ�����,���д���ٶ�
		{
            fupd_prog(FONTSECSIZE, i);//������ʾ
            if(Storage_Link::W25Q_Link != nullptr) {
                Storage_Link::W25Q_Link->read(((FONTINFOADDR / 4096) + i) * 4096, (uint8_t *) buf, 4096);
                for (j = 0; j < 1024; j++)//У������
                {
                    if (buf[j] != 0XFFFFFFFF)break;//��Ҫ����
                }
                if (j != 1024)
                    Storage_Link::W25Q_Link->Erase_Sector((FONTINFOADDR / 4096) + i);
            }
		}
		myfree(SRAMIN,buf);
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)UNIGBK_PATH); 
		res= updata_fontx(pname, 0);	//����UNIGBK.BIN
		if(res){myfree(SRAMIN,pname);return false;}
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK12_PATH); 
		res= updata_fontx(pname, 1);	//����GBK12.FON
		if(res){myfree(SRAMIN,pname);return false;}
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK16_PATH); 
		res= updata_fontx(pname, 2);	//����GBK16.FON
		if(res){myfree(SRAMIN,pname);return false;}
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK24_PATH); 
		res= updata_fontx(pname, 3);	//����GBK24.FON
		if(res){myfree(SRAMIN,pname);return false;}
		//ȫ�����º���
		ftinfo.fontok=0XAA;
        if(Storage_Link::W25Q_Link != nullptr)
            Storage_Link::W25Q_Link->write(FONTINFOADDR, (uint8_t*)&ftinfo, sizeof(ftinfo));
	}
	myfree(SRAMIN,pname);//�ͷ��ڴ� 
	myfree(SRAMIN,buf);
    if (rval)return false;
    return true;
}

void   Storage_Link::font_read(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte)
{
    if(Storage_Link::W25Q_Link != nullptr)
        Storage_Link::W25Q_Link->read(Addr, pBuffer, NumByte);
}

uint32_t  Storage_Link::get_font_gbk_size()
{
    return ftinfo.ugbksize;
}

uint32_t  Storage_Link::get_font_gbk_ADD()
{
    return ftinfo.ugbkaddr;
}

//��ʼ������
//����ֵ:0,�ֿ����.
//		 ����,�ֿⶪʧ
bool Storage_Link::font_init()
{
//    FLASH_Interface=Font_fun;
	uint8_t t=0;
	while(t<10)//������ȡ10��,���Ǵ���,˵��ȷʵ��������,�ø����ֿ���
	{
		t++;
        if(Storage_Link::W25Q_Link != nullptr)
            Storage_Link::W25Q_Link->read(FONTINFOADDR, (uint8_t*)&ftinfo, sizeof(ftinfo));
		if(ftinfo.fontok==0XAA)break;
		delay_ms(20);
	}
	if(ftinfo.fontok!=0XAA)return false;
	return true;
}

bool Storage_Link::font_init(W25QXX *Font_fun) {
    Storage_Link::W25Q_Link=Font_fun;
    return font_init();
}

uint8_t Storage_Link::f_typetell(uint8_t *fname) {
    uint8_t tbuf[5];
    uint8_t *attr;//=(uint8_t )&('\0');//��׺��
    uint8_t i=0,j;
    while(i<250)
    {
        i++;
        if(*fname=='\0')break;//ƫ�Ƶ��������.
        fname++;
    }
    if(i==250)return Storage_Link::TYPE::ERR0R;//������ַ���.
    for(i=0;i<5;i++)//�õ���׺��
    {
        fname--;
        if(*fname=='.')
        {
            fname++;
            attr=fname;
            break;
        }
    }
    strcpy((char *)tbuf,(const char*)attr);//copy
    for(i=0;i<4;i++)tbuf[i]=ff_wtoupper(tbuf[i]);//ȫ����Ϊ��д
    for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//����Ա�
    {
        for(j=0;j<FILE_MAX_SUBT_NUM;j++)//����Ա�
        {
            if(*FILE_TYPE_TBL[i][j]==0)break;//�����Ѿ�û�пɶԱȵĳ�Ա��.
            if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//�ҵ���
            {
                return Storage_Link::TYPE::BIN+((i<<4)|j);
            }
        }
    }
    return Storage_Link::TYPE::ERR0R;//û�ҵ�
}

uint8_t Storage_Link::exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free)
{
    FATFS *fs1;
    uint8_t res;
    uint32_t fre_clust;
    //�õ�������Ϣ�����д�����
    res =(uint32_t) f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
    {
        uint32_t tot_sect=(fs1->n_fatent-2)*fs1->csize;	//�õ���������
        uint32_t fre_sect=fre_clust*fs1->csize;			//�õ�����������
#if _MAX_SS!=512				  				//������С����512�ֽ�,��ת��Ϊ512�ֽ�
        tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif
        *total=tot_sect>>1;	//��λΪKB
        *free=fre_sect>>1;	//��λΪKB
    }
    return res;
}

uint8_t  Storage_Link::disk_num=0;
char     Storage_Link::disk_name[_VOLUMES][3];
W25QXX*    Storage_Link::W25Q_Link;
Storage_BASE* Storage_Link:: Storage_L[_VOLUMES];
FATFS*     Storage_Link:: fs[_VOLUMES];

bool Storage_Link::exfuns_init(char *name)
{
    Storage_Link::fs[disk_num]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//Ϊ����i�����������ڴ�
    if(!fs[disk_num])return false;
    disk_name[disk_num][0]=name[0];
    if(FR_OK!=f_mount(fs[disk_num],name,1))
        return false;
    disk_num++;
    if(disk_num>_VOLUMES)disk_num=0;
    return true;
}

bool Storage_Link::exfuns_init(const char *name,Storage_BASE *Storage)
{
    Storage_Link::Storage_L[disk_num]=Storage;
    return Storage_Link::exfuns_init((char*)name);
}

bool Storage_Link::exfuns_init(const char *name, W25QXX *W25x,bool Font) {
    Storage_Link::W25Q_Link=W25x;
    Storage_Link::Storage_L[disk_num]=Storage_Link::W25Q_Link;
    bool ret=Storage_Link::exfuns_init((char*)name);
    if(ret&&Font){
        ret=Storage_Link::font_init();
    }
    return ret;
}

bool Storage_Link::disk_init(BYTE name) {
    uint8_t ii;
    for(ii=0;ii<_VOLUMES;ii++)
        if(disk_name[ii][0]-'0'==name)
            break;
    if(ii==_VOLUMES)return false;

    return Storage_L[ii]->to_init();
}

bool Storage_Link::disk_write(BYTE name, const BYTE *buff, DWORD sector, UINT count) {
    uint8_t ii;
    for(ii=0;ii<_VOLUMES;ii++)
        if(disk_name[ii][0]-'0'==name)
            break;
    if(ii==_VOLUMES)return false;

    if(Storage_L[ii]->write(sector,(BYTE *)buff,count)>0)
        return true;
    return false;
}

bool Storage_Link::disk_read(BYTE name, BYTE *buff, DWORD sector, UINT count) {
    uint8_t ii;
    for(ii=0;ii<_VOLUMES;ii++)
        if(disk_name[ii][0]-'0'==name)
            break;
    if(ii==_VOLUMES)return false;

    Storage_L[ii]->read(sector,buff,count);
    return true;
}

bool Storage_Link::disk_ioctl(BYTE name, BYTE cmd, void *buff) {
    uint8_t ii;
    for(ii=0;ii<_VOLUMES;ii++)
        if(disk_name[ii][0]-'0'==name)
            break;
    if(ii==_VOLUMES)return false;
    bool res= true;
    switch(cmd)
    {
        case CTRL_SYNC:
            if(!Storage_L[ii]->WaitReady())res = false;
            break;
        case GET_SECTOR_SIZE:
            *(WORD*)buff = Storage_L[ii]->Get_Sector_Size();
            break;
        case GET_BLOCK_SIZE:
            *(WORD*)buff = Storage_L[ii]->Get_Block_Size();
            break;
        case GET_SECTOR_COUNT:
            *(DWORD*)buff = Storage_L[ii]->GetSectorCount();
            break;
        default:
            res = false;
            break;
    }
    return res;
}


////���ȡ�ļ���
////����ֵ:ִ�н��
//uint8_t mf_readdir()
//{
//	uint8_t res;
//	char *fn;
//#if _USE_LFN
// 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
//	fileinfo.lfname = (TCHAR*)mymalloc(SRAMIN,fileinfo.lfsize);
//#endif
//	res=f_readdir(&dir,&fileinfo);//��ȡһ���ļ�����Ϣ
//	if(res!=FR_OK||fileinfo.fname[0]==0)
//	{
//		myfree(SRAMIN,fileinfo.lfname);
//		return res;//������.
//	}
//#if _USE_LFN
//	fn=*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
//#else
//	fn=fileinfo.fname;;
//#endif
//	myfree(SRAMIN,fileinfo.lfname);
//	return 0;
//}
//
// //�����ļ�
// //path:·��
// //����ֵ:ִ�н��
//uint8_t mf_scan_files(uint8_t * path)
//{
//	FRESULT res;
//    char *fn;   /* This function is assuming non-Unicode cfg. */
//#if _USE_LFN
// 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
//	fileinfo.lfname = (TCHAR*)mymalloc(SRAMIN,fileinfo.lfsize);
//#endif
//
//    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
//    if (res == FR_OK)
//	{
//		while(true)
//		{
//	        res = f_readdir(&dir, &fileinfo);      //��ȡĿ¼�µ�һ���ļ�
//	        if (res != FR_OK || fileinfo.fname[0] == 0) break;//������/��ĩβ��,�˳�
//	        if (fileinfo.fname[0] == '.') continue;        //�����ϼ�Ŀ¼
//#if _USE_LFN
//        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
//#else
//        	fn = fileinfo.fname;
//#endif	                                              /* It is a file. */
//		}
//    }
//	myfree(SRAMIN,fileinfo.lfname);
//    return res;
//}



