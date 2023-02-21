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
#include "DS18B20.h"


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		128
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LOGIC_TASK_PRIO		3
//任务堆栈大小
#define LOGIC_STK_SIZE 		512
//任务句柄
TaskHandle_t LOGICTask_Handler;
//任务函数
[[noreturn]] void LOGIC_task(void *pvParameters);

//任务优先级
#define PID_TASK_PRIO		2
//任务堆栈大小
#define PID_STK_SIZE 		256
//任务句柄
TaskHandle_t PIDTask_Handler;
//任务函数
[[noreturn]] void PID_task(void *pvParameters);

//任务优先级
#define DATA_TASK_PRIO		2
//任务堆栈大小
#define DATA_STK_SIZE 		128
//任务句柄
TaskHandle_t DATATask_Handler;
//任务函数
[[noreturn]] void DATA_task(void *pvParameters);

//任务句柄
TaskHandle_t DATATask1_Handler;
//任务函数
[[noreturn]] void DATA1_task(void *pvParameters);

//任务句柄
TaskHandle_t DATATask2_Handler;
//任务函数
[[noreturn]] void DATA2_task(void *pvParameters);

//任务句柄
TaskHandle_t DATATask3_Handler;
//任务函数
[[noreturn]] void DATA3_task(void *pvParameters);

//任务句柄
TaskHandle_t DATATask4_Handler;
//任务函数
[[noreturn]] void DATA4_task(void *pvParameters);

//任务句柄
TaskHandle_t DATATask5_Handler;
//任务函数
[[noreturn]] void DATA5_task(void *pvParameters);

//IIC总线
Software_IIC IIC_BUS(GPIOB8,GPIOB9);
//大气压传感器
MS5805  Atmospheric_P(&IIC_BUS);
//EEPROM 数据存储
FM24Cxx power_data(&IIC_BUS);

//硬件PWM
PWM_H PWM(TIM4,1000);
//SPI总线
SPI_S SPI_BUS(GPIOE5,GPIOE3,GPIOE6);
//预处理控温软件类
pretreatment stovectrl(&SPI_BUS,GPIOE4,&PWM,2,GPIOC10,GPIOC11);
//大气温度传感器
MAX31865 Atmospheric_T(&SPI_BUS,GPIOE2);
//通信软件类
Communication m_modebus(USART2,GPIOD4,TIM7,1000,GPIOD5,GPIOD6);


Differential_pressure JY(GPIOD1,GPIOD0,100000,-100000);
Differential_pressure LY(GPIOD3,GPIOD2,4000,-500);
//计温
DS18B20  JW(GPIOB6);

Fower_Ctrl LL(&LY,&JY,&Atmospheric_P,&JW,&Atmospheric_T);

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

    delay_ms(500);

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
    PWM.config(1,2);
    LL.config(&PWM,1);
    LL.FLOW_RATE=m_modebus.data_BUS.to_float.Flow_coefficient;
    LL.set_ladder(10);
    stovectrl.initial();        //炉子初始化
    JW.init();

