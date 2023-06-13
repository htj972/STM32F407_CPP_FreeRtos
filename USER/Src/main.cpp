#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "TMC220xUart.h"
#include "CD4052.h"
#include "PWM.h"
#include "Timer_queue.h"
#include "RS485.h"



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
}led(GPIOE5,TIM6,2);

Timer_queue tIMS(TIM7,50000);

_USART_ DEBUG(USART1);
_USART_ Lora(USART6);
RS485   YU(USART3,GPIOB15,9600);



int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数

    delay_ms(1000);
    YU.config(GPIOD8,GPIOD9);
//    YU.init();


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
    while(true)
    {
        vTaskDelay(1000/portTICK_RATE_MS );			//延时10ms，模拟任务运行10ms，此函数不会引起任务调度

    }
}

//task2任务函数
[[noreturn]] void task2_task(void *pvParameters)
{
    char dir[16][18]={{"东"},{"东南偏东"},{"东南"},{"东南偏南"},
                      {"南"},{"西南偏南"},{"西南"},{"西南偏西"},
                      {"西"},{"西北偏西"},{"西北"},{"西北偏北"},
                      {"北"},{"东北偏北"},{"东北"},{"东北偏东"}};
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );

        string Interceptdata=Lora.read_data();

        DEBUG<<Interceptdata;
        Lora<<DEBUG.read_data();


        Lora<<YU.read_data();
        // 0B 03 00 00 00 1C 44 A9 0B
        //比较前段数据
        static string Intercept;
        Intercept+=Interceptdata;
        uint8_t headdata[]={0x0B,0x03,0x00,0x00,0x00,0x1C,0x44,0xA9,0x0B};
        string headd=string((char*)headdata,9);
        if(Intercept.find(headd)==string::npos){
            Intercept.clear();
            continue;
        } else{
            Intercept=Intercept.substr(Intercept.find(headd));
        }

        if((Intercept[0]==0x0B)&&((Intercept[1]==0x03)
            &&(Intercept[2]==0x00)&&(Intercept[3]==0x00)
            &&(Intercept[4]==0x00)&&(Intercept[5]==0x1C)
            &&(Intercept[6]==0x44)&&(Intercept[7]==0xA9)
            &&(Intercept[8]==0x0B))){
            //打印原数据以及转换数据
            DEBUG<<"解析数据\r\n";
            DEBUG<<"数据序号  原始数据  十进制转换\r\n";
            uint16_t data_last=0;
            for(uint8_t i=0;i<Intercept[5];i++){
                uint16_t data=Intercept[11+i*2]*256+Intercept[12+i*2];
//                DEBUG.print("data[%02d]:0x%04X  %d\r\n",i+1,data,data);
                switch (i+1) {
                    case 1:DEBUG.print("data[%02d]:湿度     :%.1f\r\n",i+1,data/10.0);
                        break;
                    case 2:DEBUG.print("data[%02d]:温度     :%.1f\r\n",i+1,data/10.0);
                        break;
                    case 3:DEBUG.print("data[%02d]:土壤湿度 :%.1f\r\n",i+1,data/10.0);
                        break;
                    case 4:DEBUG.print("data[%02d]:土壤温度 :%.1f\r\n",i+1,data/10.0);
                        break;
                    case 5:DEBUG.print("data[%02d]:PM2.5    :%d\r\n",i+1,data);
                        break;
                    case 8:
                        data_last=data;
                        break;
                    case 9:DEBUG.print("data[%02d]:光照强度    :%d\r\n",i+1,(data_last<<16)+data);
                        break;
                    case 10:DEBUG.print("data[%02d]:PM10    :%d\r\n",i+1,data);
                        break;
                    case 11:data_last=data;
                        break;
                    case 12:DEBUG.print("data[%02d]:气压    :%d\r\n",i+1,(data_last<<16)+data);
                        break;
                    case 13:data_last=data;
                        break;
                    case 14:DEBUG.print("data[%02d]:PH      :%.1f\r\n",i+1,((data_last<<16)+data)/100.0);
                        break;
                    case 15:DEBUG.print("data[%02d]:雨量      :%.1f\r\n",i+1,data/10.0);
                        break;
                    case 16:
                        data_last=data;
                        break;
                    case 17:DEBUG.print("data[%02d]:总辐射  :%d\r\n",i+1,(data_last<<16)+data);
                        break;
                    case 22:DEBUG.print("data[%02d]:电导率  :%d\r\n",i+1,data);
                        break;
                    case 23:DEBUG.print("data[%02d]:风向 0x%X %d  :%s\r\n",i+1,data,data,dir[data%16]);
                        break;
                    case 24:DEBUG.print("data[%02d]:风速    :%.1f\r\n",i+1,data/10.0);
                        break;
                    case 26:DEBUG.print("data[%02d]:氮     :%d\r\n",i+1,data);
                        break;
                    case 27:DEBUG.print("data[%02d]:磷     :%d\r\n",i+1,data);
                        break;
                    case 28:DEBUG.print("data[%02d]:钾     :%.d\r\n",i+1,data);
                        break;
                    default:
                        DEBUG.print("data[%02d]:0x%04X   %d\r\n",i+1,data,data);
                        break;
                }
            }
            DEBUG<<"解析完毕\r\n";
            Intercept.erase(0,69);
        }


    }
}


