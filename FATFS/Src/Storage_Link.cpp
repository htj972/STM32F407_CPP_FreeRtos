#include "Storage_Link.h"
#include "ff.h"
#include <cstring>
#include "malloc.h"
#include "delay.h"
#include "diskio.h"

//字库区域占用的总扇区数大小(3个字库+unigbk表+字库信息=3238700字节,约占791个W25QXX扇区)
#define FONTSECSIZE	 	791
//字库存放起始地址 
#define FONTINFOADDR 	(1024*1024*12) 					//Explorer STM32F4是从12M地址以后开始存放字库
														//前面12M被fatfs占用了.
														//12M以后紧跟3个字库+UNIGBK.BIN,总大小3.09M,被字库占用了,不能动!
														//15.10M以后,用户可以自由使用.建议用最后的100K字节比较好.

//字库信息结构体定义
//用来保存字库基本信息，地址，大小等
//__packed typedef struct
//字体信息保存地址,占33个字节,第1个字节用于标记字库是否存在.后续每8个字节一组,分别保存起始地址和文件大小
typedef struct __attribute__ ((packed)) font_info_
{
    uint8_t fontok;				//字库存在标志，0XAA，字库正常；其他，字库不存在
    uint32_t ugbkaddr; 			//unigbk的地址
    uint32_t ugbksize;			//unigbk的大小
    uint32_t f12addr;			//gbk12地址
    uint32_t gbk12size;			//gbk12的大小
    uint32_t f16addr;			//gbk16地址
    uint32_t gbk16size;			//gbk16的大小
    uint32_t f24addr;			//gbk24地址
    uint32_t gkb24size;			//gbk24的大小
}font_info;
font_info ftinfo;

//字库存放在磁盘中的路径
const char* GBK24_PATH="/SYSTEM/FONT/GBK24.FON";		//GBK24的存放位置
const char* GBK16_PATH="/SYSTEM/FONT/GBK16.FON";		//GBK16的存放位置
const char* GBK12_PATH="/SYSTEM/FONT/GBK12.FON";		//GBK12的存放位置
const char* UNIGBK_PATH="/SYSTEM/FONT/UNIGBK.BIN";		//UNIGBK.BIN的存放位置

#define FILE_MAX_TYPE_NUM		7	//最多FILE_MAX_TYPE_NUM个大类
#define FILE_MAX_SUBT_NUM		4	//最多FILE_MAX_SUBT_NUM个小类

//文件类型列表
const char*  FILE_TYPE_TBL[FILE_MAX_TYPE_NUM][FILE_MAX_SUBT_NUM]=
        {
                {"BIN"},			//BIN文件
                {"LRC"},			//LRC文件
                {"NES"},			//NES文件
                {"TXT","C","CPP","H"},	//文本文件
                {"WAV","MP3","APE","FLAC"},//支持的音乐文件
                {"BMP","JPG","JPEG","GIF"},//图片文件
                {"AVI"},			//视频文件
        };

//显示当前字体更新进度
//x,y:坐标
//size:字体大小
//fsize:整个文件大小
//pos:当前文件指针位置
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

