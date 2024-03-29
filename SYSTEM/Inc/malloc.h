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

//定义三个内存池
#define SRAMIN	 0		//内部内存池
//#define SRAMEX   1		//外部内存池
#define SRAMCCM  1		//CCM内存池(此部分SRAM仅仅CPU可以访问!!!)


#define SRAMBANK 	2	//定义支持的SRAM块数.


//mem1内存参数设定.mem1完全处于内部SRAM里面.
#define RAM_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define RAM_MAX_SIZE			(60*1024)  						//最大管理内存 100K
#define RAM_ALLOC_TABLE_SIZE	(RAM_MAX_SIZE/RAM_BLOCK_SIZE) 	//内存表大小

//mem2内存参数设定.mem2的内存池处于外部SRAM里面
#define SRAM_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define SRAM_MAX_SIZE			(0 *1024 ) 						//最大管理内存960K
#define SRAM_ALLOC_TABLE_SIZE	(SRAM_MAX_SIZE/SRAM_BLOCK_SIZE) 	//内存表大小

//mem3内存参数设定.mem3处于CCM,用于管理CCM(特别注意,这部分SRAM,仅CPU可以访问!!)
#define CCM_BLOCK_SIZE			32  	  						//内存块大小为32字节
#define CCM_MAX_SIZE			(50 *1024 ) 						//最大管理内存60K
#define CCM_ALLOC_TABLE_SIZE	(CCM_MAX_SIZE/CCM_BLOCK_SIZE) 	//内存表大小



//内存管理控制器
struct m_mallco_dev
{
    void    (*init)(uint8_t);					//初始化
    uint8_t (*perused)(uint8_t);		  	    	//内存使用率
    uint8_t *membase[SRAMBANK];				//内存池 管理SRAMBANK个区域的内存
    u16     *memmap[SRAMBANK]; 				//内存管理状态表
    uint8_t  memrdy[SRAMBANK]; 				//内存管理是否就绪
};
extern struct m_mallco_dev mallco_dev;	 //在mallco.c里面定义

void mymemset(void *s,uint8_t c,uint32_t count);	//设置内存
void mymemcpy(void *des,void *src,uint32_t n);//复制内存
void my_mem_init(uint8_t memx);				//内存管理初始化函数(外/内部调用)
uint32_t my_mem_malloc(uint8_t memx,uint32_t size);	//内存分配(内部调用)
uint8_t my_mem_free(uint8_t memx,uint32_t offset);		//内存释放(内部调用)
uint8_t my_mem_perused(uint8_t memx);				//获得内存使用率(外/内部调用)
////////////////////////////////////////////////////////////////////////////////
//用户调用函数
void myfree(uint8_t memx,void *ptr);  			//内存释放(外部调用)
void *mymalloc(uint8_t memx,uint32_t size);			//内存分配(外部调用)
void *myrealloc(uint8_t memx,void *ptr,uint32_t size);//重新分配内存(外部调用)


#if __cplusplus
}
#endif

#endif //KOKIRIKA_MALLOC_H
