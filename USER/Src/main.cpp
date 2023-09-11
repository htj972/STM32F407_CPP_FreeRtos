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
#include "tcp_client/TCP_Client_Class.h"
#include "MQTT/MQTT.h"
#include "Kstring.h"
#include "picosha2.h"
#include "ThingsBoard.h"
#include "cJSON.h"
#include "WDG.h"

/*�޸�APP����
 * /Bin/xxx.bin
 * system_stm32f4xx.c
 * ԭʼ  #define VECT_TAB_OFFSET  0x00
 * �޸�  #define VECT_TAB_OFFSET  0x8000
 *
 * STM32F407VGTx_FLASH.ld
 *ԭʼ
 * MEMORY
    {
    RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
    CCMRAM (xrw)      : ORIGIN = 0x10000000, LENGTH = 64K
    FLASH (rx)      : ORIGIN = 0x8000000, LENGTH = 1024K
    }
 * �޸�
 * MEMORY
    {
    RAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
    CCMRAM (xrw)      : ORIGIN = 0x10000000, LENGTH = 64K
    FLASH (rx)      : ORIGIN = 0x8008000, LENGTH = 992K
    }
 *
*/

//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С
#define START_STK_SIZE 		64
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define DATA_TASK_PRIO		2
//�����ջ��С
#define DATA_STK_SIZE 		128
//������
TaskHandle_t DATATask_Handler;
//������
[[noreturn]] void DATA_task(void *pvParameters);

//�������ȼ�
#define INTER_TASK_PRIO		2
//�����ջ��С
#define INTER_STK_SIZE 		(512*3)
//������
TaskHandle_t INTERTask_Handler;
//������
[[noreturn]] void INTER_task(void *pvParameters);

uint8_t times=0;
class lwip_:public Timer,public Call_Back{
public:
    lwip_(TIM_TypeDef *TIMx, uint16_t frq) {
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override {
        lwip_setup();
    }
}lwipw(TIM6,100);//����ָʾ�ƶ�ʱ��

class T_led_:public _OutPut_,public Call_Back,public Timer{
public:
    T_led_(GPIO_Pin param,TIM_TypeDef *TIMx, uint16_t frq) {
        _OutPut_::init(param,LOW);
        Timer::init(TIMx,10000/frq,8400,true);
        this->upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override{
        times++;
        Feed_Dog();
        this->change();
    }
}run(GPIOE5,TIM5,2);//����ָʾ�ƶ�ʱ��

_OutPut_ led (GPIOE6),BEEP (GPIOE4,HIGH);//����ָʾ��
_OutPut_ OUT1(GPIOE0,HIGH),OUT2(GPIOE1,HIGH);            //���

//Timer_queue tIMS(TIM7,50000);                 //��ʱ������

_USART_ DEBUG(USART1);                          //���Դ���
//_USART_ Lora(USART6);                           //Lora����

Communication MB(USART3,GPIOB15,TIM7,100);//modbusͨ��


TCP_Client_Class  tcp_sbc;

TCP_Client_Class  tcp_mq;//TCPͨ��
MQTT        mqtt_demo(&tcp_mq);
ThingsBoard tb(&DEBUG,&mqtt_demo);//

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    WDG_Init();
    delay_init(168);	//��ʼ����ʱ����

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
    my_mem_init(SRAMCCM);		//��ʼ���ڲ��ڴ��

    BEEP.flicker(100,250,2);//��������ʾ

    delay_ms(1000);//��ʱ1s
    MB.RS485::config(GPIOD8,GPIOD9);//����RS485 GPIO����

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
    xTaskCreate((TaskFunction_t )DATA_task,
                (const char*    )"DATA_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask_Handler);
    //����TASK2����
    xTaskCreate((TaskFunction_t )INTER_task,
                (const char*    )"INTER_task",
                (uint16_t       )INTER_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )INTER_TASK_PRIO,
                (TaskHandle_t*  )&INTERTask_Handler);
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//task1������
[[noreturn]] void DATA_task(void *pvParameters)//alignas(8)
{
    string str;
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        run.change();       //����ָʾ��
        //modbusͨ��
        MB.data_sync();
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

//��λ��TCP bug ��Ҫ�ڷ�������ǰ����4�ֽڵ����ݳ���
void tcp_check_send(TCP_Client_Class *tcp,const string& str){
    Kstring check;
    union {
        int len;
        char data[4];
    }da{};
    da.len=(int)str.length();
    check<<da.data[3];
    check<<da.data[2];
    check<<da.data[1];
    check<<da.data[0];
    check<<str;
    *tcp<<check;
}

//task2������
[[noreturn]] void INTER_task(void *pvParameters)
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
        while(!ThingsBoard::PHY_islink());
        if(!tcp_sbc.islink()){
            tcp_sbc.connect(10,40,12,40,50013);
        }
        if(!tb.intel_islink()) {
            tb.Connect(222, 74, 215, 220, 31883);
            tb.config("1234567", "1234567", "1234567");
            DEBUG<<"���Ľ��"<<tb.SubscribeTopic()<<"\r\n";
        }
        DEBUG<<"linking...\r\n";
        if(tb.intel_islink()&&tcp_sbc.islink()&&ThingsBoard::PHY_islink())
        while (true){
            delay_ms(100);

            if((!tb.intel_islink())||(!tcp_sbc.islink())||(!ThingsBoard::PHY_islink())){
                tb.inter_unlink();
                tcp_sbc.close();
                break;
            }

            tb.Getdatacheck();
//            tb.GetVersion();

            if(tb.TCP_data_check(&tcp_sbc))times=20;

            if(times>20){
                times=0;
                string sensor_str=MB.data_to_json();
                DEBUG<<"sensor:"<<sensor_str<<"\r\n";
                if(tcp_sbc.islink()){
                    Kstring str = ThingsBoard::TCP_data_process(sensor_str);
                    tcp_check_send(&tcp_sbc, str.GBK_to_utf8());
                }
                if(tb.intel_islink()) {
                    Kstring buf = sensor_str;
                    tb.PublishData(buf.GBK_to_utf8());
                }
            }
        }
    }
}


