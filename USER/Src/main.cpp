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
#include "cJSON.h"
#include "tcp_client/TCP_Client_Class.h"
#include "MQTT/MQTT.h"
#include "Kstring.h"
#include "picosha2.h"
#include "ThingsBoard.h"



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
_OutPut_ OUT1(GPIOE0,HIGH),OUT2(GPIOE1,HIGH);            //输出

//Timer_queue tIMS(TIM7,50000);                 //定时器队列

_USART_ DEBUG(USART1);                          //调试串口
//_USART_ Lora(USART6);                           //Lora串口

Communication MB(USART3,GPIOB15,TIM7,100);//modbus通信

TCP_Client_Class  tcp_mq;//TCP通信
MQTT    mqtt_demo(&tcp_mq);
ThingsBoard tb(&DEBUG,&mqtt_demo);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
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
//        OUT1.change();      //输出状态反转
//        OUT2.change();      //输出状态反转
        {
            MB.set_id(0x0a);//五合一传感器
            if(MB.modbus_03_send(0,5)==modbus::result::modbus_success)
            {
                uint16_t *data=MB.data_BUS.to_u16;
                if(*data<0x8000){
                    MB.env.temp=((float)*data/10.0f);
                } else{
                    MB.env.temp=((float)(0xffff-*data+1))/10.0f;
                }
                data++;
                MB.env.humi=((float)*data/10.0f);
                data++;
                MB.env.press=((float)*data/10.0f);
                data++;
                MB.env.co2=((float)*data);
                data++;
                MB.env.light=((float)*data*10);
            }
            MB.set_id(0x0b);//PM传感器
            if(MB.modbus_03_send(0,4)==modbus::result::modbus_success)
            {
                uint16_t *data=MB.data_BUS.to_u16;
                MB.env.pm1=((float)*data);
                data++;
                MB.env.pm10=((float)*data);
                data++;
                MB.env.pm25=((float)*data);
                data++;
                MB.env.pm30=((float)*data);
            }
            MB.set_id(0x0c);//风向传感器
            if(MB.modbus_03_send(0,1)==modbus::result::modbus_success)
            {
                uint16_t *data=MB.data_BUS.to_u16;
                MB.env.wind_dir=((float)*data);
            }
            MB.set_id(0x0d);//风速传感器
            if(MB.modbus_03_send(0,1)==modbus::result::modbus_success)
            {
                uint16_t *data=MB.data_BUS.to_u16;
                MB.env.wind_speed=((float)*data/100.0f);
            }
            MB.set_id(0x0e);//总辐射传感器
            if(MB.modbus_03_send(0,1)==modbus::result::modbus_success)
            {
                uint16_t *data=MB.data_BUS.to_u16;
                MB.env.solar_rad=((float)*data);
            }
            MB.set_id(0x0f);//土壤传感器
            if(MB.modbus_03_send(0,4)==modbus::result::modbus_success)
            {
                uint16_t *data=MB.data_BUS.to_u16;
                if(*data<0x8000){
                    MB.env.soil_temp=((float)*data/10.0f);
                } else{
                    MB.env.soil_temp=((float)(0xffff-*data+1))/10.0f;
                }
                data++;
                MB.env.soil_humi=((float)*data/10.0f);
                data++;
                MB.env.soil_ec=((float)*data/100.0f);
                data++;
                MB.env.soil_salt=((float)*data);
            }
        }
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
        uint8_t times=0;
        while(!ThingsBoard::PHY_islink());
//        tb.Connect(222,74,215,220,31883);
        tb.Connect(222,74,215,220,31883);
        tb.config("daocaoren","daocaoren","daocaoren");
        DEBUG<<"订阅结果"<<tb.SubscribeTopic()<<"\r\n";

        DEBUG<<"linking...\r\n";
        mqtt_demo.Clear();
        tb.updata_step=3;
        while (true){
            delay_ms(100);
            if(!tb.intel_islink()) {
                tb.inter_unlink();
                break;
            }
            if(!ThingsBoard::PHY_islink()){
                break;
            }
            tb.Getdatacheck();
//            tb.GetVersion();
            times++;
            if(times>100){
                times=0;
//                cJSON *root = cJSON_CreateObject();
                Kstring jsdata;
                jsdata<<"{";
                jsdata<<"\""<<Kstring::GBK_to_utf8("温度")<<"\""<<":"<<MB.env.temp<<",";
//                jsdata<<"\""<<Kstring::GBK_to_utf8("湿度")<<"\""<<":"<<MB.env.humi<<",";
//                jsdata<<"\""<<Kstring::GBK_to_utf8("气压")<<"\""<<":"<<MB.env.press<<",";
//                jsdata<<"\""<<Kstring::GBK_to_utf8("二氧化碳")<<"\""<<":"<<MB.env.co2<<",";
//                jsdata<<"\""<<Kstring::GBK_to_utf8("光照")<<"\""<<":"<<MB.env.light<<",";
//                jsdata<<"\"PM1\""<<":"<<MB.env.pm1<<",";
//                jsdata<<"\"PM10\""<<":"<<MB.env.pm10<<",";
//                jsdata<<"\"PM2.5\""<<":"<<MB.env.pm25<<",";
//                jsdata<<"\"PM3.0\""<<":"<<MB.env.pm30<<",";
//                jsdata<<"\""<<Kstring::GBK_to_utf8("风向")<<"\""<<":"<<MB.env.wind_dir<<",";
                jsdata<<"\""<<Kstring::GBK_to_utf8("风速")<<"\""<<":"<<MB.env.wind_speed+5<<",";
//                jsdata<<"\""<<Kstring::GBK_to_utf8("总辐射")<<"\""<<":"<<MB.env.solar_rad<<",";
//                jsdata<<"\""<<Kstring::GBK_to_utf8("土壤温度")<<"\""<<":"<<MB.env.soil_temp<<",";
//                jsdata<<"\""<<Kstring::GBK_to_utf8("土壤湿度")<<"\""<<":"<<MB.env.soil_humi<<",";
//                jsdata<<"\""<<Kstring::GBK_to_utf8("土壤电导率")<<"\""<<":"<<MB.env.soil_ec<<",";
                jsdata<<"\""<<Kstring::GBK_to_utf8("土壤盐分")<<"\""<<":"<<MB.env.soil_salt;
                jsdata<<"}";

                //添加键值对
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("温度").data(),MB.env.temp);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("湿度").data(),MB.env.humi);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("气压").data(),MB.env.press);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("二氧化碳").data(),MB.env.co2);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("光照").data(),MB.env.light);
//                cJSON_AddNumberToObject(root,"PM1",MB.env.pm1);
//                cJSON_AddNumberToObject(root,"PM10",MB.env.pm10);
//                cJSON_AddNumberToObject(root,"PM2.5",MB.env.pm25);
//                cJSON_AddNumberToObject(root,"PM3.0",MB.env.pm30);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("风向").data(),MB.env.wind_dir);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("风速").data(),MB.env.wind_speed+2);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("总辐射").data(),MB.env.solar_rad);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("土壤温度").data(),MB.env.soil_temp);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("土壤湿度").data(),MB.env.soil_humi);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("土壤电导率").data(),MB.env.soil_ec);
//                cJSON_AddNumberToObject(root,Kstring::GBK_to_utf8("土壤盐分").data(),MB.env.soil_salt);
                //将JSON对象转化为字符串
//                string buf=cJSON_Print(root);
//                //删除JSON对象
//                cJSON_Delete(root);
                tb.PublishData(jsdata);
            }
        }

    }
}


