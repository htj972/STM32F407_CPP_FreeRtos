#include "sys.h"
#include "delay.h"
#include "USART.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "OLED_SSD1306.h"
#include "W25QXX.h"
#include "FM24Cxx.h"
#include "RTC_DS3231.h"

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
#define TASK1_STK_SIZE 		128
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
Software_IIC    SIIC1(GPIOD0,GPIOD1);
OLED_SSD1306    MOLED(&SIIC1,2);
Software_IIC    SIIC2(GPIOE4,GPIOE5);
FM24Cxx         FM24C64(&SIIC2);
Software_IIC    SIIC3(GPIOE2,GPIOE3);
RTC_DS32xx      time1(&SIIC3);


std::string asdasd="123456";
std::string eprom="FM24C64 text";
int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4
    delay_init(168);					//��ʼ����ʱ����

    //U1.set_send_DMA();
    U1.write("adsda321s3a1d3a1sd3sd\r\n");
    U1.write((uint8_t *)asdasd.data(),5);
    FM24C64.init();
    FM24C64.writestr(0,eprom);
    std::string text;
    FM24C64.readstr(0,&text,eprom.length());
    MOLED.init();
    MOLED.Fill(0xff);
    delay_ms(1000);
    MOLED.Fill(0x00);
    time1.set_date(2022,10,20);
    time1.set_time(11,12,15);
    MOLED.Print(0,2,"%d",time1.get_week());
    MOLED.Print(0,4,"%d",time1.get_name());
    //MOLED.Print(0,4,time1.get_name_str());
    MOLED.Print(0,6,time1.get_name_str());
//    QueueHandle_t xQueue1 = nullptr;
//    {
//
//        xSemaphoreTake(xQueue1,portMAX_DELAY);
//        xSemaphoreGive(xQueue1);
//    }

    //asdas.writestr(151,&asdasd,125);

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
[[noreturn]] void task1_task(void *pvParameters)
{
    u8 task1_num=0;
    while(true)
    {
        task1_num++;					//����1ִ�д�����1 ע��task1_num1�ӵ�255��ʱ������㣡��
        taskENTER_CRITICAL();           //�����ٽ���
        taskEXIT_CRITICAL();            //�˳��ٽ���
        led.change();
        vTaskDelay(500/portTICK_RATE_MS );			//��ʱ10ms��ģ����������10ms���˺������������������
        //U1<<"sadasd:"<<15<<asdd;
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
        MOLED.Print(0,0,"%04d",task2_num);
    }
}


