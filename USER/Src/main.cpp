#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "RS485.h"
#include "malloc.h"
#include "lwip_comm/lwip_comm.h"
#include "Communication.h"
#include "udp/UDP_Class.h"
#include "cJSON.h"
#include "tcp_client/TCP_Client_Class.h"
#include "MQTT/MQTT.h"
#include "Kstring.h"
#include "picosha2.h"
#include "ThingsBoard.h"



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
#define TASK2_STK_SIZE 		512*2
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
//        this->change();
        lwip_setup();
    };
}led(GPIOE5,TIM6,1000);//����ָʾ�ƶ�ʱ��

_OutPut_ run (GPIOE6),BEEP (GPIOE4,HIGH);//����ָʾ��
_OutPut_ OUT1(GPIOE0),OUT2(GPIOE1);            //���

//Timer_queue tIMS(TIM7,50000);                 //��ʱ������

_USART_ DEBUG(USART1);                          //���Դ���
_USART_ Lora(USART6);                           //Lora����

Communication SUN(USART3,GPIOB15,TIM7,100);//modbusͨ��

TCP_Client_Class  tcp_mq;//TCPͨ��
MQTT    mqtt_demo(&tcp_mq);
ThingsBoard tb(&DEBUG,&mqtt_demo);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��

//    BEEP.flicker(100,250,2);//��������ʾ

    delay_ms(1000);//��ʱ1s
    SUN.RS485::config(GPIOD8,GPIOD9);//����RS485 GPIO����

//    Kstring asdsad="����";
////    asdsad.append("����");
//    string ret=asdsad.GBK_to_utf8();
//    //16���ƴ�ӡRET
//    for (char i : ret) {
//        DEBUG.print("0x%02X ",i);
//    }

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
    string str;
    while(true)
    {
        vTaskDelay(500/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        run.change();       //����ָʾ��
        OUT1.change();      //���״̬��ת
        OUT2.change();      //���״̬��ת
        if(ThingsBoard::PHY_islink())
        {
            led.set(ON);
        }
        else
        {
            led.set(OFF);
        }
    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{

//    std::string src_str = "Hello, World!";
//    std::string hash_hex_str;
//    picosha2::hash256_hex_string(src_str, hash_hex_str);
//    DEBUG << "SHA256(" << src_str << ") = " << hash_hex_str << "\r\n";
//
//
//    std::string src_str1 = "Hello,";
//    std::string src_str2 = " World!";
//    std::vector<unsigned char> hash(picosha2::k_digest_size);
//
//    picosha2::hash256_one_by_one hasher;
//    hasher.process(src_str1.begin(), src_str1.end());
//    hasher.process(src_str2.begin(), src_str2.end());
//    hasher.finish();
//    hasher.get_hash_bytes(hash.begin(), hash.end());
//
//    std::string hash_hex_str2 = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
//
//    DEBUG << "SHA256(Hello, World!) = " << hash_hex_str2 << "\r\n";

    tb.intel_link();

    while(true)
    {
//        string buf;
        uint8_t num=5;
        uint8_t times=0;
        while(!ThingsBoard::PHY_islink());
        tb.Connect(222,74,215,220,31883);
        tb.config("daocaoren","daocaoren","daocaoren");
        DEBUG<<"���Ľ��"<<tb.SubscribeTopic()<<"\r\n";

        DEBUG<<"linking...\r\n";
        mqtt_demo.Clear();
        tb.updata_step=3;
        while (true){
//            delay_ms(100);
//            times++;
//            if(times>100){
//                times=0;
//                tb.PublishData("����",num++);
//            }


            tb.Getdatacheck();
            tb.GetVersion();



            if(!tb.intel_islink()) {
                tb.inter_unlink();
                break;
            }
            if(!ThingsBoard::PHY_islink()){
                break;
            }
        }

    }
}


