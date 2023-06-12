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
        this->change();
    };
}led(GPIOE5,TIM6,2);

Timer_queue tIMS(TIM7,50000);

_USART_ DEBUG(USART1);
_USART_ Lora(USART6);
RS485   YU(USART3,GPIOB15,9600);



int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����

    delay_ms(1000);
    YU.config(GPIOD8,GPIOD9);
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
        vTaskDelay(1000/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������

    }
}
// 0B 03 00 00 00 1C 44 A9 0B
// 03 38 00 FF 01 08 03 E8 00 F4 00 33 13 88 00 00 00 02 42 FD 00 47 00 01 59 B0 00 00 02 92 00 03 00 00 01 93 00 00 00 00 00 00 00 00 00 1A 00 31 00 09 00 00 00 01 00 02 00 05 34 BB

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );

        string Interceptdata=Lora.read_data();

        DEBUG<<Interceptdata;
        Lora<<DEBUG.read_data();


        Lora<<YU.read_data();
        // 0B 03 00 00 00 1C 44 A9 0B
        //�Ƚ�ǰ������
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
            //��ӡԭ�����Լ�ת������
            DEBUG<<"��������\r\n";
            DEBUG<<"�������  ԭʼ����  ʮ����ת��\r\n";
            for(uint8_t i=0;i<Intercept[5];i++){
                uint16_t data=Intercept[11+i*2]*256+Intercept[12+i*2];
                DEBUG.print("data[%02d]:0x%04X  %d\r\n",i+1,data,data);
            }
            DEBUG<<"�������\r\n";
            Intercept.erase(0,69);
        }


    }
}


