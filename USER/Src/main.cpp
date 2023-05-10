#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "Communication.h"
#include "DW_DIS.h"
#include "TEOM.h"
#include "W25QXX.h"
#include "Storage_Link.h"
#include "USB_MSC.h"


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		128
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define TEMP_TASK_PRIO		4
//任务堆栈大小
#define TEMP_STK_SIZE 		256
//任务句柄
TaskHandle_t TEMPTask_Handler;
//任务函数
[[noreturn]] void TEMP_task(void *pvParameters);

//任务优先级
#define COM_TASK_PRIO		3
//任务堆栈大小
#define COM_STK_SIZE 		1024
//任务句柄
TaskHandle_t COMTask_Handler;
//任务函数
[[noreturn]] void COM_task(void *pvParameters);

//任务优先级
#define DIS_TASK_PRIO		3
//任务堆栈大小
#define DIS_STK_SIZE 		1024
//任务句柄
TaskHandle_t DISTask_Handler;
//任务函数
[[noreturn]] void DIS_task(void *pvParameters);


_OutPut_    led(GPIOE6);

class T_led_:public _OutPut_,public Call_Back,public Timer{
public:
    T_led_(GPIO_Pin param,TIM_TypeDef *TIMx, uint16_t frq) {
        _OutPut_::init(param,LOW);
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override{
        this->change();
    };
}led2(GPIOE5,TIM6,2);

//SPI总线
SPI SPI_BUS(SPI1);
//TEOM 驱动类
TEOM_Machine teom(GPIOB0,GPIOB1,GPIOD14,TIM4,2);
//TEOM 温度控制类
TEOM_TEMP TEMP(&SPI_BUS,GPIOC0,GPIOC1,GPIOC2,GPIOC3,GPIOA0,TIM1,1000);
//通信软件类
Communication m_modebus(USART3,GPIOE15,TIM5,100);
//迪文显示类
DW_DIS MDW(USART6,TIM7,10);
//差压显示
pressure_dif pressure(GPIOD4,GPIOD5);

//W25QXX flash(&SPI_BUS,GPIOC4);
//Storage_Link flash_fatfs(&flash);


USB_MSC USB ;
Storage_Link USB_fatfs(&USB);

/*修改APP启动
 * /ZR-APP/ZR-055V.bin
 * system_stm32f4xx.c
 * 原始  #define VECT_TAB_OFFSET  0x00
 * 修改  #define VECT_TAB_OFFSET  0x8000
 *
 * STM32F407VGTx_FLASH.ld
 *原始
 * MEMORY
    {
    RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
    CCMRAM (xrw)      : ORIGIN = 0x10000000, LENGTH = 64K
    FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 1024K
    }
 * 修改
 * MEMORY
    {
    RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
    CCMRAM (xrw)      : ORIGIN = 0x10000000, LENGTH = 64K
    FLASH (rx)      : ORIGIN = 0x8008000, LENGTH = 992K
    }
 *
*/

int main()
{
//#define FLASH_ADDR_OFFSET		0x00000
//#ifdef VECT_TAB_RAM
//    NVIC_SetVectorTable(NVIC_VectTab_RAM, FLASH_ADDR_OFFSET);
//#else
//    NVIC_SetVectorTable(NVIC_VectTab_FLASH, FLASH_ADDR_OFFSET);
//#endif
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数
    delay_ms(1000);
    TEMP.initial();
    teom.inital();
    teom.Link_PRE(&pressure);
    USB.init();
//    USB.wait_Linked();
    USB_fatfs.init();
//    flash_fatfs.init();

//    uint16_t ii=0;
//    char dasda[50];
//
//    if(USB.Get_device_sata()==USB_MSC::SATA::Linked) {
//        again:
//        sprintf(dasda,"data/qwe%d.txt",ii);
//        ii++;
//        if (f_open(&USB_fatfs.fp,USB_fatfs.setdir(dasda),FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {
//            f_lseek(&USB_fatfs.fp,USB_fatfs.fp.fsize);                                                                        //??????±ê????????
//            f_write(&USB_fatfs.fp, dasda, strlen(dasda), &USB_fatfs.plen);
//            f_close(&USB_fatfs.fp);
//        }
//        if(ii<10)goto again;
//    }

//    f_mkfs(flash_fatfs.get_name(),1,4096);
//    MDW.Link_Flash(&USB_fatfs);
    MDW.Link_TEOM(&teom);
    MDW.Link_Temp(&TEMP);
    MDW.Link_Com(&m_modebus);
    MDW.initial();



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
    //创建温度任务
    xTaskCreate((TaskFunction_t )TEMP_task,
                (const char*    )"TEMP_task",
                (uint16_t       )TEMP_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TEMP_TASK_PRIO,
                (TaskHandle_t*  )&TEMPTask_Handler);
    //创建COM任务
    xTaskCreate((TaskFunction_t )COM_task,
                (const char*    )"COM_task",
                (uint16_t       )COM_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )COM_TASK_PRIO,
                (TaskHandle_t*  )&COMTask_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )DIS_task,
                (const char*    )"DIS_task",
                (uint16_t       )DIS_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DIS_TASK_PRIO,
                (TaskHandle_t*  )&DISTask_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//U读取任务
[[noreturn]] void TEMP_task(void *pvParameters)
{
    while(true) {
        vTaskDelay(50/portTICK_RATE_MS );
//        delay_ms(10);
        USB.Upset();
        if(USB.Get_device_sata()==USB_MSC::SATA::Linked) {
            led.set(ON);
        } else{
            led.set(OFF);
        }
    }
}
//串通信任务
[[noreturn]] void COM_task(void *pvParameters)
{
    while(true) {
        vTaskDelay(500 / portTICK_RATE_MS);
        m_modebus.data_sync();
        pressure.read();
    }
}
//迪文屏任务
[[noreturn]] void DIS_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );
        MDW.key_handle();
        MDW.Dis_handle();
    }
}