//更新某一个
//x,y:坐标
//size:字体大小
//fxpath:路径
//fx:更新的内容 0,ungbk;1,gbk12;2,gbk16;3,gbk24;
bool  Storage_Link::updata_fontx(uint8_t *fxpath, uint8_t fx)
{
	uint32_t flashaddr=0;
 	uint8_t res;
	uint16_t bread;
	uint32_t offx=0;
	uint8_t rval=0;
    FIL *fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//分配内存
	if(fftemp==nullptr)rval=1;
    auto *tempbuf=(uint8_t *)mymalloc(SRAMIN,4096);				//分配4096个字节空间
	if(tempbuf==nullptr)rval=1;
 	res=f_open(fftemp,(const TCHAR*)fxpath,FA_READ); 
 	if(res)rval=2;//打开文件失败  
 	if(rval==0)	 
	{
		switch(fx)
		{
			case 0:												//更新UNIGBK.BIN
				ftinfo.ugbkaddr=FONTINFOADDR+sizeof(ftinfo);	//信息头之后，紧跟UNIGBK转换码表
				ftinfo.ugbksize=fftemp->fsize;					//UNIGBK大小
				flashaddr=ftinfo.ugbkaddr;
				break;
			case 1:
				ftinfo.f12addr=ftinfo.ugbkaddr+ftinfo.ugbksize;	//UNIGBK之后，紧跟GBK12字库
				ftinfo.gbk12size=fftemp->fsize;					//GBK12字库大小
				flashaddr=ftinfo.f12addr;						//GBK12的起始地址
				break;
			case 2:
				ftinfo.f16addr=ftinfo.f12addr+ftinfo.gbk12size;	//GBK12之后，紧跟GBK16字库
				ftinfo.gbk16size=fftemp->fsize;					//GBK16字库大小
				flashaddr=ftinfo.f16addr;						//GBK16的起始地址
				break;
			case 3:
				ftinfo.f24addr=ftinfo.f16addr+ftinfo.gbk16size;	//GBK16之后，紧跟GBK24字库
				ftinfo.gkb24size=fftemp->fsize;					//GBK24字库大小
				flashaddr=ftinfo.f24addr;						//GBK24的起始地址
				break;
            default:
                break;
		} 
			
		while(res==FR_OK)//死循环执行
		{
	 		res=f_read(fftemp,tempbuf,4096,(UINT *)&bread);		//读取数据	 
			if(res!=FR_OK)break;								//执行错误
            if(Storage_Link::W25Q_Link != nullptr)
                Storage_Link::W25Q_Link->write(offx + flashaddr, tempbuf, 4096);
	  		offx+=bread;
            fupd_prog(fftemp->fsize, offx);	 			//进度显示
			if(bread!=4096)break;								//读完了.
	 	} 	
		f_close(fftemp);		
	}			 
	myfree(SRAMIN,fftemp);	//释放内存
	myfree(SRAMIN,tempbuf);	//释放内存
    if(res>0)return false;
    return true;
} 
//更新字体文件,UNIGBK,GBK12,GBK16,GBK24一起更新
//x,y:提示信息的显示地址
//size:字体大小
//src:字库来源磁盘."0:",SD卡;"1:",FLASH盘,"2:",U盘.
//提示信息字体大小										  
//返回值:0,更新成功;
//		 其他,错误代码.	  
bool  Storage_Link::update_font(uint8_t size,uint8_t *src)
{
 	uint16_t i,j;
	uint8_t rval=0;
	ftinfo.fontok=0XFF;
    auto *pname=(uint8_t *)mymalloc(SRAMIN,100);	//申请100字节内存
    auto * buf=(uint32_t *)mymalloc(SRAMIN,4096);	//申请4K字节内存
    FIL *fftemp=(FIL*)mymalloc(SRAMIN,sizeof(FIL));	//分配内存
	if(buf==nullptr||pname==nullptr||fftemp==nullptr)
	{
		myfree(SRAMIN,fftemp);
		myfree(SRAMIN,pname);
		myfree(SRAMIN,buf);
		return false;	//内存申请失败
	}
	//先查找文件是否正常 
	strcpy((char*)pname,(char*)src);	//copy src内容到pname
	strcat((char*)pname,(char*)UNIGBK_PATH);
    uint8_t res=f_open(fftemp,(const TCHAR*)pname,FA_READ);
 	if(res)rval|=1<<4;//打开文件失败  
	strcpy((char*)pname,(char*)src);	//copy src内容到pname
	strcat((char*)pname,(char*)GBK12_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<5;//打开文件失败  
	strcpy((char*)pname,(char*)src);	//copy src内容到pname
	strcat((char*)pname,(char*)GBK16_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<6;//打开文件失败  
	strcpy((char*)pname,(char*)src);	//copy src内容到pname
	strcat((char*)pname,(char*)GBK24_PATH); 
 	res=f_open(fftemp,(const TCHAR*)pname,FA_READ); 
 	if(res)rval|=1<<7;//打开文件失败   
	myfree(SRAMIN,fftemp);//释放内存
	if(rval==0)//字库文件都存在.
	{  
//		LCD_ShowString(x,y,240,320,size,"Erasing sectors... ");//提示正在擦除扇区
		for(i=0;i<FONTSECSIZE;i++)	//先擦除字库区域,提高写入速度
		{
            fupd_prog(FONTSECSIZE, i);//进度显示
            if(Storage_Link::W25Q_Link != nullptr) {
                Storage_Link::W25Q_Link->read(((FONTINFOADDR / 4096) + i) * 4096, (uint8_t *) buf, 4096);
                for (j = 0; j < 1024; j++)//校验数据
                {
                    if (buf[j] != 0XFFFFFFFF)break;//需要擦除
                }
                if (j != 1024)
                    Storage_Link::W25Q_Link->Erase_Sector((FONTINFOADDR / 4096) + i);
            }
		}
		myfree(SRAMIN,buf);
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)UNIGBK_PATH); 
		res= updata_fontx(pname, 0);	//更新UNIGBK.BIN
		if(res){myfree(SRAMIN,pname);return false;}
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)GBK12_PATH); 
		res= updata_fontx(pname, 1);	//更新GBK12.FON
		if(res){myfree(SRAMIN,pname);return false;}
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)GBK16_PATH); 
		res= updata_fontx(pname, 2);	//更新GBK16.FON
		if(res){myfree(SRAMIN,pname);return false;}
		strcpy((char*)pname,(char*)src);				//copy src内容到pname
		strcat((char*)pname,(char*)GBK24_PATH); 
		res= updata_fontx(pname, 3);	//更新GBK24.FON
		if(res){myfree(SRAMIN,pname);return false;}
		//全部更新好了
		ftinfo.fontok=0XAA;
        if(Storage_Link::W25Q_Link != nullptr)
            Storage_Link::W25Q_Link->write(FONTINFOADDR, (uint8_t*)&ftinfo, sizeof(ftinfo));
	}
	myfree(SRAMIN,pname);//释放内存 
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

