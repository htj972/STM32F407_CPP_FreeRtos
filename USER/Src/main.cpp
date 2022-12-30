#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "IIC.h"
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
#define TASK2_STK_SIZE 		1024
//������
TaskHandle_t Task2Task_Handler;
//������
[[noreturn]] void task2_task(void *pvParameters);


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



int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����

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
    uint8_t sec_t=0;
    while(true) {
        vTaskDelay(200 / portTICK_RATE_MS);            //��ʱ10ms��ģ����������10ms���˺������������������

        m_modebus.data_sync();
    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    static uint8_t flag=0;
    uint8_t ret=0;
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );
        MDW.key_handle();
        MDW.Dis_handle();
//        if(MDW.get_curInterface()==3){
//            MDW.vspf_Text(0x1000,(char *)"%05.1lf",TEMP.temp_sensor[0].get_temp());
//            MDW.vspf_Text(0x1040,(char *)"%05.1lf",TEMP.temp_sensor[1].get_temp());
//            MDW.vspf_Text(0x1080,(char *)"%05.1lf",TEMP.temp_sensor[2].get_temp());
//            MDW.vspf_Text(0x10C0,(char *)"%05.1lf",TEMP.temp_sensor[3].get_temp());
//            MDW.vspf_Text(0x1100,(char *)"%05.1lf",TEMP.temp_sensor[4].get_temp());
//        }
    }
}


