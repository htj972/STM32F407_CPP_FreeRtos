/**
* @Author kokirika
* @Name MALLOC
* @Date 2022-10-27
**/

#ifndef KOKIRIKA_MALLOC_H
#define KOKIRIKA_MALLOC_H

#include <sys.h>
#include "stddef.h"

#if __cplusplus
extern "C"{
#endif

//���������ڴ��
#define SRAMIN	 0		//�ڲ��ڴ��
//#define SRAMEX   1		//�ⲿ�ڴ��
#define SRAMCCM  1		//CCM�ڴ��(�˲���SRAM����CPU���Է���!!!)


#define SRAMBANK 	1	//����֧�ֵ�SRAM����.


//mem1�ڴ�����趨.mem1��ȫ�����ڲ�SRAM����.
#define MEM1_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM1_MAX_SIZE			50*1024  						//�������ڴ� 100K
#define MEM1_ALLOC_TABLE_SIZE	MEM1_MAX_SIZE/MEM1_BLOCK_SIZE 	//�ڴ���С

//mem2�ڴ�����趨.mem2���ڴ�ش����ⲿSRAM����
#define MEM2_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM2_MAX_SIZE			0 *1024  						//�������ڴ�960K
#define MEM2_ALLOC_TABLE_SIZE	MEM2_MAX_SIZE/MEM2_BLOCK_SIZE 	//�ڴ���С

//mem3�ڴ�����趨.mem3����CCM,���ڹ���CCM(�ر�ע��,�ⲿ��SRAM,��CPU���Է���!!)
#define MEM3_BLOCK_SIZE			32  	  						//�ڴ���СΪ32�ֽ�
#define MEM3_MAX_SIZE			60 *1024  						//�������ڴ�60K
#define MEM3_ALLOC_TABLE_SIZE	MEM3_MAX_SIZE/MEM3_BLOCK_SIZE 	//�ڴ���С



//�ڴ���������
struct _m_mallco_dev
{
    void    (*init)(uint8_t);					//��ʼ��
    uint8_t (*perused)(uint8_t);		  	    	//�ڴ�ʹ����
    uint8_t *membase[SRAMBANK];				//�ڴ�� ����SRAMBANK��������ڴ�
    u16     *memmap[SRAMBANK]; 				//�ڴ����״̬��
    uint8_t  memrdy[SRAMBANK]; 				//�ڴ�����Ƿ����
};
extern struct _m_mallco_dev mallco_dev;	 //��mallco.c���涨��

void mymemset(void *s,uint8_t c,uint32_t count);	//�����ڴ�
void mymemcpy(void *des,void *src,uint32_t n);//�����ڴ�
void my_mem_init(uint8_t memx);				//�ڴ�����ʼ������(��/�ڲ�����)
uint32_t my_mem_malloc(uint8_t memx,uint32_t size);	//�ڴ����(�ڲ�����)
uint8_t my_mem_free(uint8_t memx,uint32_t offset);		//�ڴ��ͷ�(�ڲ�����)
uint8_t my_mem_perused(uint8_t memx);				//����ڴ�ʹ����(��/�ڲ�����)
////////////////////////////////////////////////////////////////////////////////
//�û����ú���
void myfree(uint8_t memx,void *ptr);  			//�ڴ��ͷ�(�ⲿ����)
void *mymalloc(uint8_t memx,uint32_t size);			//�ڴ����(�ⲿ����)
void *myrealloc(uint8_t memx,void *ptr,uint32_t size);//���·����ڴ�(�ⲿ����)


#if __cplusplus
}
#endif

#endif //KOKIRIKA_MALLOC_H
