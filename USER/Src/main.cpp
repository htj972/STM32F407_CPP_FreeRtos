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
#include "tcp_client/TCP_Client_Class.h"
#include "MQTT/MQTT.h"
#include "Kstring.h"
#include "picosha2.h"
#include "ThingsBoard.h"
#include "cJSON.h"
#include "WDG.h"

/*修改APP启动
 * /Bin/xxx.bin
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

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		64
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define DATA_TASK_PRIO		2
//任务堆栈大小
#define DATA_STK_SIZE 		128
//任务句柄
TaskHandle_t DATATask_Handler;
//任务函数
[[noreturn]] void DATA_task(void *pvParameters);

//任务优先级
#define INTER_TASK_PRIO		2
//任务堆栈大小
#define INTER_STK_SIZE 		(512*3)
//任务句柄
TaskHandle_t INTERTask_Handler;
//任务函数
[[noreturn]] void INTER_task(void *pvParameters);

uint8_t times=0;
class lwip_:public Timer,public Call_Back{
public:
    lwip_(TIM_TypeDef *TIMx, uint16_t frq) {
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override {
        lwip_setup();
    }
}lwipw(TIM6,100);//运行指示灯定时器

class T_led_:public _OutPut_,public Call_Back,public Timer{
public:
    T_led_(GPIO_Pin param,TIM_TypeDef *TIMx, uint16_t frq) {
        _OutPut_::init(param,LOW);
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override{
        times++;
        Feed_Dog();
        this->change();
    }
}run(GPIOE5,TIM5,2);//运行指示灯定时器

_OutPut_ led (GPIOE6),BEEP (GPIOE4,HIGH);//运行指示灯
_OutPut_ OUT1(GPIOE0,HIGH),OUT2(GPIOE1,HIGH);            //输出

//Timer_queue tIMS(TIM7,50000);                 //定时器队列

_USART_ DEBUG(USART1);                          //调试串口
//_USART_ Lora(USART6);                           //Lora串口

Communication MB(USART3,GPIOB15,TIM7,100);//modbus通信


TCP_Client_Class  tcp_sbc;

TCP_Client_Class  tcp_mq;//TCP通信
MQTT        mqtt_demo(&tcp_mq);
ThingsBoard tb(&DEBUG,&mqtt_demo);//

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    WDG_Init();
    delay_init(168);	//初始化延时函数

    my_mem_init(SRAMIN);		//初始化内部内存池
    my_mem_init(SRAMCCM);		//初始化内部内存池

    BEEP.flicker(100,250,2);//蜂鸣器提示

    delay_ms(1000);//延时1s
    MB.RS485::config(GPIOD8,GPIOD9);//配置RS485 GPIO引脚

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
    xTaskCreate((TaskFunction_t )DATA_task,
                (const char*    )"DATA_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )INTER_task,
                (const char*    )"INTER_task",
                (uint16_t       )INTER_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )INTER_TASK_PRIO,
                (TaskHandle_t*  )&INTERTask_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


//task1任务函数
[[noreturn]] void DATA_task(void *pvParameters)//alignas(8)
{
    string str;
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度
        run.change();       //运行指示灯
        //modbus通信
        MB.data_sync();
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

//上位机TCP bug 需要在发送数据前加上4字节的数据长度
void tcp_check_send(TCP_Client_Class *tcp,const string& str){
    Kstring check;
    union {
        int len;
        char data[4];
    }da{};
    da.len=(int)str.length();
    check<<da.data[3];
    check<<da.data[2];
    check<<da.data[1];
    check<<da.data[0];
    check<<str;
    *tcp<<check;
}

//task2任务函数
[[noreturn]] void INTER_task(void *pvParameters)
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
        while(!ThingsBoard::PHY_islink());
        if(!tcp_sbc.islink()){
            tcp_sbc.connect(10,40,12,40,50013);
        }
        if(!tb.intel_islink()) {
            tb.Connect(222, 74, 215, 220, 31883);
            tb.config("1234567", "1234567", "1234567");
            DEBUG<<"订阅结果"<<tb.SubscribeTopic()<<"\r\n";
        }
        DEBUG<<"linking...\r\n";
        if(tb.intel_islink()&&tcp_sbc.islink()&&ThingsBoard::PHY_islink())
        while (true){
            delay_ms(100);

            if((!tb.intel_islink())||(!tcp_sbc.islink())||(!ThingsBoard::PHY_islink())){
                tb.inter_unlink();
                tcp_sbc.close();
                break;
            }

            tb.Getdatacheck();
//            tb.GetVersion();

            if(tb.TCP_data_check(&tcp_sbc))times=20;

            if(times>20){
                times=0;
                string sensor_str=MB.data_to_json();
                DEBUG<<"sensor:"<<sensor_str<<"\r\n";
                if(tcp_sbc.islink()){
                    Kstring str = ThingsBoard::TCP_data_process(sensor_str);
                    tcp_check_send(&tcp_sbc, str.GBK_to_utf8());
                }
                if(tb.intel_islink()) {
                    Kstring buf = sensor_str;
                    tb.PublishData(buf.GBK_to_utf8());
                }
            }
        }
    }
}


