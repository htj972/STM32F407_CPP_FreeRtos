#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "TMC220xUart.h"



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
}led(GPIOE2,TIM6,2);

_USART_ DEBUG(USART1);
//_USART_ TMC_U(USART3);

class TMC_UART:public _USART_{
private:
    _OutPut_ CS1,CS2;
public:
    TMC_UART(GPIO_Pin CS_Pin1,GPIO_Pin CS_Pin2,USART_TypeDef* USARTx) {
        CS1.init(CS_Pin1,HIGH);
        CS2.init(CS_Pin2,HIGH);
        this->init(USARTx);
    }
    void send(uint8_t ch,string str){
        switch(ch){
            case 2:
                CS1.set(ON);
                CS2.set(OFF);
            break;
            case 3:
                CS1.set(ON);
                CS2.set(ON);
            break;
            case 4:
                CS1.set(OFF);
                CS2.set(ON);
            break;
            default:
                CS1.set(OFF);
                CS2.set(OFF);
            break;
        }
        this->write(std::move(str));
    }
}TMC_U(GPIOE15,GPIOE14,USART3);

TMC220xUart TMC_X(&TMC_U,GPIOE12,GPIOE13,GPIOA3);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    delay_init(168);	//初始化延时函数
    delay_ms(1000);
    DEBUG.println((string)"ONLINE");
    TMC_U.send(1,"line1\r\n");

    TMC_X.init(16,50,2,90,true);
    TMC_X.Return_to_zero();

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
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );
//        DEBUG<<TMC_U.read_data();
    }
}


