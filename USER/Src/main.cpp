#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "Communication.h"
#include "DW_DIS.h"
#include "TEOM.h"


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С
#define START_STK_SIZE 		128
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define TEMP_TASK_PRIO		2
//�����ջ��С
#define TEMP_STK_SIZE 		128
//������
TaskHandle_t TEMPTask_Handler;
//������
[[noreturn]] void TEMP_task(void *pvParameters);

//�������ȼ�
#define PRE_TASK_PRIO		2
//�����ջ��С
#define PRE_STK_SIZE 		128
//������
TaskHandle_t PRETask_Handler;
//������
[[noreturn]] void PRE_task(void *pvParameters);

//�������ȼ�
#define COM_TASK_PRIO		3
//�����ջ��С
#define COM_STK_SIZE 		512
//������
TaskHandle_t COMTask_Handler;
//������
[[noreturn]] void COM_task(void *pvParameters);

//�������ȼ�
#define DIS_TASK_PRIO		3
//�����ջ��С
#define DIS_STK_SIZE 		1024
//������
TaskHandle_t DISTask_Handler;
//������
[[noreturn]] void DIS_task(void *pvParameters);


_OutPut_    led(GPIOE6);

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
}led2(GPIOE5,TIM6,2);

//TEOM ������
TEOM_Machine teom(GPIOB0,GPIOB1,GPIOD14,TIM4,2);
//TEOM �¶ȿ�����
TEOM_TEMP TEMP(SPI1,GPIOC0,GPIOC1,GPIOC2,GPIOC3,GPIOA0,TIM1,1000);
//ͨ�������
Communication m_modebus(USART3,GPIOE15,TIM5,100);
//������ʾ��
DW_DIS MDW(USART6,TIM7,10);
//��ѹ��ʾ
pressure_dif pressure(GPIOD4,GPIOD5);




int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����
    delay_ms(1000);
    TEMP.initial();
    teom.inital();
    MDW.Link_TEOM(&teom);
    MDW.Link_Temp(&TEMP);
    MDW.Link_Com(&m_modebus);
    MDW.initial();



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
    //�����¶�����
    xTaskCreate((TaskFunction_t )TEMP_task,
                (const char*    )"TEMP_task",
                (uint16_t       )TEMP_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )TEMP_TASK_PRIO,
                (TaskHandle_t*  )&TEMPTask_Handler);
    //����ѹ������
    xTaskCreate((TaskFunction_t )PRE_task,
                (const char*    )"PRE_task",
                (uint16_t       )PRE_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )PRE_TASK_PRIO,
                (TaskHandle_t*  )&PRETask_Handler);
    //����COM����
    xTaskCreate((TaskFunction_t )COM_task,
                (const char*    )"COM_task",
                (uint16_t       )COM_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )COM_TASK_PRIO,
                (TaskHandle_t*  )&COMTask_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t )DIS_task,
                (const char*    )"DIS_task",
                (uint16_t       )DIS_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DIS_TASK_PRIO,
                (TaskHandle_t*  )&DISTask_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�¶ȶ�ȡ����
[[noreturn]] void TEMP_task(void *pvParameters)
{
    while(true) {
        delay_ms(100);
    }
}
//ѹ����ȡ����
[[noreturn]] void PRE_task(void *pvParameters)
{
    while(true) {
        delay_ms(100);
        pressure.read();
    }
}
//��ͨ������
[[noreturn]] void COM_task(void *pvParameters)
{
    uint8_t sec_t=0;
    while(true) {
        vTaskDelay(200 / portTICK_RATE_MS);
        m_modebus.data_sync();

    }
}
//����������
[[noreturn]] void DIS_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );
        MDW.key_handle();
        MDW.Dis_handle();
    }
}


