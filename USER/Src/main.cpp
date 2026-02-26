#include "sys.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Out_In_Put.h"
#include "Timer.h"
#include "USART.h"
#include "malloc.h"
#include "Gateway.h"
#include "Communication.h"
#include "WDG.h"
#include "lwip_comm/lwip_comm.h"
#include "HC595.h"
#include "HC165.h"
#include "SPI.h"
#include "W25QXX.h"
#include "Storage_Link.h"
#include "ADC.h"
#include "ModbusTcp.h"
#include "tcp_server/TCP_Server.h"
#include "USER_Register.h"
#include "tcp_server/TCP_Server_App.h"


//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小
#define START_STK_SIZE 		64
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define LOGIC_TASK_PRIO		2
//任务堆栈大小
#define LOGIC_STK_SIZE 		(128)
//任务句柄
TaskHandle_t LOGICTask_Handler;
//任务函数
[[noreturn]] void LOGIC_task(void *pvParameters);

//任务优先级
#define IO_TASK_PRIO		2
//任务堆栈大小
#define IO_STK_SIZE 		(128)
//任务句柄
TaskHandle_t IOTask_Handler;
//任务函数
[[noreturn]] void IO_task(void *pvParameters);

//任务优先级
#define Net_TASK_PRIO		1
//任务堆栈大小
#define Net_STK_SIZE 		(128*4)
//任务句柄
TaskHandle_t NetTask_Handler;
//任务函数
[[noreturn]] void Net_task(void *pvParameters);
uint16_t tims=0;
//运行指示灯
class T_led_:public _OutPut_,public Call_Back,public Timer{
private:
    bool mode=false;
public:
    T_led_(GPIO_Pin param,TIM_TypeDef *TIMx, uint16_t frq) {
        _OutPut_::init(param,LOW);
        Timer::init(TIMx,10000/frq,8400,true);
        this->Timer::upload_extern_fun(this);
    }
    void set_mode(bool modex){
        this->mode=modex;
    }
    void Callback(int  ,char** ) override{
        if(mode)this->change();
        Feed_Dog();
        tims++;
    };
}led(GPIOD11,TIM6,2);//运行指示灯定时器

class lwip_:public Timer,public Call_Back{
public:
    lwip_(TIM_TypeDef *TIMx, uint16_t frq) {
        Timer::init(TIMx,10000/frq,8400, false);
        this->Timer::upload_extern_fun(this);
    }
    void Callback(int  ,char** ) override {
        lwip_setup();
    }
}lwipw(TIM5,100);//运行指示灯定时器

_OutPut_ error_led (GPIOD10);//运行指示灯

_OutPut_ TrmLED[6]={
        _OutPut_(GPIOE2,HIGH),
        _OutPut_(GPIOE6,HIGH),
        _OutPut_(GPIOE4,HIGH),
        _OutPut_(GPIOE3,HIGH),
        _OutPut_(GPIOC13,HIGH),
        _OutPut_(GPIOE5,HIGH)
};

_USART_ Debug(USART1,115200);
HC595 OUT_driver(GPIOC6,GPIOD14,GPIOD13,GPIOD15,GPIOD12,2);//74HC595驱动
HC165 IN_driver(GPIOE12,GPIOE13,GPIOE14,GPIOE15,2);//74HC165驱动
_ADC_ ADC1_driver(ADC1,4),ADC2_driver(ADC1,5),
         ADC3_driver(ADC1,8),ADC4_driver(ADC1,9);//ADC驱动
Software_IIC IIC(GPIOE0,GPIOE1);//硬件IIC1
FM24Cxx eeprom(&IIC,FM24Cxx::AT24C16);//FM24C16驱动
SPI SPI1_driver(SPI1,GPIOB3,GPIOB4,GPIOB5);//硬件SPI1
W25QXX W25Q(&SPI1_driver,GPIOD7);
Storage_Link flash_fatfs(&W25Q);
RS485 RS485A(USART3,GPIOD8,GPIOD9,GPIOB15);//RS485A驱动
RS485 RS485B(USART2,GPIOD5,GPIOD6,GPIOD4);//RS485A驱动
_InPut_ INput1(GPIOE10),INput2(GPIOE9);

