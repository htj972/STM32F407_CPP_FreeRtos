#include "sys.h"
#include "delay.h"
#include "USART.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "OLED_SSD1306.h"
#include "Timer.h"
#include "DW_LCD.h"



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
#define TASK2_STK_SIZE 		128
//������
TaskHandle_t Task2Task_Handler;
//������
[[noreturn]] void task2_task(void *pvParameters);

_OutPut_        led(GPIOE6);
_USART_         U1(USART1,115200);
Software_IIC    SIIC1(GPIOB4,GPIOB5);
OLED_SSD1306    MOLED(&SIIC1,OLED_SSD1306::Queue::OWN_Queue);

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
}led2(GPIOD9,TIM6,10);

class K_DW: private _USART_,public DW_LCD,private Timer{
public:
    K_DW(USART_TypeDef* USARTx,TIM_TypeDef *TIMx, uint16_t frq) {
        _USART_::init(USARTx,115200);
        DW_LCD::init(this);
        Timer::init(TIMx,10000/frq,8400,true);
    }
    void init(){
        DW_LCD::Timer_Link(this);
    }
}MDW(USART2,TIM7,20);


int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);					//��ʼ����ʱ����

    MOLED.init();
    MOLED.Fill(0xff);
    delay_ms(1000);
    MOLED.Fill(0x00);
//    LCD.Interface_switching(1);
//    LCD.Timer_Link(&DWTx);
    MDW.init();
    MDW.Interface_switching(1);
    MDW.SetBackLight(20);
    MDW.set_Progress_bar(PIC_ADD(1),50,87,362,724,399);
    delay_ms(1000);
    MDW.Interface_switching(3);

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
    while(true)
    {
        vTaskDelay(1000/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(200/portTICK_RATE_MS );
        U1<<U1.read_data();
        MOLED.Queue_star();
        led.change();
        MOLED.Queue_end();
        if (MDW.get_key_sata())
        if(MDW.get_key_address()==key_H_address) {
            switch (MDW.get_key_data()) {
                case 0:
                    MDW.Interface_switching(3);
                    break;
                case 1:
                    MDW.Interface_switching(5);
                break;
                case 2:
                    MDW.Interface_switching(7);
                    break;
                case 3:
                    MDW.Interface_switching(9);
                    break;
                case 4:
                    MDW.Interface_switching(11);
                    break;
            }
        }
    }
}


