#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "TMC220xUart.h"



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
}led(GPIOE2,TIM6,2);

_USART_ DEBUG(USART1);
//_USART_ TMC_U(USART3);

class TMC_UART:public _USART_{
private:
    _OutPut_ CS1,CS2;
public:
    TMC_UART(GPIO_Pin CS_Pin1,GPIO_Pin CS_Pin2,USART_TypeDef* USARTx) {
        CS1.init(CS_Pin1,HIGH);
        CS2.init(CS_Pin2,HIGH);
        this->init(USARTx);
    }
    void send(uint8_t ch,string str){
        switch(ch){
            case 2:
                CS1.set(ON);
                CS2.set(OFF);
            break;
            case 3:
                CS1.set(ON);
                CS2.set(ON);
            break;
            case 4:
                CS1.set(OFF);
                CS2.set(ON);
            break;
            default:
                CS1.set(OFF);
                CS2.set(OFF);
            break;
        }
        this->write(std::move(str));
    }
}TMC_U(GPIOE15,GPIOE14,USART3);

TMC220xUart TMC_X(&TMC_U,GPIOE12,GPIOE13,GPIOA3);

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����
    delay_ms(1000);
    DEBUG.println((string)"ONLINE");
    TMC_U.send(1,"line1\r\n");

    TMC_X.init(16,50,2,90,true);
    TMC_X.Return_to_zero();

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

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );
//        DEBUG<<TMC_U.read_data();
    }
}