ModbusTCP MDTCP;
auto& kokirika = USER_Register::instance(SRAMCCM);
// auto& srv = TcpServer::instance();
auto& srv = TcpServer_APP::instance();

int main()
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
    //WDG_Init();
    delay_init(168);	//初始化延时函数
    delay_ms(1000);//延时1s

    OUT_driver.init();
    IN_driver.init();
    IN_driver.set_shift(1,(const char[]){0,1,2,3,7,6,5,4});
    IN_driver.set_shift(0,(const char[]){4,5,6,7,3,2,1,0});
    OUT_driver.set_shift(1,(const char[]){7,6,5,4,3,2,1,0});
    my_mem_init(SRAMIN);		//初始化内部内存池
    my_mem_init(SRAMCCM);		//初始化内部内存池
    led.set_mode(true);
    kokirika.init(128, 18, 12, 128);//初始化寄存器区

    //绑定线圈,范围:线圈0~127 0x00-0x007F 0~16为物理输出 0x00-0x0F 17~127为虚拟输出 0x10-0x7F
    MDTCP.bindCoils(kokirika.getCoils(), kokirika.getCoilCount());
    //绑定离散输入,范围:离散输入0~18 0x0000-0x000F 0~15为物理输入 0x00-0x0F 16~18为高频输入 0x10-0x12
    MDTCP.bindDiscrete(kokirika.getDiscrete(), kokirika.getDiscreteCount());
    //绑定输入寄存器,范围:输入寄存器0~12 0x0000-0x000B 0~8为ADC输入 0x00-0x08 9~12为高频输入 0x09-0x0B
    MDTCP.bindInput(kokirika.getInput(), kokirika.getInputCount());
    //绑定保持寄存器,范围:保持寄存器0~127 0x0000-0x007F
    MDTCP.bindHoldingRam(0, kokirika.getHolding(), kokirika.getHoldingCount());
    //绑定保持寄存器到eeprom 1024，范围:保持寄存器128~1151 0x0080-0x047F
    MDTCP.bindHoldingStorage(kokirika.getHoldingCount(), &eeprom,
                             0,(FM24Cxx::AT24C16+1)/2);
    kokirika.Add_Pulse_Pin(&INput1);//上传脉冲输入引脚1
    kokirika.Add_Pulse_Pin(&INput2);//上传脉冲输入引脚2


    W25Q.init();
    flash_fatfs.init();
    // Debug<<"W25Q ID:"<<W25Q.GetID()<<"\r\n";
    // char dasda[50];

    // sprintf(dasda,"qwe%d.txt",1);
    // W25Q.write(0,(uint8_t*)dasda,strlen(dasda));

    // char read_buf1[50]{};
    // W25Q.read(0,(uint8_t*)read_buf1,8);
    // Debug.print("read flash:%s\r\n",read_buf1);
    // Debug.print("flash init %d! disk:%s\r\n",flash_fatfs.init(),flash_fatfs.get_name());

    // FATFS fs1;
    // if(FR_OK!=f_mount(&fs1,"0:",1))
    //     Debug<<"Flash mount failed!\r\n";
    // f_mkfs(flash_fatfs.get_name(),1,4096);
    // //获取文件系统信息
    // uint32_t  filetotal,filefree;
    // Storage_Link::exf_getfree((uint8_t*)flash_fatfs.get_name(),&filetotal,&filefree);

