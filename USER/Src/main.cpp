#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "RS485.h"
#include "malloc.h"
#include "lwip_comm/lwip_comm.h"
#include "Communication.h"
#include "udp/UDP_Class.h"
#include "cJSON.h"
#include "tcp_client/TCP_Client_Class.h"
#include "MQTT/MQTT.h"
#include "Kstring.h"
#include "picosha2.h"
#include "ThingsBoard.h"



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
#define TASK2_STK_SIZE 		512*2
//任务句柄
TaskHandle_t Task2Task_Handler;
//任务函数
[[noreturn]] void task2_task(void *pvParameters);

extern u32 lwip_localtime;		//lwip本地时间计数器,单位:ms
//运行指示灯
class T_led_:public _OutPut_,public Call_Back,public Timer{
public:
    T_led_(GPIO_Pin param,TIM_TypeDef *TIMx, uint16_t frq) {
        _OutPut_::init(param,LOW);
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override{
//        this->change();
        lwip_setup();
    };
}led(GPIOE5,TIM6,1000);//运行指示灯定时器

_OutPut_ run (GPIOE6),BEEP (GPIOE4,HIGH);//运行指示灯
_OutPut_ OUT1(GPIOE0),OUT2(GPIOE1);            //输出

//Timer_queue tIMS(TIM7,50000);                 //定时器队列

_USART_ DEBUG(USART1);                          //调试串口
_USART_ Lora(USART6);                           //Lora串口

Communication SUN(USART3,GPIOB15,TIM7,100);//modbus通信

TCP_Client_Class  tcp_mq;//TCP通信
MQTT    mqtt_demo(&tcp_mq);
ThingsBoard tb(&DEBUG,&mqtt_demo);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数

    my_mem_init(SRAMIN);		//初始化内部内存池

//    BEEP.flicker(100,250,2);//蜂鸣器提示

    delay_ms(1000);//延时1s
    SUN.RS485::config(GPIOD8,GPIOD9);//配置RS485 GPIO引脚

//    Kstring asdsad="风速";
////    asdsad.append("风速");
//    string ret=asdsad.GBK_to_utf8();
//    //16进制打印RET
//    for (char i : ret) {
//        DEBUG.print("0x%02X ",i);
//    }

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
    string str;
    while(true)
    {
        vTaskDelay(500/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度
        run.change();       //运行指示灯
        OUT1.change();      //输出状态反转
        OUT2.change();      //输出状态反转
        if(ThingsBoard::PHY_islink())
        {
            led.set(ON);
        }
        else
        {
            led.set(OFF);
        }
    }
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{

//    std::string src_str = "Hello, World!";
//    std::string hash_hex_str;
//    picosha2::hash256_hex_string(src_str, hash_hex_str);
//    DEBUG << "SHA256(" << src_str << ") = " << hash_hex_str << "\r\n";
//
//
//    std::string src_str1 = "Hello,";
//    std::string src_str2 = " World!";
//    std::vector<unsigned char> hash(picosha2::k_digest_size);
//
//    picosha2::hash256_one_by_one hasher;
//    hasher.process(src_str1.begin(), src_str1.end());
//    hasher.process(src_str2.begin(), src_str2.end());
//    hasher.finish();
//    hasher.get_hash_bytes(hash.begin(), hash.end());
//
//    std::string hash_hex_str2 = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
//
//    DEBUG << "SHA256(Hello, World!) = " << hash_hex_str2 << "\r\n";

    tb.intel_link();

    while(true)
    {
//        string buf;
        uint8_t num=5;
        uint8_t times=0;
        while(!ThingsBoard::PHY_islink());
        tb.Connect(222,74,215,220,31883);
        tb.config("daocaoren","daocaoren","daocaoren");
        DEBUG<<"订阅结果"<<tb.SubscribeTopic()<<"\r\n";

        DEBUG<<"linking...\r\n";
        mqtt_demo.Clear();
        tb.updata_step=3;
        while (true){
//            delay_ms(100);
//            times++;
//            if(times>100){
//                times=0;
//                tb.PublishData("风速",num++);
//            }


            tb.Getdatacheck();
            tb.GetVersion();



            if(!tb.intel_islink()) {
                tb.inter_unlink();
                break;
            }
            if(!ThingsBoard::PHY_islink()){
                break;
            }
        }

    }
}


