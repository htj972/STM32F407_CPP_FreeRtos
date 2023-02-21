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
#include "DS18B20.h"


//�������ȼ�
#define START_TASK_PRIO		1
//�����ջ��С
#define START_STK_SIZE 		128
//������
TaskHandle_t StartTask_Handler;
//������
void start_task(void *pvParameters);

//�������ȼ�
#define LOGIC_TASK_PRIO		3
//�����ջ��С
#define LOGIC_STK_SIZE 		512
//������
TaskHandle_t LOGICTask_Handler;
//������
[[noreturn]] void LOGIC_task(void *pvParameters);

//�������ȼ�
#define PID_TASK_PRIO		2
//�����ջ��С
#define PID_STK_SIZE 		256
//������
TaskHandle_t PIDTask_Handler;
//������
[[noreturn]] void PID_task(void *pvParameters);

//�������ȼ�
#define DATA_TASK_PRIO		2
//�����ջ��С
#define DATA_STK_SIZE 		128
//������
TaskHandle_t DATATask_Handler;
//������
[[noreturn]] void DATA_task(void *pvParameters);

//������
TaskHandle_t DATATask1_Handler;
//������
[[noreturn]] void DATA1_task(void *pvParameters);

//������
TaskHandle_t DATATask2_Handler;
//������
[[noreturn]] void DATA2_task(void *pvParameters);

//������
TaskHandle_t DATATask3_Handler;
//������
[[noreturn]] void DATA3_task(void *pvParameters);

//������
TaskHandle_t DATATask4_Handler;
//������
[[noreturn]] void DATA4_task(void *pvParameters);

//������
TaskHandle_t DATATask5_Handler;
//������
[[noreturn]] void DATA5_task(void *pvParameters);

//IIC����
Software_IIC IIC_BUS(GPIOB8,GPIOB9);
//����ѹ������
MS5805  Atmospheric_P(&IIC_BUS);
//EEPROM ���ݴ洢
FM24Cxx power_data(&IIC_BUS);

//Ӳ��PWM
PWM_H PWM(TIM4,1000);
//SPI����
SPI_S SPI_BUS(GPIOE5,GPIOE3,GPIOE6);
//Ԥ������������
pretreatment stovectrl(&SPI_BUS,GPIOE4,&PWM,2,GPIOC10,GPIOC11);
//�����¶ȴ�����
MAX31865 Atmospheric_T(&SPI_BUS,GPIOE2);
//ͨ�������
Communication m_modebus(USART2,GPIOD4,TIM7,1000,GPIOD5,GPIOD6);


Differential_pressure JY(GPIOD1,GPIOD0,100000,-100000);
Differential_pressure LY(GPIOD3,GPIOD2,4000,-500);
//����
DS18B20  JW(GPIOB6);

Fower_Ctrl LL(&LY,&JY,&Atmospheric_P,&JW,&Atmospheric_T);

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

    delay_ms(500);

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
    PWM.config(1,2);
    LL.config(&PWM,1);
    LL.FLOW_RATE=m_modebus.data_BUS.to_float.Flow_coefficient;
    LL.set_ladder(10);
    stovectrl.initial();        //¯�ӳ�ʼ��
    JW.init();

//    PWM.set_channel1_Duty_cycle(5 );

    taskENTER_CRITICAL();           //�����ٽ���
    //������ʼ����
    xTaskCreate((TaskFunction_t )start_task,          //������
                (const char*    )"start_task",           //��������
                (uint16_t       )START_STK_SIZE,     //�����ջ��С
                (void*          )nullptr,            //���ݸ��������Ĳ���
                (UBaseType_t    )START_TASK_PRIO,       //�������ȼ�
                (TaskHandle_t*  )&StartTask_Handler);   //������
    taskEXIT_CRITICAL();            //�˳��ٽ���
    vTaskStartScheduler();          //�����������
}

