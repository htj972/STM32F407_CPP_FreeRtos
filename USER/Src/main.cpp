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
#include "lwip_comm/lwip_comm.h"
#include "udp/UDP_Class.h"
#include "fertilizer.h"
#include "cJSON.h"


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
        this->upload_extern_fun(this);
    }
    void set_mode(bool modex){
        this->mode=modex;
    }
    void Callback(int  ,char** ) override{
        if(mode)this->change();
        Feed_Dog();
        tims++;
    };
}led(GPIOE5,TIM6,2);//运行指示灯定时器

class lwip_:public Timer,public Call_Back{
public:
    lwip_(TIM_TypeDef *TIMx, uint16_t frq) {
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override {
        lwip_setup();
    }
}lwipw(TIM5,100);//运行指示灯定时器

_OutPut_ error_led (GPIOE6);//运行指示灯

//RS485   MB(USART3,GPIOB15,115200);  //调试串口
//modbus modbus1(&MB,modbus::HOST,1,1000,20);
Communication MB(USART3,GPIOB15,TIM7,100);//modbus通信
UDP_Class udp_demo(8089);

#define MD_Debug 0

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    WDG_Init();
    delay_init(168);	//初始化延时函数
    delay_ms(1000);//延时1s

    my_mem_init(SRAMIN);		//初始化内部内存池
    my_mem_init(SRAMCCM);		//初始化内部内存池
    lwip_dhcp_process_handle();
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
    while(lwip_comm_init()!=0)
    {
        delay_ms(1200);
    }

    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
    {
        lwip_periodic_handle();
    }
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
//    udp_demo.connect(192,168,31,173);
    udp_demo.bind();//绑定端口
    //发送准备就绪
    udp_demo.write("{\"ready\":true}");
    smatch result;
    while(true)
    {
        delay_ms(10);
        if(udp_demo.available()){
            error_led.change();
            udp_demo.set_romte_ip(udp_demo.get_remote_ip(),8089);
            string cmd=udp_demo.read_data();
            //删除cmd内的/n后，对比数据
            //使用正则表达式删除/n
            regex reg(R"(\n)");
            cmd=regex_replace(cmd,reg,"");
            if(cmd.find(R"({"cmd":"q_fertilizermach")")!=string::npos)
            {
                if(cmd.find(R"("call":")")!=string::npos)
                {
                    //find获取call的值
                    uint16_t pos=cmd.find(R"("call":")")+7;
                    uint16_t pot=cmd.find(R"("})");
                    uint16_t poe=cmd.find(R"("db":)")+5;
                    uint16_t pof=cmd.find(R"(,"call)");
                    udp_demo.write(MB.data_to_json(cmd.substr(poe,pof-poe),cmd.substr(pos,pot-pos)));
                }
                else
                {
                    udp_demo.write(MB.data_to_json());
                }
            }
            //{"cmd":"c_fertilizermach","press": 5.2,"flow": 0.2,"call":"asdasd","db":4}
            else if(cmd.find(R"({"cmd":"c_fertilizermach")")!=string::npos)
            {
//               FWmode.get_cmd_str(cmd);
                //只用正则表达式获取压力值和流量值
                regex reg1(R"("press":\s*(\d+\.\d+),\s*"flow":\s*(\d+\.\d+))");
                if(regex_search(cmd,result,reg1))
                {
                    float press=stof(result[1]);
                    float flow=stof(result[2]);
                    MB.send_fertilizermach(press,flow);
                    if(cmd.find(R"("call":")")!=string::npos)
                    {
                        //find获取call的值
                        uint16_t pos=cmd.find(R"("call":")")+7;
                        uint16_t pot=cmd.find(R"("})");
                        uint16_t poe=cmd.find(R"("db":)")+5;
                        uint16_t pof=cmd.find(R"(,"call)");
                        udp_demo.write(R"({"cmd_result":"ok","db":)"+cmd.substr(poe,pof-poe)+R"(,"call":")"+cmd.substr(pos,pot-pos)+R"("})");
                    }
                    else
                    {
                        udp_demo.write(R"({"cmd_result":"ok"})");
                    }
                }
            }
            //{"cmd":"c_fertilizerpump","type":1} //1开,2关,3暂停4恢复
            else if(cmd.find(R"({"cmd":"c_fertilizerpump")")!=string::npos)
            {
                regex reg2(R"("type":\s*(\d+))");
                if(regex_search(cmd,result,reg2))
                {
                    MB.send_fertilizerpump(stoi(result[1]));
                    if(cmd.find(R"("call":")")!=string::npos)
                    {
                        //find获取call的值
                        uint16_t pos=cmd.find(R"("call":")")+7;
                        uint16_t pot=cmd.find(R"("})");
                        uint16_t poe=cmd.find(R"("db":)")+5;
                        uint16_t pof=cmd.find(R"(,"call)");
                        udp_demo.write(R"({"cmd_result":"ok","db":)"+cmd.substr(poe,pof-poe)+R"(,"call":")"+cmd.substr(pos,pot-pos)+R"("})");
                    }
                    else
                    {
                        udp_demo.write(R"({"cmd_result":"ok"})");
                    }
                }
            }
            //{"cmd":"c_waterpump","type":1} //1开,2关,3暂停4恢复
            else if(cmd.find(R"({"cmd":"c_waterpump")")!=string::npos)
            {
                regex reg3(R"("type":\s*(\d+))");
                if(regex_search(cmd,result,reg3))
                {
                    MB.send_waterpump(stoi(result[1]));
                    if(cmd.find(R"("call":")")!=string::npos)
                    {
                        //find获取call的值
                        uint16_t pos=cmd.find(R"("call":")")+7;
                        uint16_t pot=cmd.find(R"("})");
                        uint16_t poe=cmd.find(R"("db":)")+5;
                        uint16_t pof=cmd.find(R"(,"call)");
                        udp_demo.write(R"({"cmd_result":"ok","db":)"+cmd.substr(poe,pof-poe)+R"(,"call":")"+cmd.substr(pos,pot-pos)+R"("})");
                    }
                    else {
                        udp_demo.write(R"({"cmd_result":"ok"})");
                    }
                }
            }

        }
    }
}

//task2任务函数
[[noreturn]] void RS485_task(void *pvParameters)
{
    uint8_t dsmi=0;
    uint8_t times=0;
    struct
    {
        float data;
        float value;
        uint16_t valuex;
    }received_data{};
    smatch gresult;
    while(true) {
        delay_ms(100);

        times++;
        if(times>=0) {
            MB.data_sync();
            MB.run_time_sync();
            times = 0;
        }

//        if (xQueueReceive(xMailbox, &gresult, portMAX_DELAY) == pdPASS) {
//            MB<<"press:"<<gresult[1]<<" flow:"<<gresult[2]<<"\r\n";
//
//        }
    }
}


