#include "sys.h"
#include "delay.h"
#include "USART.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
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
#define TASK1_STK_SIZE 		128
//������
TaskHandle_t Task1Task_Handler;
//������
[[noreturn]] void task1_task(void *pvParameters);

//�������ȼ�
#define TASK2_TASK_PRIO		2
//�����ջ��С
#define TASK2_STK_SIZE 		128
//������
TaskHandle_t Task2Task_Handler;
//������
[[noreturn]] void task2_task(void *pvParameters);

_OutPut_ led;
_USART_  U1;

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);					//��ʼ����ʱ����

    U1.init(USART1,115200);
    led.init(GPIOD3,LOW);
    //U1.set_send_DMA();
    U1.write("adsda321s3a1d3a1sd3sd\r\n");


    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,            //������
                (const char*    )"start_task",          //��������
                (uint16_t       )START_STK_SIZE,        //�����ջ��С
                (void*          )nullptr,                  //���ݸ��������Ĳ���
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
[[noreturn]] void task1_task(void *pvParameters)
{
    u8 task1_num=0;
    while(true)
    {
        task1_num++;					//����1ִ�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
        taskENTER_CRITICAL();           //�����ٽ���
        taskEXIT_CRITICAL();            //�˳��ٽ���
        led.change();
        vTaskDelay(500/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        //U1<<"sadasd:"<<15<<asdd;
    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    u8 task2_num=0;
    while(true)
    {
        task2_num++;					//����2ִ�д�����1 ע��task2_num1�ӵ�255��ʱ������㣡��
        taskENTER_CRITICAL();           //�����ٽ���
        taskEXIT_CRITICAL();            //�˳��ٽ���
        vTaskDelay(1000/portTICK_RATE_MS );
        U1<<U1.read_data();
    }
}