//初始化字体
//返回值:0,字库完好.
//		 其他,字库丢失
bool Storage_Link::font_init()
{
//    FLASH_Interface=Font_fun;
	uint8_t t=0;
	while(t<10)//连续读取10次,都是错误,说明确实是有问题,得更新字库了
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
    uint8_t *attr;//=(uint8_t )&('\0');//后缀名
    uint8_t i=0,j;
    while(i<250)
    {
        i++;
        if(*fname=='\0')break;//偏移到了最后了.
        fname++;
    }
    if(i==250)return Storage_Link::TYPE::ERR0R;//错误的字符串.
    for(i=0;i<5;i++)//得到后缀名
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
    for(i=0;i<4;i++)tbuf[i]=ff_wtoupper(tbuf[i]);//全部变为大写
    for(i=0;i<FILE_MAX_TYPE_NUM;i++)	//大类对比
    {
        for(j=0;j<FILE_MAX_SUBT_NUM;j++)//子类对比
        {
            if(*FILE_TYPE_TBL[i][j]==0)break;//此组已经没有可对比的成员了.
            if(strcmp((const char *)FILE_TYPE_TBL[i][j],(const char *)tbuf)==0)//找到了
            {
                return Storage_Link::TYPE::BIN+((i<<4)|j);
            }
        }
    }
    return Storage_Link::TYPE::ERR0R;//没找到
}

uint8_t Storage_Link::exf_getfree(uint8_t *drv,uint32_t *total,uint32_t *free)
{
    FATFS *fs1;
    uint8_t res;
    uint32_t fre_clust;
    //得到磁盘信息及空闲簇数量
    res =(uint32_t) f_getfree((const TCHAR*)drv, (DWORD*)&fre_clust, &fs1);
    if(res==0)
    {
        uint32_t tot_sect=(fs1->n_fatent-2)*fs1->csize;	//得到总扇区数
        uint32_t fre_sect=fre_clust*fs1->csize;			//得到空闲扇区数
#if _MAX_SS!=512				  				//扇区大小不是512字节,则转换为512字节
        tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif
        *total=tot_sect>>1;	//单位为KB
        *free=fre_sect>>1;	//单位为KB
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
    Storage_Link::fs[disk_num]=(FATFS*)mymalloc(SRAMIN,sizeof(FATFS));	//为磁盘i工作区申请内存
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


////打读取文件夹
////返回值:执行结果
//uint8_t mf_readdir()
//{
//	uint8_t res;
//	char *fn;
//#if _USE_LFN
// 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
//	fileinfo.lfname = (TCHAR*)mymalloc(SRAMIN,fileinfo.lfsize);
//#endif
//	res=f_readdir(&dir,&fileinfo);//读取一个文件的信息
//	if(res!=FR_OK||fileinfo.fname[0]==0)
//	{
//		myfree(SRAMIN,fileinfo.lfname);
//		return res;//读完了.
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
// //遍历文件
// //path:路径
// //返回值:执行结果
//uint8_t mf_scan_files(uint8_t * path)
//{
//	FRESULT res;
//    char *fn;   /* This function is assuming non-Unicode cfg. */
//#if _USE_LFN
// 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
//	fileinfo.lfname = (TCHAR*)mymalloc(SRAMIN,fileinfo.lfsize);
//#endif
//
//    res = f_opendir(&dir,(const TCHAR*)path); //打开一个目录
//    if (res == FR_OK)
//	{
//		while(true)
//		{
//	        res = f_readdir(&dir, &fileinfo);      //读取目录下的一个文件
//	        if (res != FR_OK || fileinfo.fname[0] == 0) break;//错误了/到末尾了,退出
//	        if (fileinfo.fname[0] == '.') continue;        //忽略上级目录
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



