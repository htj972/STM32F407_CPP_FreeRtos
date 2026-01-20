#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "malloc.h"
#include "Kstring.h"
#include "Gateway.h"
#include "Communication.h"
#include "WDG.h"
#include "lwip_comm/lwip_comm.h"
#include "fertilizer.h"
#include "cJSON.h"
#include "HC595.h"
#include "HC165.h"
#include "SPI.h"
#include "W25QXX.h"
#include "Storage_Link.h"


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		64
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LOGIC_TASK_PRIO		3
//任务堆栈大小
#define LOGIC_STK_SIZE 		(128*10)
//任务句柄
TaskHandle_t LOGICTask_Handler;
//任务函数
[[noreturn]] void LOGIC_task(void *pvParameters);

//任务优先级
#define RS485_TASK_PRIO		3
//任务堆栈大小
#define RS485_STK_SIZE 		(128)
//任务句柄
TaskHandle_t RS485Task_Handler;
//任务函数
[[noreturn]] void RS485_task(void *pvParameters);
uint16_t tims=0;
//运行指示灯
class T_led_:public _OutPut_,public Call_Back,public Timer{
private:
    bool mode=false;
public:
    T_led_(GPIO_Pin param,TIM_TypeDef *TIMx, uint16_t frq) {
        _OutPut_::init(param,LOW);
        Timer::init(TIMx,10000/frq,8400,true);
        this->Timer::upload_extern_fun(this);
    }
    void set_mode(bool modex){
        this->mode=modex;
    }
    void Callback(int  ,char** ) override{
        if(mode)this->change();
        Feed_Dog();
        tims++;
    };
}led(GPIOD10,TIM6,2);//运行指示灯定时器

_OutPut_ error_led (GPIOD11);//运行指示灯

_OutPut_ TrmLED[6]={
    _OutPut_(GPIOE2,HIGH),
    _OutPut_(GPIOE6,HIGH),
    _OutPut_(GPIOE4,HIGH),
    _OutPut_(GPIOE3,HIGH),
    _OutPut_(GPIOC13,HIGH),
    _OutPut_(GPIOE5,HIGH)
};

_USART_ Debug(USART1,115200);

HC595 OUT_driver(GPIOC6,GPIOD14,GPIOD13,GPIOD15,GPIOD12,2);//74HC595驱动
HC165 IN_driver(GPIOE12,GPIOE13,GPIOE14,GPIOE15,2);//74HC165驱动
Software_IIC IIC(GPIOE0,GPIOE1);//硬件IIC1
FM24Cxx eeprom(&IIC,FM24Cxx::AT24C16);//FM24C16驱动
SPI SPI1_driver;//硬件SPI1
W25QXX Flash(&SPI1_driver,GPIOD7);//W25Q64驱动
Storage_Link flash_fatfs(&Flash);
SDIO_CARD SD;
Storage_Link SD_fatfs(&SD);

//class lwip_:public Timer,public Call_Back{
//public:
//    lwip_(TIM_TypeDef *TIMx, uint16_t frq) {
//        Timer::init(TIMx,10000/frq,8400,true);
//        this->Timer::upload_extern_fun(this);
//    }
//    void Callback(int  ,char** ) override {
//        lwip_setup();
//    }
//}lwipw(TIM5,100);//运行指示灯定时器



#define MD_Debug 0

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    //WDG_Init();
    delay_init(168);	//初始化延时函数
    delay_ms(1000);//延时1s
    SPI1_driver.config(GPIOE3,GPIOE4,GPIOE5);
    SPI1_driver.init();

    OUT_driver.init();
    IN_driver.init();
    IN_driver.set_shift(0,(const char[]){3,2,1,0,4,5,6,7});
    IN_driver.set_shift(1,(const char[]){7,6,5,4,0,1,2,3});
    OUT_driver.set_shift(1,(const char[]){7,6,5,4,3,2,1,0});
    my_mem_init(SRAMIN);		//初始化内部内存池
    my_mem_init(SRAMCCM);		//初始化内部内存池
    eeprom.write(0,(uint8_t*)"KOKIRIKA",8);
    uint8_t Kstring_buf[10];
    eeprom.read(0,Kstring_buf,8);
    Debug.print("EEPROM read:%s\r\n",Kstring_buf);
    //lwip_dhcp_process_handle();
#if MD_Debug
    {
        MB<<"lwIP Initing...\r\n";
        while(lwip_comm_init()!=0)
        {
            MB<<"lwIP Init failed!\r\n";
            delay_ms(1200);
            MB<<"Retrying...\r\n";
        }
        MB<<"lwIP Init Successed\r\n";
        //等待DHCP获取
        MB<<"DHCP IP configing...\r\n";
        while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
        {
            lwip_periodic_handle();
        }
        uint8_t speed;
        MB<<DHCP_str[0]<<DHCP_str[1]<<DHCP_str[2]<<DHCP_str[3]<<DHCP_str[4];
        speed=LAN8720_Get_Speed();//得到网速
        if(speed&1<<1)MB<<"Ethernet Speed:100M\r\n";
        else MB<<"Ethernet Speed:10M\r\n";
    }
#else
//    while(lwip_comm_init()!=0)
//    {
//        delay_ms(1200);
//    }
//
//    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
//    {
//        lwip_periodic_handle();
//    }
#endif
    led.set_mode(true);

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
    xTaskCreate((TaskFunction_t )LOGIC_task,
                (const char*    )"LOGIC_task",
                (uint16_t       )LOGIC_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )LOGIC_TASK_PRIO,
                (TaskHandle_t*  )&LOGICTask_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )RS485_task,
                (const char*    )"RS485_task",
                (uint16_t       )RS485_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )RS485_TASK_PRIO,
                (TaskHandle_t*  )&RS485Task_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}
QueueHandle_t xMailbox;
//fertilizer FWmode;
//task1任务函数
[[noreturn]] void LOGIC_task(void *pvParameters)//alignas(8)
{
    uint8_t ii=0;
    while(true)
    {
        delay_ms(800);
        error_led.change();
        TrmLED[ii++].change();
        if(ii>=6)ii=0;
    }
}

//task2任务函数
[[noreturn]] void RS485_task(void *pvParameters)
{
    uint8_t ptemp=0;
    uint8_t Idata[2];
    while(true) {
        delay_ms(50);
        IN_driver.upset();
        IN_driver.Get_input(Idata);
        //Debug.print("IN data: %02X %02X\r\n",Idata[0],Idata[1]);
        if(Idata[1]!=ptemp) {
            ptemp = Idata[1];
            Debug.print("IN data high byte: %02X\r\n", Idata[1]);
        }
    }
}


