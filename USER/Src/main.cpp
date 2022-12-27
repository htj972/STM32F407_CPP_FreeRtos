#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "IIC.h"
#include "Communication.h"
#include "MAX31865.h"
#include "DW_DIS.h"


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

////IIC����
//Software_IIC IIC_BUS(GPIOB8,GPIOB9);
////����ѹ������
//MS5805  Atmospheric_P(&IIC_BUS);
////EEPROM ���ݴ洢
//FM24Cxx power_data(&IIC_BUS);

////SPI����
//SPI_S SPI_BUS(GPIOE5,GPIOE3,GPIOE6);
////�����¶ȴ�����
//MAX31865 Atmospheric_T(&SPI_BUS,GPIOE4);

//ͨ�������
Communication m_modebus(USART3,GPIOE15,TIM5,100);

_OutPut_    led(GPIOE6);

SPI         spi1(SPI1);
MAX31865    temp1(&spi1,GPIOC0);
MAX31865    temp2(&spi1,GPIOC1);
MAX31865    temp3(&spi1,GPIOC2);
MAX31865    temp4(&spi1,GPIOC3);
MAX31865    temp5(&spi1,GPIOA0);

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
}led2(GPIOE5,TIM6,10);

DW_DIS MDW(USART6,TIM7,10);




int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);	//��ʼ����ʱ����

    temp1.init();
    temp1.config(MAX31865::MODE::PT1000,2000);
    temp2.init();
    temp2.config(MAX31865::MODE::PT1000,2000);
    temp3.init();
    temp3.config(MAX31865::MODE::PT1000,2000);
    temp4.init();
    temp4.config(MAX31865::MODE::PT1000,2000);
    temp5.init();
    temp5.config(MAX31865::MODE::PT1000,2000);

    MDW.SetBackLight(20);
    MDW.set_dis_sleep_time(15);
    MDW.init();



//    power_data.init();          //���ݴ洢��ʼ��
    //��ȡ��������
//    power_data.read(0,m_modebus.data_BUS.to_u8t,sizeof(m_modebus.data_BUS));
    //����������ȷ��  ͷ���ϵ磨�汾�ţ�
//    if(m_modebus.data_BUS.to_float.version-HARD_version!=0){
//        K_POWER_DATA init_data;             //��ʵ��һ�������� ���췽�����ó�ʼ��ֵ
//        m_modebus.data_BUS=init_data;       //ת�����ݵ�ʹ�ýṹ��
//        power_data.write(0,init_data.to_u8t,sizeof(init_data));  //д������
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
    uint8_t sec_t=0;
    while(true)
    {
        vTaskDelay(1000/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������


    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    static uint8_t flag=0;
    uint8_t ret=0;
    while(true)
    {
        vTaskDelay(100/portTICK_RATE_MS );
        MDW.key_handle();
        MDW.Dis_handle();
        if(MDW.get_curInterface()==5)
        {
            if(flag==0)
                ret=m_modebus.modbus_03_send(0,2);
            flag=1;
            MDW.vspf_Text(0X1000,(char*)"%02x - %lf",ret,m_modebus.data_BUS.to_float.version);
        }
    }
}


