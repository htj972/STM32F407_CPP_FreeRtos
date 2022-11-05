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
#include "MAX31865.h"
#include "PWM.h"
#include "Temp_ctrl.h"
#include "SD_CARD.h"

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
SPI_S           MSPI(GPIOC0,GPIOC2,GPIOC3);
MAX31865        M865(&MSPI,GPIOC1);
PWM_H           MPWM(TIM10,100);
Temp_ctrl       MCTRL(&M865,&MPWM,1);
SPI             spi2(SPI2);
SD_CARD         SD1(&spi2,GPIOB12);

std::string asdasd="123456";


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
    MPWM.config(1);

    MCTRL.PID_ENABLE_Range(1);
    MCTRL.init();
    MCTRL.set_target(40);

    MSPI.config(SPI_S::CP::OL_0_HA_0);
    M865.init();
    M865.config(MAX31865::PT100,390);

//    spi2.config(GPIOB13,GPIOB14,GPIOB15);
//    spi2.init(SPI2);
    spi2.ReadWriteDATA(0xff);
    uint8_t ret=SD1.init();
    while(ret)//��ⲻ��SD��
    {
        MOLED.Print(0,0,"r:%x",ret);
        ret=SD1.init();
        delay_ms(200);
        led.change();
    }
    MOLED.Print(0,2,"S:%d",SD1.GetSectorCount());//�õ�������
    SD1.writestr(5,asdasd);
    uint8_t data[20];
    SD1.read(5,data,asdasd.length());
    MOLED.Print(0,0,"D:%s",data);

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
        vTaskDelay(250/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        if(sec_t!=time1.get_sec())
        {
            led.change();
            sec_t=time1.get_sec();
            MOLED.Queue_star();

            float T=2.5,H=3.4;
            SHT35.get_temp_humi(&T,&H);
            MOLED.Print(0,4,"T:%3.1lf H:%3.1lf",T,H);

            MOLED.Print(0,6,"T:%4.1lf",M865.get_sensor_temp());
//            MOLED.Print(0,6,"%02d:%02d:%02d",time1.get_hour(),time1.get_min(),sec_t);

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
        vTaskDelay(200/portTICK_RATE_MS );

        MCTRL.upset();

        U1<<U1.read_data();
        MOLED.Queue_star();
        //MOLED.Print(0,0,"%04d",task2_num);
        MOLED.Queue_end();
    }
}


