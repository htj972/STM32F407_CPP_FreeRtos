#include <regex>
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
#include "ThingsBoard.h"
#include "Kstring.h"


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С
#define START_STK_SIZE 		64
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LOGIC_TASK_PRIO		2
//�����ջ��С
#define LOGIC_STK_SIZE 		128
//������
TaskHandle_t LOGICTask_Handler;
//������
[[noreturn]] void LOGIC_task(void *pvParameters);

//�������ȼ�
#define EC20_TASK_PRIO		2
//�����ջ��С
#define EC20_STK_SIZE 		(128*4)
//������
TaskHandle_t EC20Task_Handler;
//������
[[noreturn]] void EC20_task(void *pvParameters);

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

_OutPut_ run (GPIOE6),RST(GPIOA11);//����ָʾ��
_OutPut_ OUT(GPIOB0,HIGH);            //���

//Timer_queue tIMS(TIM7,50000);            //��ʱ������

Timer tIM_EC(TIM5,100,8400,true);

_USART_ DEBUG(USART2);             //���Դ���
RS485   com(USART3,GPIOB15);

EC20    ET(USART1);
ThingsBoard TB(&DEBUG,&ET);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����
    delay_ms(1000);//��ʱ1s

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
    my_mem_init(SRAMCCM);		//��ʼ���ڲ��ڴ��
    ET.Link_RST_Pin(&RST);
    ET.reset();

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
    xTaskCreate((TaskFunction_t )LOGIC_task,
                (const char*    )"LOGIC_task",
                (uint16_t       )LOGIC_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )LOGIC_TASK_PRIO,
                (TaskHandle_t*  )&LOGICTask_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t )EC20_task,
                (const char*    )"EC20_task",
                (uint16_t       )EC20_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )EC20_TASK_PRIO,
                (TaskHandle_t*  )&EC20Task_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//task1������
[[noreturn]] void LOGIC_task(void *pvParameters)//alignas(8)
{
    while(true)
    {
        vTaskDelay(200/portTICK_RATE_MS );
        run.change();       //����ָʾ��
    }
}


//task2������
[[noreturn]] void EC20_task(void *pvParameters)
{
    Kstring da;
    while (!ET.getrdy()){
        delay_ms(1000);
        DEBUG<<".";
    }
    while(true) {
        DEBUG << "EC20:" << (ET.init() ? "OK" : "error") << "\r\n";
        ET.setdebug(&DEBUG);

        ET.Register(EC20::APN::APN_CMNET);
        ET.Link_TIMER_CALLback(&tIM_EC);

        TB.Connect(222, 74, 215, 220, 31883);
        TB.config("EC20", "EC20", "EC20");
        TB.SubscribeTopic();
        while (true) {
            vTaskDelay(100 / portTICK_RATE_MS);

            if (!ET.get_Link_Status())
                break;

            da += DEBUG.read_data();
            if (da.find("\r\n") != string::npos) {
            TB.PublishData(da.GBK_to_utf8());
            da.clear();
//                ET << da;
//                da.clear();
            }
//            DEBUG << ET.read_data();
        TB.Getdatacheck();
        }
    }
}


