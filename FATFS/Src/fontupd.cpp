#include "fontupd.h"
#include "ff.h"
#include <cstring>
#include "malloc.h"
#include "delay.h"

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

//��ʾ��ǰ������½���
//x,y:����
//size:�����С
//fsize:�����ļ���С
//pos:��ǰ�ļ�ָ��λ��
uint32_t  Storage_Link::fupd_prog(uint16_t x, uint16_t y, uint32_t fsize, uint32_t pos)
{
	float prog;
    float t=0XFF;
	prog=(float)pos/(float)fsize;
	prog*=100;
	if(t!=prog)
	{
		//LCD_ShowString(x+3*size/2,y,240,320,size,"%");		
		t=prog;
		if(t>100)t=100;
	}
	return 0;					    
}

//����ĳһ��
//x,y:����
//size:�����С
//fxpath:·��
//fx:���µ����� 0,ungbk;1,gbk12;2,gbk16;3,gbk24;
bool  Storage_Link::updata_fontx(uint16_t x, uint16_t y, uint8_t *fxpath, uint8_t fx)
{
	uint32_t flashaddr=0;
 	uint8_t res;
	uint16_t bread;
	uint32_t offx=0;
	uint8_t rval=0;
    FIL *fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//�����ڴ�
	if(fftemp==nullptr)rval=1;
    uint8_t *tempbuf=(uint8_t *)mymalloc(SRAMIN,4096);				//����4096���ֽڿռ�
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
//			W25QXX_Write(tempbuf,offx+flashaddr,4096);		//��0��ʼд��4096������
            Link->write(offx+flashaddr,tempbuf,4096);
	  		offx+=bread;
            fupd_prog(x, y, fftemp->fsize, offx);	 			//������ʾ
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
bool  Storage_Link::update_font(uint16_t x,uint16_t y,uint8_t size,uint8_t *src)
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
            fupd_prog(x + 20 * size / 2, y, FONTSECSIZE, i);//������ʾ
//			W25QXX_Read((uint8_t )buf,((FONTINFOADDR/4096)+i)*4096,4096);//������������������
            Link->read(((FONTINFOADDR/4096)+i)*4096,(uint8_t*)buf,4096);
			for(j=0;j<1024;j++)//У������
			{
				if(buf[j]!=0XFFFFFFFF)break;//��Ҫ����  	  
			}
//			if(j!=1024)W25QXX_Erase_Sector((FONTINFOADDR/4096)+i);	//��Ҫ����������
            if(j!=1024)Link->Erase_Sector((FONTINFOADDR/4096)+i);
		}
		myfree(SRAMIN,buf);
//		LCD_ShowString(x,y,240,320,size,"Updating UNIGBK.BIN");
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)UNIGBK_PATH); 
		res= updata_fontx(x + 20 * size / 2, y, pname, 0);	//����UNIGBK.BIN
		if(res){myfree(SRAMIN,pname);return false;}
//		LCD_ShowString(x,y,240,320,size,"Updating GBK12.BIN  ");
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK12_PATH); 
		res= updata_fontx(x + 20 * size / 2, y, pname, 1);	//����GBK12.FON
		if(res){myfree(SRAMIN,pname);return false;}
//		LCD_ShowString(x,y,240,320,size,"Updating GBK16.BIN  ");
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK16_PATH); 
		res= updata_fontx(x + 20 * size / 2, y, pname, 2);	//����GBK16.FON
		if(res){myfree(SRAMIN,pname);return false;}
//		LCD_ShowString(x,y,240,320,size,"Updating GBK24.BIN  ");
		strcpy((char*)pname,(char*)src);				//copy src���ݵ�pname
		strcat((char*)pname,(char*)GBK24_PATH); 
		res= updata_fontx(x + 20 * size / 2, y, pname, 3);	//����GBK24.FON
		if(res){myfree(SRAMIN,pname);return false;}
		//ȫ�����º���
		ftinfo.fontok=0XAA;
//		W25QXX_Write((uint8_t )&ftinfo,FONTINFOADDR,sizeof(ftinfo));	//�����ֿ���Ϣ
        Link->write(FONTINFOADDR,(uint8_t*)&ftinfo,sizeof(ftinfo));
	}
	myfree(SRAMIN,pname);//�ͷ��ڴ� 
	myfree(SRAMIN,buf);
    if (rval)return false;
    return true;
}

void   Storage_Link::font_read(uint32_t Addr , uint8_t *pBuffer, uint16_t NumByte)
{
    Link->read(Addr,pBuffer,NumByte);
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
//		W25QXX_Read((uint8_t )&ftinfo,FONTINFOADDR,sizeof(ftinfo));//����ftinfo�ṹ������
        Link->read(FONTINFOADDR,(uint8_t*)&ftinfo,sizeof(ftinfo));
		if(ftinfo.fontok==0XAA)break;
		delay_ms(20);
	}
	if(ftinfo.fontok!=0XAA)return false;
	return true;
}

bool Storage_Link::font_init(W25QXX *Font_fun) {
    Link=Font_fun;
    return font_init();
}
