#include "sys.h"
#include "delay.h"
#include "USART.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "OLED_SSD1306.h"
#include "FM24Cxx.h"
#include "RTC_DS3231.h"
#include "SHT3x.h"
#include "MS5805.h"
#include "DS18B20.h"
#include "MAX31865.h"
#include "Timer.h"

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
#define TASK2_STK_SIZE 		128
//������
TaskHandle_t Task2Task_Handler;
//������
[[noreturn]] void task2_task(void *pvParameters);

_OutPut_        led(GPIOE6);
_USART_         U1(USART1,115200);
Software_IIC    SIIC1(GPIOB4,GPIOB5);
OLED_SSD1306    MOLED(&SIIC1,OLED_SSD1306::Queue::OWN_Queue,2);
Software_IIC    SIIC3(GPIOE2,GPIOE3);
RTC_DS32xx      time1(&SIIC3);
Software_IIC    SIIC4(GPIOD1,GPIOD0);
SHT3x           SHT35(&SIIC4);
Software_IIC    SIIC5(GPIOD11,GPIOD10);
MS5805          MMS5805(&SIIC5);
DS18B20         DHT11(GPIOD7);
SPI_S           MSPI(GPIOC0,GPIOC2,GPIOC3);
MAX31865        M865(&MSPI,GPIOC1);
Timer           MTIM(TIM2,5000-1,8400-1);

std::string asdasd="123456";

void asda()
{
    led.change();
}

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);					//��ʼ����ʱ����

    U1.write((uint8_t *)asdasd.data(),5);
    MOLED.init();
    MOLED.Fill(0xff);
    delay_ms(1000);
    MOLED.Fill(0x00);
    SHT35.init();
    MMS5805.init();
    DHT11.init();

    MTIM.upload_extern_fun(asda);

    MTIM.Timer_extern_fun(std::bind(&_OutPut_::change, &led));

    MSPI.config(SPI_S::CP::OL_0_HA_0);
    M865.init();
    M865.config(MAX31865::PT100,390);
    MTIM.set_NVIC(true);

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
        taskENTER_CRITICAL();           //�����ٽ���
        taskEXIT_CRITICAL();            //�˳��ٽ���
        vTaskDelay(250/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        if(sec_t!=time1.get_sec())
        {
//            U4.println("%4d-%2d-%2d",time1.get_year(),time1.get_month(),time1.get_day());
//            U4.println("%2d:%2d:%2d",time1.get_hour(),time1.get_min(),time1.get_sec());
            sec_t=time1.get_sec();
            MOLED.Queue_star();
            float MT=2.5,MP=3.4;
            MMS5805.get_temp_pres(&MT,&MP);
            MOLED.Print(0,2,"T:%3.1lf P:%5.1f",MT,MP);

            float T=2.5,H=3.4;
            SHT35.get_temp_humi(&T,&H);
            MOLED.Print(0,4,"T:%3.1lf H:%3.1lf",T,H);


            MOLED.Print(0,6,"T:%4.1lf",M865.get_sensor_temp());
//            MOLED.Print(0,6,"%02d:%02d:%02d",time1.get_hour(),time1.get_min(),sec_t);

            MOLED.Print(0,0,"T:%4.1lf",DHT11.get_sensor_temp());

            MOLED.Queue_end();
        }
    }
}

//task2������
[[noreturn]] void task2_task(void *pvParameters)
{
    uint16_t task2_num=0;
    while(true)
    {
        task2_num++;					//����2ִ�д�����1 ע��task2_num1�ӵ�255��ʱ������㣡��
        taskENTER_CRITICAL();           //�����ٽ���
        taskEXIT_CRITICAL();            //�˳��ٽ���
        vTaskDelay(200/portTICK_RATE_MS );
        U1<<U1.read_data();
        MOLED.Queue_star();
        //MOLED.Print(0,0,"%04d",task2_num);
        MOLED.Queue_end();
    }
}


