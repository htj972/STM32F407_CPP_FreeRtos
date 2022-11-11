#include "fattester.h"
#include "exfuns.h"
#include "malloc.h"
#include "ff.h"
//Ϊ����ע�Ṥ����
//path:����·��������"0:"��"1:"
//mt:0��������ע�ᣨ�Ժ�ע�ᣩ��1������ע��
//����ֵ:ִ�н��
uint8_t mf_mount(uint8_t* path,uint8_t mt)
{
	return (uint8_t)f_mount((FATFS *)fs[2],(const TCHAR*)path,mt);
}
//��·���µ��ļ�
//path:·��+�ļ���
//mode:��ģʽ
//����ֵ:ִ�н��
uint8_t mf_open(uint8_t*path,uint8_t mode)
{
	uint8_t res;
	res=f_open(file,(const TCHAR*)path,mode);//���ļ���
	return res;
}
//�ر��ļ�
//����ֵ:ִ�н��
uint8_t mf_close()
{
	f_close(file);
	return 0;
}
//��������
//len:�����ĳ���
//����ֵ:ִ�н��
uint8_t mf_read(uint16_t len)
{
	uint16_t i;
	int res=0;
	uint16_t tlen=0;
	for(i=0;i<len/512;i++)
	{
		res=f_read(file,fatbuf,512,&br);
		if(res)
		{
			break;
		}else
		{
			tlen+=br;
		}
	}
	if(len%512)
	{
		res=f_read(file,fatbuf,len%512,&br);
//		if(res!=1)
//		{
//            tlen =tlen+br;
//		}
	}
	return res;
}
//д������
//dat:���ݻ�����
//len:д�볤��
//����ֵ:ִ�н��
uint8_t mf_write(uint8_t*dat,uint16_t len)
{
	uint8_t res;
	res=f_write(file,dat,len,&bw);
	return res;
}

//��Ŀ¼
 //path:·��
//����ֵ:ִ�н��
uint8_t mf_opendir(uint8_t* path)
{
	return f_opendir(&dir,(const TCHAR*)path);
}
//�ر�Ŀ¼
//����ֵ:ִ�н��
uint8_t mf_closedir()
{
	return f_closedir(&dir);
}
//���ȡ�ļ���
//����ֵ:ִ�н��
uint8_t mf_readdir()
{
	uint8_t res;
	char *fn;
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = (TCHAR*)mymalloc(SRAMIN,fileinfo.lfsize);
#endif
	res=f_readdir(&dir,&fileinfo);//��ȡһ���ļ�����Ϣ
	if(res!=FR_OK||fileinfo.fname[0]==0)
	{
		myfree(SRAMIN,fileinfo.lfname);
		return res;//������.
	}
#if _USE_LFN
	fn=*fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else
	fn=fileinfo.fname;;
#endif
	myfree(SRAMIN,fileinfo.lfname);
	return 0;
}

 //�����ļ�
 //path:·��
 //����ֵ:ִ�н��
