#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "RS485.h"
#include "malloc.h"
#include "EC20.h"


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

extern u32 lwip_localtime;		//lwip����ʱ�������,��λ:ms
//����ָʾ��
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
}led(GPIOE5,TIM6,2);//����ָʾ�ƶ�ʱ��

_OutPut_ run (GPIOE6);//����ָʾ��
_OutPut_ OUT(GPIOB0);            //���

//Timer_queue tIMS(TIM7,50000);            //��ʱ������

_USART_ DEBUG(USART2);             //���Դ���
RS485   com(USART3,GPIOB15);
//_USART_ us(USART1);
EC20    ET(USART1);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����
    delay_ms(1000);//��ʱ1s

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
//    ET.init(&us);
    while (!ET.getrdy()){
        delay_ms(1000);
        DEBUG<<".";
    }
    DEBUG<<"EC20:"<<(ET.init()?"OK":"error");
    ET.setdebug(&DEBUG);
    ET.Register(EC20::APN::APN_CMNET);


    ET.mqttopen(1,"222.74.215.220",31883);
    ET.mqttconn(1,"EC20","EC20","EC20");
//    ET.mqttconn(1,"75287960-4549-11ee-9a3c-af6ad99d76ae","123456");


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
        vTaskDelay(200/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        run.change();       //����ָʾ��
        OUT.change();      //���״̬��ת
    }
}


//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    string da;
    while(true) {
        vTaskDelay(100/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������

        da+=DEBUG.read_data();
        if(da.find("\r\n") != string::npos) {
            ET.mqttpub(1, "v1/devices/me/telemetry", da);
            da.clear();
        }

//        ET<<DEBUG.read_data();
//        DEBUG<<ET.read_data();
    }
}


