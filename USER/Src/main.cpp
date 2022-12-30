#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "IIC.h"
#include "OLED_SSD1306.h"
#include "Communication.h"
#include "pretreatment.h"
#include "MS5805.h"
#include "FM24Cxx.h"
#include "Fower_Ctrl.h"


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

//IIC����
Software_IIC IIC_BUS(GPIOB8,GPIOB9);
//����ѹ������
MS5805  Atmospheric_P(&IIC_BUS);
//EEPROM ���ݴ洢
FM24Cxx power_data(&IIC_BUS);

//SPI����
SPI_S SPI_BUS(GPIOE5,GPIOE3,GPIOE6);
//Ԥ������������
pretreatment stovectrl(&SPI_BUS,GPIOE4,TIM4,1000,2,GPIOC10,GPIOC11);
//�����¶ȴ�����
MAX31865 Atmospheric_T(&SPI_BUS,GPIOE2);
//ͨ�������
Communication m_modebus(USART2,GPIOD4,TIM7,100,GPIOD5,GPIOD6);




Differential_pressure tes1(GPIOD1,GPIOD0,100000,-100000,TIM9,100);
Differential_pressure tes2(GPIOD3,GPIOD2,4000,-500,TIM10,100);

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
}led2(GPIOE1,TIM6,10);


class OLED:private Software_IIC,public OLED_SSD1306{
public:
    OLED(GPIO_Pin Pin_Scl, GPIO_Pin Pin_Sda){
        Software_IIC::init(Pin_Scl, Pin_Sda);
        OLED_SSD1306::config(this);
    }
    void initial(){
        this->OLED_SSD1306::init();
        this->CLS();
    }
}MLED(GPIOB12,GPIOB13);


int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����
    MLED.initial();             //OLED ��ʼ��

    Atmospheric_T.init();       //
    Atmospheric_T.config(MAX31865::PT100,3900);
    Atmospheric_P.init();       //����ѹ��ʼ��
    power_data.init();          //���ݴ洢��ʼ��
    //��ȡ��������
    power_data.read(0,m_modebus.data_BUS.to_u8t,sizeof(m_modebus.data_BUS));
    //����������ȷ��  ͷ���ϵ磨�汾�ţ�
    if(m_modebus.data_BUS.to_float.version-HARD_version!=0){
        K_POWER_DATA init_data;             //��ʵ��һ�������� ���췽�����ó�ʼ��ֵ
        m_modebus.data_BUS=init_data;       //ת�����ݵ�ʹ�ýṹ��
        power_data.write(0,init_data.to_u8t,sizeof(init_data));  //д������
    }

    stovectrl.initial();        //¯�ӳ�ʼ��

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
        vTaskDelay(200/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        MLED.Print(0,0,"V%.2lf",m_modebus.data_BUS.to_float.version);


        MLED.Print(0,2,"%.2lf",stovectrl.get_cur());

        MLED.Print(0,4,"%.2lf",Atmospheric_T.get_temp());

        MLED.Print(0,6,"%.2lf",Atmospheric_P.get_pres());
//        MLED.Print(0,6,"%.2lf",stovectrl.get_temp_cache());
//        pretreatment1.set_target(150);
//        pretreatment1.upset();

        m_modebus.data_BUS.to_float.stove_temp_r=stovectrl.get_cur();

        if(m_modebus.data_BUS.to_float.stove_work==1){
            stovectrl.turn_ON();
        }
        else
        {
            stovectrl.turn_OFF();
        }

    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(500);
        stovectrl.upset();
    }
}