//��ʼ����������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //����LOGIC����
    xTaskCreate((TaskFunction_t )LOGIC_task,
                (const char*    )"LOGIC_task",
                (uint16_t       )LOGIC_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )LOGIC_TASK_PRIO,
                (TaskHandle_t*  )&LOGICTask_Handler);
    //����PID����
    xTaskCreate((TaskFunction_t )PID_task,
                (const char*    )"PID_task",
                (uint16_t       )PID_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )PID_TASK_PRIO,
                (TaskHandle_t*  )&PIDTask_Handler);

    //����DATA����
    xTaskCreate((TaskFunction_t )DATA_task,
                (const char*    )"DATA_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask_Handler);

    //����DATA1����
    xTaskCreate((TaskFunction_t )DATA1_task,
                (const char*    )"DATA1_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask1_Handler);

    //����DATA2����
    xTaskCreate((TaskFunction_t )DATA2_task,
                (const char*    )"DATA2_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask2_Handler);

    //����DATA3����
    xTaskCreate((TaskFunction_t )DATA3_task,
                (const char*    )"DATA3_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask3_Handler);

    //����DATA4����
    xTaskCreate((TaskFunction_t )DATA4_task,
                (const char*    )"DATA4_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask4_Handler);

    //����DATA5����
    xTaskCreate((TaskFunction_t )DATA5_task,
                (const char*    )"DATA5_task",
                (uint16_t       )DATA_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )DATA_TASK_PRIO,
                (TaskHandle_t*  )&DATATask5_Handler);

    vTaskDelete(StartTask_Handler);
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LOGIC������
[[noreturn]] void LOGIC_task(void *pvParameters)
{
    static bool LL_init_f= false;
    while(true)
    {
        delay_ms(200);

        MLED.OLED_SSD1306::Queue_star();
        MLED.Print(0,0,"%.2lf   %.2lf",LL.LiuYa,LL.JiYa);
        MLED.Print(0,2,"%.2lf   %.2lf",LL.JiWen,LL.DaQiWen);

        MLED.Print(0,4,"%.2lf",LL.DaQiYa);

        MLED.Print(0,6,"%.2lf",LL.cur);
        MLED.OLED_SSD1306::Queue_end();


        m_modebus.data_BUS.to_float.stove_temp_r=stovectrl.get_cur();
        m_modebus.data_BUS.to_float.Flow_value_r=LL.cur;
        m_modebus.data_BUS.to_float.air_temp=Atmospheric_T.get_temp_cache();

        m_modebus.data_BUS.to_float.Flow_coefficient=LL.FLOW_RATE;


        if(m_modebus.data_BUS.to_float.stove_work==1){
            stovectrl.turn_ON();
        }
        else
        {
            stovectrl.turn_OFF();
        }

        if(m_modebus.data_BUS.to_float.Flow_work==1){
            LL.set_target(m_modebus.data_BUS.to_float.Flow_value_s);
            LL.TURN_ON();
            if(!LL_init_f)
            {
                LL_init_f= true;
                JY.calculate_pres_offset(0);
                LY.calculate_pres_offset(0);
            }
            if(m_modebus.data_BUS.to_float.Flow_value_w>0){
                LL.FLOW_RATE_change(m_modebus.data_BUS.to_float.Flow_value_w);
                m_modebus.data_BUS.to_float.Flow_value_w=0;
                m_modebus.data_BUS.to_float.Flow_coefficient=LL.FLOW_RATE;
                power_data.write(0,m_modebus.data_BUS.to_u8t,sizeof(m_modebus.data_BUS));  //д������
            }
        }

        else
        {
            LL.TURN_OFF();
        }

    }
}

//PID������
[[noreturn]] void PID_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(500);
        stovectrl.upset();
        LL.upset();
    }
}

//DATA������
[[noreturn]] void DATA_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(20);
        LL.calculation_hole_flow();
    }
}

//DATA1������
[[noreturn]] void DATA1_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.LiuYa_data_upset();
    }
}

//DATA2������
[[noreturn]] void DATA2_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.JiYa_data_upset();
    }
}

//DATA3������
[[noreturn]] void DATA3_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.DaQiYa_data_upset();
    }
}

//DATA4������
[[noreturn]] void DATA4_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.JiWen_data_upset();
    }
}

//DATA5������
[[noreturn]] void DATA5_task(void *pvParameters)
{
    while(true)
    {
        delay_ms(10);
        LL.DaQiWen_data_upset();
    }
}
