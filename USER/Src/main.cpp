#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "Timer_queue.h"
#include "RS485.h"
#include "malloc.h"
#include "lwip_comm/lwip_comm.h"
#include "tcp_client_demo/tcp_client_demo.h"
#include "udp_demo/udp_demo.h"
#include "modbus.h"



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
        lwip_localtime+=10;
    };
}led(GPIOE5,TIM6,100);

_OutPut_ run (GPIOE6),BEEP (GPIOE4,HIGH);
_OutPut_ OUT1(GPIOE0),OUT2(GPIOE1);

Timer_queue tIMS(TIM7,50000);

_USART_ DEBUG(USART1);
_USART_ Lora(USART6);
RS485   YU(USART3,GPIOB15,9600);

modbus SUN(&YU,modbus::HOST);

uint16_t sun_data[2];

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��

    BEEP.set(ON);
    delay_ms(100);
    BEEP.set(OFF);
    delay_ms(250);

    BEEP.set(ON);
    delay_ms(100);
    BEEP.set(OFF);

    SUN.config(sun_data,2);

    delay_ms(1000);
    YU.config(GPIOD8,GPIOD9);

//    YU.init();


    DEBUG<<"Ethernet lwIP Test\r\n"<<"KOKIRIKA\r\n"<<"2023-6-2\r\n";

    DEBUG<<"lwIP Initing...\r\n";
    uint8_t ret =lwip_comm_init();
    while(ret!=0)
    {
        ret =lwip_comm_init();
        DEBUG<<"lwIP Init failed!\r\n";
        delay_ms(1200);
        DEBUG<<"Retrying...\r\n";
    }
    DEBUG<<"lwIP Init Successed\r\n";
    //�ȴ�DHCP��ȡ
    DEBUG<<"DHCP IP configing...\r\n";
    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//�ȴ�DHCP��ȡ�ɹ�/��ʱ���
    {
        lwip_periodic_handle();
    }

    uint8_t speed;
    char buf[30];

    DEBUG<<DHCP_str[0]<<DHCP_str[1]<<DHCP_str[2]<<DHCP_str[3]<<DHCP_str[4];

    DEBUG<<"lwIP DHCP Successed\r\n";
    if(lwipdev.dhcpstatus==2)sprintf((char*)buf,"DHCP IP:%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
    else sprintf((char*)buf,"Static IP:%d.%d.%d.%d\r\n",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);//��ӡ��̬IP��ַ
    DEBUG<<buf;
    speed=LAN8720_Get_Speed();//�õ�����
    if(speed&1<<1)DEBUG<<"Ethernet Speed:100M\r\n";
    else DEBUG<<"Ethernet Speed:10M\r\n";




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
        run.change();
        OUT1.change();
        OUT2.change();
        SUN.modbus_03_send(0,1);
    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );


        udp_demo_test();
        tcp_client_test();


    }
}


