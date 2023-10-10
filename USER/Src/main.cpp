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
#include "Communication.h"
#include "WDG.h"



//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С
#define START_STK_SIZE 		128
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define TASK1_TASK_PRIO		2
//�����ջ��С
#define TASK1_STK_SIZE 		512
//������
TaskHandle_t Task1Task_Handler;
//������
[[noreturn]] void task1_task(void *pvParameters);

//�������ȼ�
#define TASK2_TASK_PRIO		2
//�����ջ��С
#define TASK2_STK_SIZE 		512
//������
TaskHandle_t Task2Task_Handler;
//������
[[noreturn]] void task2_task(void *pvParameters);


//����ָʾ��
class T_led_:public _OutPut_,public Call_Back,public Timer{
public:
    T_led_(GPIO_Pin param,TIM_TypeDef *TIMx, uint16_t frq) {
        _OutPut_::init(param,LOW);
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override{
        Feed_Dog();
        this->change();
    }
}run(GPIOE5,TIM5,2);//����ָʾ�ƶ�ʱ��

_OutPut_ led (GPIOE6),BEEP (GPIOE4,HIGH);//����ָʾ��
_OutPut_ OUT1(GPIOE0,HIGH),OUT2(GPIOE1,HIGH);            //���

_USART_ DEBUG(USART1);
_USART_ Lora(USART6);

Communication MB(USART3,GPIOB15,TIM7,100);//modbusͨ��

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    WDG_Init();
    delay_init(168);	//��ʼ����ʱ����

    BEEP.flicker(100,250,2);//��������ʾ

    delay_ms(1000);
    MB.RS485::config(GPIOD8,GPIOD9);
//    YU.init();


    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,          //������
                (const char*    )"start_task",           //��������
                (uint16_t       )START_STK_SIZE,     //�����ջ��С
                (void*          )nullptr,            //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TASK1_TASK_PRIO,
                (TaskHandle_t*  )&Task1Task_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t )task2_task,
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TASK2_TASK_PRIO,
                (TaskHandle_t*  )&Task2Task_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//task1������
[[noreturn]] void task1_task(void *pvParameters)//alignas(8)
{
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        led.change();       //����ָʾ��
        //modbusͨ��
//        MB.data_sync();
    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    char dir[16][18]={{"��"},{"����ƫ��"},{"����"},{"����ƫ��"},
                      {"��"},{"����ƫ��"},{"����"},{"����ƫ��"},
                      {"��"},{"����ƫ��"},{"����"},{"����ƫ��"},
                      {"��"},{"����ƫ��"},{"����"},{"����ƫ��"}};
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );

        DEBUG<<Lora.read_data();
        Lora<<DEBUG.read_data();
    }
}