//     Debug.print("Total Size:%d MB  Free Size:%d MB\r\n",filetotal<<10,filefree<<10);
// //    char dasda[50];
//     sprintf(dasda,"qwe%d.txt",1);
//     Debug<<"1\r\n";
//
//     while (f_open(&flash_fatfs.fp,flash_fatfs.setdir(dasda),FA_WRITE | FA_OPEN_ALWAYS) != FR_OK);
//     Debug<<"2\r\n";
//     f_lseek(&flash_fatfs.fp,flash_fatfs.fp.fsize);                                                                        //??????±ê????????
//     f_write(&flash_fatfs.fp, dasda, strlen(dasda), &flash_fatfs.plen);
//     f_close(&flash_fatfs.fp);
//     Debug<<"3\r\n";
//     while(f_open(&flash_fatfs.fp,flash_fatfs.setdir(dasda),FA_READ) != FR_OK);
//     char read_buf[50]{};
//     f_read(&flash_fatfs.fp, read_buf, strlen(dasda), &flash_fatfs.plen);
//     f_close(&flash_fatfs.fp);





    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,          //任务函数
                (const char*    )"start_task",           //任务名称
                (uint16_t       )START_STK_SIZE,     //任务堆栈大小
                (void*          )nullptr,            //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )LOGIC_task,
                (const char*    )"LOGIC_task",
                (uint16_t       )LOGIC_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )LOGIC_TASK_PRIO,
                (TaskHandle_t*  )&LOGICTask_Handler);
    //创建TASK2任务
    xTaskCreate((TaskFunction_t )IO_task,
                (const char*    )"IO_task",
                (uint16_t       )IO_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )IO_TASK_PRIO,
                (TaskHandle_t*  )&IOTask_Handler);

    //创建TASK3任务
    xTaskCreate((TaskFunction_t )Net_task,
                (const char*    )"Net_task",
                (uint16_t       )Net_STK_SIZE,
                (void*          )nullptr,
                (UBaseType_t    )Net_TASK_PRIO,
                (TaskHandle_t*  )&NetTask_Handler);
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//task1任务函数
[[noreturn]] void LOGIC_task(void *pvParameters)//alignas(8)
{
    uint8_t ii=0;
    //RS485A.set_send_DMA();
    //RS485B.set_send_DMA();
    while(true)
    {
        delay_ms(1000);
        error_led.change();
        TrmLED[ii++].change();
        if(ii>=6)ii=0;
//        Debug.print("ADC1:%4.2lf ADC2:%4.2lf  ADC3:%4.2lf ADC4:%4.2lf\r\n",
//                    ADC1_driver.Get_v_value(),
//                    ADC2_driver.Get_v_value(),
//                    ADC3_driver.Get_v_value(),
//                    ADC4_driver.Get_v_value());

        RS485A<<"KOKIRIKA A RS485 TEST\r\n";
        RS485B<<"KOKIRIKA B RS485 TEST\r\n";

    }
}

//task1任务函数
[[noreturn]] void IO_task(void *pvParameters)//alignas(8)
{
    while(true)
    {
        delay_ms(50);
        OUT_driver.Set_Hex(kokirika.getCoils());
        IN_driver.upset();
        IN_driver.Get_input(kokirika.getDiscrete());
    }
}

#define MD_Debug 1
//task3任务函数
[[noreturn]] void Net_task(void *pvParameters)
{
    lwipw.set_NVIC(true);
#if MD_Debug
    {
        Debug<<"lwIP Initing...\r\n";
        while(lwip_comm_init()!=0)
        {
            Debug<<"lwIP Init failed!\r\n";
            delay_ms(1200);
            Debug<<"Retrying...\r\n";
        }
        Debug<<"lwIP Init Successed\r\n";
        netinit:
        //等待DHCP获取
        Debug<<"DHCP IP configing...\r\n";
        while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
        {
            lwip_periodic_handle();
        }
        uint8_t speed;
        Debug<<DHCP_str[0]<<DHCP_str[1]<<DHCP_str[2]<<DHCP_str[3]<<DHCP_str[4];
        speed=LAN8720_Get_Speed();//得到网速
        if(speed&1<<1)Debug<<"Ethernet Speed:100M\r\n";
        else Debug<<"Ethernet Speed:10M\r\n";
    }
    srv.init();

#else
    while(lwip_comm_init()!=0)
    {
        delay_ms(1200);
    }
    netinit:
    while((lwipdev.dhcpstatus!=2)&&(lwipdev.dhcpstatus!=0XFF))//等待DHCP获取成功/超时溢出
    {
        lwip_periodic_handle();
    }
#endif
    while(true) {
        //delay_ms(50);
        if(CheckLinkStatus()!=1) {
            srv.shutdown();
            tcp_abort_all();
            lwip_net_close();
            lwip_net_open();
            goto netinit;
        }
        srv.pollOnce(MDTCP);
        vTaskDelay(1);
    }
}