uint8_t mf_scan_files(uint8_t * path)
{
	FRESULT res;
    char *fn;   /* This function is assuming non-Unicode cfg. */
#if _USE_LFN
 	fileinfo.lfsize = _MAX_LFN * 2 + 1;
	fileinfo.lfname = (TCHAR*)mymalloc(SRAMIN,fileinfo.lfsize);
#endif

    res = f_opendir(&dir,(const TCHAR*)path); //��һ��Ŀ¼
    if (res == FR_OK)
	{
		while(true)
		{
	        res = f_readdir(&dir, &fileinfo);                   //��ȡĿ¼�µ�һ���ļ�
	        if (res != FR_OK || fileinfo.fname[0] == 0) break;  //������/��ĩβ��,�˳�
	        if (fileinfo.fname[0] == '.') continue;             //�����ϼ�Ŀ¼
#if _USE_LFN
        	fn = *fileinfo.lfname ? fileinfo.lfname : fileinfo.fname;
#else
        	fn = fileinfo.fname;
#endif	                                              /* It is a file. */
		}
    }
	myfree(SRAMIN,fileinfo.lfname);
    return res;
}
//��ʾʣ������
//drv:�̷�
//����ֵ:ʣ������(�ֽ�)
uint32_t mf_showfree(uint8_t *drv)
{
	FATFS *fs1;
	uint8_t res;
    uint32_t fre_clust=0, fre_sect=0, tot_sect=0;
    //�õ�������Ϣ�����д�����
    res = f_getfree((const TCHAR*)drv,(DWORD*)&fre_clust, &fs1);
    if(res==0)
	{
	    tot_sect = (fs1->n_fatent - 2) * fs1->csize;//�õ���������
	    fre_sect = fre_clust * fs1->csize;			//�õ�����������
#if _MAX_SS!=512
		tot_sect*=fs1->ssize/512;
		fre_sect*=fs1->ssize/512;
#endif
//		if(tot_sect<20480)//������С��10M
//		{
//		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
//		    printf("\r\n����������:%d KB\r\n"
//		           "���ÿռ�:%d KB\r\n",
//		           tot_sect>>1,fre_sect>>1);
//		}else
//		{
//		    /* Print free space in unit of KB (assuming 512 bytes/sector) */
//		    printf("\r\n����������:%d MB\r\n"
//		           "���ÿռ�:%d MB\r\n",
//		           tot_sect>>11,fre_sect>>11);
//		}
	}
	return fre_sect;
}
//�ļ���дָ��ƫ��
//offset:����׵�ַ��ƫ����
//����ֵ:ִ�н��.
uint8_t mf_lseek(uint32_t offset)
{
	return f_lseek(file,offset);
}
//��ȡ�ļ���ǰ��дָ���λ��.
//����ֵ:λ��
uint32_t mf_tell(void)
{
	return f_tell(file);
}
//��ȡ�ļ���С
//����ֵ:�ļ���С
uint32_t mf_size(void)
{
	return f_size(file);
}
//����Ŀ¼
//pname:Ŀ¼·��+����
//����ֵ:ִ�н��
uint8_t mf_mkdir(uint8_t*pname)
{
	return f_mkdir((const TCHAR *)pname);
}
//��ʽ��
//path:����·��������"0:"��"1:"
//mode:ģʽ
//au:�ش�С
//����ֵ:ִ�н��
uint8_t mf_fmkfs(uint8_t* path,uint8_t mode,uint16_t au)
{
	return f_mkfs((const TCHAR*)path,mode,au);//��ʽ��,drv:�̷�;mode:ģʽ;au:�ش�С
}
//ɾ���ļ�/Ŀ¼
//pname:�ļ�/Ŀ¼·��+����
//����ֵ:ִ�н��
uint8_t mf_unlink(uint8_t *pname)
{
	return  f_unlink((const TCHAR *)pname);
}

//�޸��ļ�/Ŀ¼����(���Ŀ¼��ͬ,�������ƶ��ļ�Ŷ!)
//oldname:֮ǰ������
//newname:������
//����ֵ:ִ�н��
uint8_t mf_rename(uint8_t *oldname,uint8_t* newname)
{
	return  f_rename((const TCHAR *)oldname,(const TCHAR *)newname);
}
//��ȡ�̷����������֣�
//path:����·��������"0:"��"1:"
void mf_getlabel(uint8_t *path)
{
	uint8_t buf[20];
	uint32_t sn=0;
	uint8_t res;
	res=f_getlabel ((const TCHAR *)path,(TCHAR *)buf,(DWORD*)&sn);
}
//�����̷����������֣����11���ַ�������֧�����ֺʹ�д��ĸ����Լ����ֵ�
//path:���̺�+���֣�����"0:ALIENTEK"��"1:OPENEDV"
void mf_setlabel(uint8_t *path)
{
	uint8_t res;
	res=f_setlabel ((const TCHAR *)path);
}

//���ļ������ȡһ���ַ���
//size:Ҫ��ȡ�ĳ���
void mf_gets(uint16_t size)
{
 	TCHAR* rbuf;
	rbuf=f_gets((TCHAR*)fatbuf,size,file);
}
//��Ҫ_USE_STRFUNC>=1
//дһ���ַ����ļ�
//c:Ҫд����ַ�
//����ֵ:ִ�н��
uint8_t mf_putc(uint8_t c)
{
	return f_putc((TCHAR)c,file);
}
//д�ַ������ļ�
//c:Ҫд����ַ���
//����ֵ:д����ַ�������
uint8_t mf_puts(uint8_t*c)
{
	return f_puts((TCHAR*)c,file);
}













