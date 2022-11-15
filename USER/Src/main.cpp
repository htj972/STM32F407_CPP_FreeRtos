#include <cstring>
#include "sys.h"
#include "delay.h"
#include "USART.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "OLED_SSD1306.h"
#include "SD_SPI.h"
#include "Storage_Link.h"

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		128
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TASK1_TASK_PRIO		2
//任务堆栈大小
#define TASK1_STK_SIZE 		512
//任务句柄
TaskHandle_t Task1Task_Handler;
//任务函数
[[noreturn]] void task1_task(void *pvParameters);

//任务优先级
#define TASK2_TASK_PRIO		2
//任务堆栈大小
#define TASK2_STK_SIZE 		128
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
[[noreturn]] void task2_task(void *pvParameters);

_OutPut_        led(GPIOE6);
_OutPut_        led1(GPIOE5);
_USART_         U1(USART1,115200);
Software_IIC    SIIC1(GPIOB4,GPIOB5);
OLED_SSD1306    MOLED(&SIIC1,OLED_SSD1306::Queue::OWN_Queue);

SPI             spi2(SPI2);
SD_SPI          SD1(&spi2, GPIOB12);

std::string asdasd="123456";
std::string sdaasda="564";



int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);					//初始化延时函数

    U1.write((uint8_t *)asdasd.data(),5);
    MOLED.init();
    MOLED.Fill(0xff);
    delay_ms(1000);
    MOLED.Fill(0x00);

    spi2.config(GPIOB13,GPIOB14,GPIOB15);
    spi2.init(SPI2);
    uint8_t ret=SD1.init();
    while(ret)//检测不到SD卡
    {
        ret=SD1.init();
        delay_ms(500);
        MOLED.Print(0,0,"ret:%x",ret);
        delay_ms(500);
        MOLED.Print(0,0,(uint8_t*)"       ");
    }
    MOLED.Print(0,0,(uint8_t*)"init OK!");

//    MOLED.Print(0,4,asdasd);//关闭文件
//    SD1.write(0,(uint8_t *)"1",1);
//    uint8_t getas[1024];
//    SD1.read(0,getas,1);
//
//    MOLED.Print(0,6,getas);//关闭文件
    FIL fp;//文件指针
    FIL fp2;//文件指针
    UINT plen;											//文件长度临时数据
    DIR dr;
    FRESULT rrt;
    char text_data[10]="123456";
    char gext_data[10]="qwerty";

    MOLED.Print(0,2,"%s",
    Storage_Link::exfuns_init((char*)"0:",&SD1)?"true":"false");

//    f_mkfs("0:",1,4096);
    f_mkdir("0:/asd");

    rrt=f_open(&fp,"0:/asd/23.txt",FA_WRITE|FA_OPEN_ALWAYS);
    if(rrt==FR_OK)
    {
        f_lseek(&fp,fp.fsize);																		//移动光标到文件尾
        f_write(&fp,text_data,strlen((char*)text_data),&plen);		//写入数据
        f_close(&fp);
        MOLED.Print(0,4,(char*)"true");//关闭文件
    }
    else
     MOLED.Print(0,4,(char*)"false");//关闭文件

    rrt=f_open(&fp2,"0:/asd/23.txt",FA_READ);
    rrt=f_read(&fp2,gext_data,strlen((char*)text_data),&plen);
    f_close(&fp2);
    MOLED.Print(0,6,gext_data);//关闭文件


    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,          //任务函数
                (const char*    )"start_task",           //任务名称
                (uint16_t       )START_STK_SIZE,     //任务堆栈大小
                (void*          )nullptr,            //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
[[noreturn]] void task1_task(void *pvParameters)//alignas(8)
{
    uint8_t sec_t=0;
    while(true)
    {
        vTaskDelay(250/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度
        led.change();
    }
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{

    while(true)
    {
        vTaskDelay(200/portTICK_RATE_MS );
        U1<<U1.read_data();
        MOLED.Queue_star();
        //MOLED.Print(0,0,"%04d",task2_num);
        led1.change();
        MOLED.Queue_end();
    }
}


