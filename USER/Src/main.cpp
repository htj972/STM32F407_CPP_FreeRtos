#include <regex>
#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "malloc.h"
#include "EC20.h"
#include "ThingsBoard.h"
#include "Kstring.h"
#include "Gateway.h"
#include "Communication.h"
#include "WDG.h"


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		64
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LOGIC_TASK_PRIO		2
//任务堆栈大小
#define LOGIC_STK_SIZE 		128
//任务句柄
TaskHandle_t LOGICTask_Handler;
//任务函数
[[noreturn]] void LOGIC_task(void *pvParameters);

//任务优先级
#define EC20_TASK_PRIO		3
//任务堆栈大小
#define EC20_STK_SIZE 		(128*10)
//任务句柄
TaskHandle_t EC20Task_Handler;
//任务函数
[[noreturn]] void EC20_task(void *pvParameters);
uint16_t tims=0;
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
        Feed_Dog();
        tims++;
    };
}led(GPIOE5,TIM6,2);//运行指示灯定时器

_OutPut_ run (GPIOE6);//运行指示灯
//_OutPut_ Beep(GPIOE4,LOW);          //蜂鸣器
_OutPut_ OUT(GPIOC10,HIGH);            //输出
_OutPut_ RST(GPIOA11);//EC20复位引脚

Timer tIM_EC(TIM5,100,8400,true);

_USART_ DEBUG(USART2);             //调试串口
//RS485   com(USART3,GPIOD10);

EC20    ET(USART1);
ThingsBoard TB(&DEBUG,&ET);

Communication COM(USART3,GPIOB15,TIM7,100);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    WDG_Init();
    delay_init(168);	//初始化延时函数
    delay_ms(1000);//延时1s

    my_mem_init(SRAMIN);		//初始化内部内存池
    my_mem_init(SRAMCCM);		//初始化内部内存池

//    Beep.flicker(100,200,2);

    ET.Link_RST_Pin(&RST);
    ET.reset();
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
    xTaskCreate((TaskFunction_t )EC20_task,
                (const char*    )"EC20_task",
                (uint16_t       )EC20_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )EC20_TASK_PRIO,
                (TaskHandle_t*  )&EC20Task_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
[[noreturn]] void LOGIC_task(void *pvParameters)//alignas(8)
{
    Kstring da;
    while(true)
    {
        vTaskDelay(1000/portTICK_RATE_MS );
        run.change();
        COM.data_sync();
    }
}

//task2任务函数
[[noreturn]] void EC20_task(void *pvParameters)
{
    while(true) {
        while (!ET.getrdy()) {
            delay_ms(1000);
            DEBUG << ".";
        }
        while (true) {
            DEBUG << "EC20:" << (ET.init() ? "OK" : "error") << "\r\n";
            ET.setdebug(&DEBUG);
            ET.setGPS();
            ET.Register(EC20::APN::APN_CMNET);
            ET.Link_TIMER_CALLback(&tIM_EC);

            TB.Connect(222, 74, 215, 220, 31883);
            TB.config("XCGY", "XC001", "XC001");
            TB.SubscribeTopic();
            while (true) {
                vTaskDelay(100 / portTICK_RATE_MS);
                if (!ET.get_Link_Status())
                    break;
                TB.Getdatacheck();
                if(tims>=10*2){
                    tims=0;
                    string sensor_str=COM.data_to_json();
                    if(ET.getGPS()){
                        //删除后面的“}”
                        sensor_str.pop_back();
                        sensor_str.append(R"(,"date":")");
                        sensor_str.append(ET.GPS_D.date);
                        sensor_str.append(R"(","UTC":")");
                        sensor_str.append(ET.GPS_D.UTC);
                        sensor_str.append(R"(","longitude":")");
                        sensor_str.append(ET.GPS_D.longitude);
                        sensor_str.append(R"(","latitude":")");
                        sensor_str.append(ET.GPS_D.latitude);
                        sensor_str.append(R"("})");
                    }
//                    DEBUG<<sensor_str<<"\r\n";
                    TB.PublishData(sensor_str);

                }
            }
        }
    }
}


