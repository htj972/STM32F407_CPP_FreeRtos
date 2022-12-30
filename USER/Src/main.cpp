#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "IIC.h"
#include "OLED_SSD1306.h"
#include "Communication.h"
#include "pretreatment.h"
#include "MS5805.h"
#include "FM24Cxx.h"
#include "Fower_Ctrl.h"


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

//IIC总线
Software_IIC IIC_BUS(GPIOB8,GPIOB9);
//大气压传感器
MS5805  Atmospheric_P(&IIC_BUS);
//EEPROM 数据存储
FM24Cxx power_data(&IIC_BUS);

//SPI总线
SPI_S SPI_BUS(GPIOE5,GPIOE3,GPIOE6);
//预处理控温软件类
pretreatment stovectrl(&SPI_BUS,GPIOE4,TIM4,1000,2,GPIOC10,GPIOC11);
//大气温度传感器
MAX31865 Atmospheric_T(&SPI_BUS,GPIOE2);
//通信软件类
Communication m_modebus(USART2,GPIOD4,TIM7,100,GPIOD5,GPIOD6);




Differential_pressure tes1(GPIOD1,GPIOD0,100000,-100000,TIM9,100);
Differential_pressure tes2(GPIOD3,GPIOD2,4000,-500,TIM10,100);

//运行指示灯
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
    OLED(GPIO_Pin Pin_Scl, GPIO_Pin Pin_Sda){
        Software_IIC::init(Pin_Scl, Pin_Sda);
        OLED_SSD1306::config(this);
    }
    void initial(){
        this->OLED_SSD1306::init();
        this->CLS();
    }
}MLED(GPIOB12,GPIOB13);


int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数
    MLED.initial();             //OLED 初始化

    Atmospheric_T.init();       //
    Atmospheric_T.config(MAX31865::PT100,3900);
    Atmospheric_P.init();       //大气压初始化
    power_data.init();          //数据存储初始化
    //读取缓存数据
    power_data.read(0,m_modebus.data_BUS.to_u8t,sizeof(m_modebus.data_BUS));
    //检验数据正确性  头次上电（版本号）
    if(m_modebus.data_BUS.to_float.version-HARD_version!=0){
        K_POWER_DATA init_data;             //新实体一个新数据 构造方法放置初始数值
        m_modebus.data_BUS=init_data;       //转移数据到使用结构体
        power_data.write(0,init_data.to_u8t,sizeof(init_data));  //写入数据
    }

    stovectrl.initial();        //炉子初始化

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
        vTaskDelay(200/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度
        MLED.Print(0,0,"V%.2lf",m_modebus.data_BUS.to_float.version);


        MLED.Print(0,2,"%.2lf",stovectrl.get_cur());

        MLED.Print(0,4,"%.2lf",Atmospheric_T.get_temp());

        MLED.Print(0,6,"%.2lf",Atmospheric_P.get_pres());
//        MLED.Print(0,6,"%.2lf",stovectrl.get_temp_cache());
//        pretreatment1.set_target(150);
//        pretreatment1.upset();

        m_modebus.data_BUS.to_float.stove_temp_r=stovectrl.get_cur();

        if(m_modebus.data_BUS.to_float.stove_work==1){
            stovectrl.turn_ON();
        }
        else
        {
            stovectrl.turn_OFF();
        }

    }
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(500);
        stovectrl.upset();
    }
}


