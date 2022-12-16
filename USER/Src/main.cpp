#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "SPI.h"
#include "MAX31865.h"
#include "RS485.h"
#include "modbus.h"
#include "IIC.h"
#include "OLED_SSD1306.h"



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
#define TASK2_STK_SIZE 		512
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
[[noreturn]] void task2_task(void *pvParameters);

SPI_S       spi1(GPIOE5,GPIOE3,GPIOE6);
MAX31865    temp1(&spi1,GPIOE2);
MAX31865    temp2(&spi1,GPIOE4);
//RS485       modus_DE(USART2,GPIOD4,9600);
//modbus      SLAVE_BUS(&modus_DE);

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
}led2(GPIOE1,TIM6,10);

class OLED:private Software_IIC,public OLED_SSD1306{
public:
    OLED(uint8_t Pin_Scl, uint8_t Pin_Sda){
        Software_IIC::init(Pin_Scl, Pin_Sda);
        OLED_SSD1306::config(this);
    }
}MLED(GPIOB12,GPIOB13);

class SLAVE_BUS:private RS485,private Timer,public modbus{
public:
    SLAVE_BUS(USART_TypeDef* USARTx,uint8_t Pinx,TIM_TypeDef *TIMx, uint16_t frq){
        RS485::init(USARTx,Pinx);
        Timer::init(TIMx,10000/frq,8400,true);
        modbus::init(this);
    }
    uint16_t data_BUS[20]{};
    string asd;
    void initial(){
        RS485::config(GPIOD5,GPIOD6);
        modbus::Link_UART_CALLback();
        modbus::Link_TIMER_CALLback(this);
        modbus::config(this->data_BUS,0,10);
        for(uint8_t ii=0;ii<20;ii++){
            data_BUS[ii]=ii;
        }
    }
    void data_ADD_output(){
        MLED.Print(0,0,"%02x %02x %02x %02x %02x %02x %02x %02x"\
        ,data_BUS[0],data_BUS[1],data_BUS[2],data_BUS[3]\
        ,data_BUS[4],data_BUS[5],data_BUS[6],data_BUS[7]);
    }

    void dssfsfa(){
        static uint8_t sdatime=0;
        uint16_t len_t=this->modbus_receive_data.length();
        if(this->reveive_len!=len_t){
            this->reveive_len = len_t;
            sdatime=0;
        }
        else if(len_t!=0)
        {
            sdatime++;
            if(sdatime==10) {
                sdatime=0;
                asd = modbus_receive_data;
                modbus_receive_data.clear();
                MLED.Print(0, 0, "%02x %02x %02x %02x"\
                , asd[0], asd[1], asd[2], asd[3]);
                MLED.Print(0, 2, "%02x %02x %02x %02x"\
                , asd[4], asd[5], asd[6], asd[7]);
            }
        }
    }
}m_modebus(USART2,GPIOD4,TIM7,100);

string wreer="123456789";

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);					//初始化延时函数
    MLED.OLED_SSD1306::init();
    MLED.CLS();
    wreer+='a';
    MLED.Print(0, 0,wreer);
    m_modebus.initial();
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
        vTaskDelay(1000/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度

    }
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );

    }
}