//    PWM.set_channel1_Duty_cycle(5 );

    taskENTER_CRITICAL();           //进入临界区
    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,          //任务函数
                (const char*    )"start_task",           //任务名称
                (uint16_t       )START_STK_SIZE,     //任务堆栈大小
                (void*          )nullptr,            //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
    taskEXIT_CRITICAL();            //退出临界区
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建LOGIC任务
    xTaskCreate((TaskFunction_t )LOGIC_task,
                (const char*    )"LOGIC_task",
                (uint16_t       )LOGIC_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )LOGIC_TASK_PRIO,
                (TaskHandle_t*  )&LOGICTask_Handler);
    //创建PID任务
    xTaskCreate((TaskFunction_t )PID_task,
                (const char*    )"PID_task",
                (uint16_t       )PID_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )PID_TASK_PRIO,
                (TaskHandle_t*  )&PIDTask_Handler);

    //创建DATA任务
    xTaskCreate((TaskFunction_t )DATA_task,
                (const char*    )"DATA_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask_Handler);

    //创建DATA1任务
    xTaskCreate((TaskFunction_t )DATA1_task,
                (const char*    )"DATA1_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask1_Handler);

    //创建DATA2任务
    xTaskCreate((TaskFunction_t )DATA2_task,
                (const char*    )"DATA2_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask2_Handler);

    //创建DATA3任务
    xTaskCreate((TaskFunction_t )DATA3_task,
                (const char*    )"DATA3_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask3_Handler);

    //创建DATA4任务
    xTaskCreate((TaskFunction_t )DATA4_task,
                (const char*    )"DATA4_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask4_Handler);

    //创建DATA5任务
    xTaskCreate((TaskFunction_t )DATA5_task,
                (const char*    )"DATA5_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask5_Handler);

    vTaskDelete(StartTask_Handler);
    taskEXIT_CRITICAL();            //退出临界区
}

//LOGIC任务函数
[[noreturn]] void LOGIC_task(void *pvParameters)
{
    static bool LL_init_f= false;
    while(true)
    {
        delay_ms(200);

        MLED.OLED_SSD1306::Queue_star();
        MLED.Print(0,0,"%.2lf   %.2lf",LL.LiuYa,LL.JiYa);
        MLED.Print(0,2,"%.2lf   %.2lf",LL.JiWen,LL.DaQiWen);

        MLED.Print(0,4,"%.2lf",LL.DaQiYa);

        MLED.Print(0,6,"%.2lf",LL.cur);
        MLED.OLED_SSD1306::Queue_end();


        m_modebus.data_BUS.to_float.stove_temp_r=stovectrl.get_cur();
        m_modebus.data_BUS.to_float.Flow_value_r=LL.cur;
        m_modebus.data_BUS.to_float.air_temp=Atmospheric_T.get_temp_cache();

        m_modebus.data_BUS.to_float.Flow_coefficient=LL.FLOW_RATE;


        if(m_modebus.data_BUS.to_float.stove_work==1){
            stovectrl.turn_ON();
        }
        else
        {
            stovectrl.turn_OFF();
        }

        if(m_modebus.data_BUS.to_float.Flow_work==1){
            LL.set_target(m_modebus.data_BUS.to_float.Flow_value_s);
            LL.TURN_ON();
            if(!LL_init_f)
            {
                LL_init_f= true;
                JY.calculate_pres_offset(0);
                LY.calculate_pres_offset(0);
            }
            if(m_modebus.data_BUS.to_float.Flow_value_w>0){
                LL.FLOW_RATE_change(m_modebus.data_BUS.to_float.Flow_value_w);
                m_modebus.data_BUS.to_float.Flow_value_w=0;
                m_modebus.data_BUS.to_float.Flow_coefficient=LL.FLOW_RATE;
                power_data.write(0,m_modebus.data_BUS.to_u8t,sizeof(m_modebus.data_BUS));  //写入数据
            }
        }

        else
        {
            LL.TURN_OFF();
        }

    }
}

//PID任务函数
[[noreturn]] void PID_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(500);
        stovectrl.upset();
        LL.upset();
    }
}

//DATA任务函数
[[noreturn]] void DATA_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(20);
        LL.calculation_hole_flow();
    }
}

//DATA1任务函数
[[noreturn]] void DATA1_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.LiuYa_data_upset();
    }
}

//DATA2任务函数
[[noreturn]] void DATA2_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.JiYa_data_upset();
    }
}

//DATA3任务函数
[[noreturn]] void DATA3_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.DaQiYa_data_upset();
    }
}

//DATA4任务函数
[[noreturn]] void DATA4_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.JiWen_data_upset();
    }
}

//DATA5任务函数
[[noreturn]] void DATA5_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.DaQiWen_data_upset();
    }
}
