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
#include "FM24Cxx.h"
#include "Gateway.h"
#include "Communication.h"


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
#define LOGIC_STK_SIZE 		512
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

_OutPut_ run (GPIOE6),UP(GPIOE2),DW(GPIOE3),NET(GPIOE4);//����ָʾ��
_OutPut_ OUT(GPIOC10,HIGH);            //���
_OutPut_ RST(GPIOA8);//EC20��λ����
EC20    ET(USART1);//EC20
Timer tIM_EC(TIM5,100,8400,true);//EC20��ʱ��
_USART_ DEBUG(USART6);             //���Դ���
Communication com(USART3,GPIOD10,TIM7,100);//RS485
WH_COM        lora(UART5,TIM4,100);//LORA

ThingsBoard TB(&DEBUG,&ET);//TB
Gateway GW(GPIOB14,GPIOB15,FM24Cxx::AT24C256);//����

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����
    delay_ms(1000);//��ʱ1s

    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
    my_mem_init(SRAMCCM);		//��ʼ���ڲ��ڴ��

    lora.initial();
    if(GW.inital())DEBUG<<"GW OK\r\n";//��ʼ������
    else DEBUG<<"GW error\r\n";
    GW.print_env(&DEBUG);//��ӡ���ػ���
    GW.run_cmd(&DEBUG);//��������ָ��
    GW.link_OUT(&OUT);//�������
    GW.Link_rs485(&com);//����RS485
    GW.Link_lora(&lora);//����LORA

    ET.Link_RST_Pin(&RST);//����EC20��λ����
    ET.reset();//��λEC20

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
        vTaskDelay(200/portTICK_RATE_MS );
        run.change();       //����ָʾ��
        if(DEBUG.available())//��ȡ������Ч
            da = DEBUG.read_data("\r\n");//��ȡ���Դ�������

        if (!da.empty()) {
            DEBUG << da << ">>";
            DEBUG << GW.Command(da);
            DEBUG << "<<";
            da.clear();
        }
    }
}

//task2������
[[noreturn]] void EC20_task(void *pvParameters)
{
    DEBUG << "\r\n<<";
    while (!ET.getrdy()){
        delay_ms(1000);
        DEBUG<<".";
    }
    while(true) {
        DEBUG << "EC20:" << (ET.init() ? "OK" : "error") << "\r\n";
        ET.setdebug(&DEBUG);

        ET.Register(EC20::APN::APN_CMNET);
        ET.Link_TIMER_CALLback(&tIM_EC);
        GW.link_thingsBoard(&TB);
        while (true) {
            vTaskDelay(100 / portTICK_RATE_MS);//��ʱ100ms
            if (!ET.get_Link_Status())//�ж������Ƿ�����
                break;
            if(TB.Getdatacheck()){//�ж��Ƿ���������Ҫ�ϴ�
                UP.flicker(50);//����ָʾ��
            }
            if(TB.cmd.find("\r\n")!=std::string::npos){//�ж��Ƿ���ָ����Ҫִ��
                DEBUG<<GW.Command(TB.cmd);//ִ��ָ��
                TB.cmd.clear();//���ָ��
                DW.flicker(50);//����ָʾ��
            }
            NET.change();       //����ָʾ��
        }
    }
}


