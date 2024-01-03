#include <regex>
#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "malloc.h"
#include "EC20.h"
#include "ThingsBoard.h"
#include "Kstring.h"
#include "Gateway.h"
#include "Communication.h"
#include "WDG.h"


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
#define EC20_TASK_PRIO		3
//�����ջ��С
#define EC20_STK_SIZE 		(128*10)
//������
TaskHandle_t EC20Task_Handler;
//������
[[noreturn]] void EC20_task(void *pvParameters);
uint16_t tims=0;
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
        Feed_Dog();
        tims++;
    };
}led(GPIOE5,TIM6,2);//����ָʾ�ƶ�ʱ��

_OutPut_ run (GPIOE6);//����ָʾ��
//_OutPut_ Beep(GPIOE4,LOW);          //������
_OutPut_ OUT(GPIOC10,HIGH);            //���
_OutPut_ RST(GPIOA11);//EC20��λ����

Timer tIM_EC(TIM5,100,8400,true);

_USART_ DEBUG(USART2);             //���Դ���
//RS485   com(USART3,GPIOD10);

EC20    ET(USART1);
ThingsBoard TB(&DEBUG,&ET);

Communication COM(USART3,GPIOB15,TIM7,100);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    WDG_Init();
    delay_init(168);	//��ʼ����ʱ����
    delay_ms(1000);//��ʱ1s

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
    my_mem_init(SRAMCCM);		//��ʼ���ڲ��ڴ��

//    Beep.flicker(100,200,2);

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
    Kstring da;
    while(true)
    {
        vTaskDelay(1000/portTICK_RATE_MS );
        run.change();
        COM.data_sync();
    }
}

//task2������
[[noreturn]] void EC20_task(void *pvParameters)
{
    while(true) {
        while (!ET.getrdy()) {
            delay_ms(1000);
            DEBUG << ".";
        }
        while (true) {
            DEBUG << "EC20:" << (ET.init() ? "OK" : "error") << "\r\n";
            ET.setdebug(&DEBUG);
            ET.setGPS();
            ET.Register(EC20::APN::APN_CMNET);
            ET.Link_TIMER_CALLback(&tIM_EC);

            TB.Connect(222, 74, 215, 220, 31883);
            TB.config("XCGY", "XC001", "XC001");
            TB.SubscribeTopic();
            while (true) {
                vTaskDelay(100 / portTICK_RATE_MS);
                if (!ET.get_Link_Status())
                    break;
                TB.Getdatacheck();
                if(tims>=10*2){
                    tims=0;
                    string sensor_str=COM.data_to_json();
                    if(ET.getGPS()){
                        //ɾ������ġ�}��
                        sensor_str.pop_back();
                        sensor_str.append(R"(,"date":")");
                        sensor_str.append(ET.GPS_D.date);
                        sensor_str.append(R"(","UTC":")");
                        sensor_str.append(ET.GPS_D.UTC);
                        sensor_str.append(R"(","longitude":")");
                        sensor_str.append(ET.GPS_D.longitude);
                        sensor_str.append(R"(","latitude":")");
                        sensor_str.append(ET.GPS_D.latitude);
                        sensor_str.append(R"("})");
                    }
//                    DEBUG<<sensor_str<<"\r\n";
                    TB.PublishData(sensor_str);

                }
            }
        }
    }
}


