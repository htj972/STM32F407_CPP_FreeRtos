/**
* @Author kokirika
* @Name DW_LCD
* @Date 2022-12-09
**/

#include <cstdarg>
#include <cstring>
#include "DW_LCD.h"
/*!
 * 配置通道 无需init
 * @param USART 串口指针
 * @param head 数据帧头
 */
DW_LCD::DW_LCD(_USART_ *USART,uint8_t head1,uint8_t head2) {
    this->init(USART,head1,head2);
}
/*!
 * 配置通道
 * @param USART 串口指针
 * @param head 数据帧头
 */
void DW_LCD::init(_USART_ *USART, uint8_t head1, uint8_t head2) {
    this->USARTX=USART;
    this->dis_protect_time=0;
    this->sleep_flag= false;
    this->head_address[0]=head1;
    this->head_address[1]=head2;
    this->uart_get_len=0;
    this->ret_key=false;
    this->set_Queue_mode(OWN_Queue);
    this->USARTX->upload_extern_fun(this);
}
/*!
 * 触摸屏  界面切换  函数
 * @param Interface 界面号
 * @param head 数据帧头
 * @return void
 */
void DW_LCD::Interface_switching(uint16_t Interface,bool save)
{
    uint8_t LcdOrder[7] = {this->head_address[0],this->head_address[1],0x04,0x80,0x03,0x00,0x00};
    this->Queue_star();
    if(save)this->curInterface = Interface;        //系统界面状态标注
    LcdOrder[5] = (Interface >> 8) & 0x00ff;
    LcdOrder[6] = Interface & 0x00ff;
    this->USARTX->write((uint8_t*)LcdOrder,7);
    this->Queue_end();
}
/*!
 * 触摸屏  数据输出
 * @param address 地址
 * @param date     数据
 * @param decimal
 * @return void
 */
void DW_LCD::Data_Output(uint16_t address,float date,uint8_t decimal)
{
    volatile uint8_t LcdOrder[10] = {this->head_address[0],this->head_address[1],0x07,0x82,0x00,0x00,0x00,0x00,0x00,0x00,};
    this->Queue_star();
    LcdOrder[4] = (address >> 8) & 0x00ff;
    LcdOrder[5] = address & 0x00ff;
    this->USARTX->write((uint8_t*)LcdOrder,10);
    this->Queue_end();
}
/*!
 * 触摸屏  格式化输出
 * @param address 地址
 * @param fmt     格式
 * @param ...     数据
 * @return 数据长度
 */
int DW_LCD::vspf_Text(uint16_t address,char *fmt, ...)//函数功能： 字符串不定参数打印，串口输出 函数；
{
    char buffer[128];
    va_list argptr;     //va_list  包含在 #include <stdarg.h>    2019.03.23
    int cnt;
    va_start(argptr, fmt);
    cnt = vsprintf(&buffer[6], fmt, argptr);
    va_end(argptr);
    Text_Output(address,(char *)buffer);
    return cnt;
}
/*!
 * 触摸屏  文本数据使出
 * @param address 地址
 * @param p       数据
 * @return
 */
void DW_LCD::Text_Output(uint16_t address,char *p)
{
    volatile uint8_t  Length ;
    this->Queue_star();
    Length = strlen(&p[6]); //获得字符串长度
    p[0] = this->head_address[0];
    p[1] = this->head_address[1];
    p[2] = Length + 3;
    p[3] = 0x82;
    p[4] = (address >> 8) & 0x00ff;
    p[5] = address & 0x00ff;
    this->USARTX->write((uint8_t*)p,p[2] + 3);
    this->Queue_end();
}
/*!
 * 触摸屏  图片剪切粘贴函数
 * @param address 地址
 * @param PictureID    图片所在 ID
 * @param IDs_x  IDs_y  图片 开始坐标
 * @param IDe_x  IDe_y  图片 结束坐标
 * @param x      Y      图片 粘贴开始坐标
 * @return
 */
void DW_LCD::Picture_cut(uint16_t address,uint16_t PictureID,uint16_t IDs_x,uint16_t IDs_y,uint16_t IDe_x,uint16_t IDe_y,uint16_t x,uint16_t y)
{
    volatile uint8_t LcdOrder[24] = {this->head_address[0],this->head_address[1],0x15,0x82,0x00,0x00,0x00,0x06,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,};
    this->Queue_star();
    LcdOrder[4] = (address >> 8) & 0x00ff;
    LcdOrder[5] = address & 0x00ff;

    LcdOrder[10] = (PictureID >> 8) & 0x00ff;   //剪切图片所在 ID
    LcdOrder[11] = PictureID & 0x00ff;

    LcdOrder[12] = (IDs_x >> 8) & 0x00ff;      // IDs_x   剪切图片开始坐标 x
    LcdOrder[13] = IDs_x & 0x00ff;

    LcdOrder[14] = (IDs_y >> 8) & 0x00ff;      // IDs_y   剪切图片开始坐标 y
    LcdOrder[15] = IDs_y & 0x00ff;

    LcdOrder[16] = (IDe_x >> 8) & 0x00ff;      // IDe_x   剪切图片结束坐标 x
    LcdOrder[17] = IDe_x & 0x00ff;

    LcdOrder[18] = (IDe_y >> 8) & 0x00ff;      // IDe_y   剪切图片结束坐标 y
    LcdOrder[19] = IDe_y & 0x00ff;

    LcdOrder[20] = (x >> 8) & 0x00ff;          // x   显示位置坐标 x
    LcdOrder[21] = x & 0x00ff;

    LcdOrder[22] = (y >> 8) & 0x00ff;          // y   显示位置坐标 y
    LcdOrder[23] = y & 0x00ff;

    this->USARTX->write((uint8_t*)LcdOrder,LcdOrder[2] + 3);
    //if(g.BacklightMake != 0)                 //背光关闭时不在上传数据	DW_LCD_strut.usart_->data_send((uint8_t*)LcdOrder,24);
    this->Queue_end();
}
/*!
 * 触摸屏  设置进度条进度 图片ID为 +1
 * @param address 地址
 * @param IDs_x,IDs_y    图标开始坐标
 * @param IDe_x,IDe_y    图标结束坐标
 * @return
 */
void DW_LCD::set_Progress_bar(uint16_t address,uint8_t progress, uint16_t IDs_x, uint16_t IDs_y, uint16_t IDe_x, uint16_t IDe_y) {
    uint16_t end_x=(IDe_x-IDs_x)/100*progress+IDs_x;
    this->Picture_cut(address,this->curInterface+1,IDs_x,IDs_y,end_x,IDe_y,IDs_x,IDs_y);
}
/*!
 * 触摸屏  图标变量 放置函数
 * @param address 地址
 * @param IDNum    图标所在位置 ID
 * @return
 */
void DW_LCD::ICO_cut(uint16_t address,uint16_t IDNum)
{
    volatile uint8_t LcdOrder[8] = {this->head_address[0],this->head_address[1],0x05,0x82,0x00,0x00,0x00,0x00,};
    this->Queue_star();
    LcdOrder[4] = (address >> 8) & 0x00ff;
    LcdOrder[5] = address & 0x00ff;
    LcdOrder[6] = (IDNum >> 8) & 0x00ff;		 //  图标 所在 位置
    LcdOrder[7] = IDNum & 0x00ff;
    this->USARTX->write((uint8_t*)LcdOrder,8);
    this->Queue_end();
}
/*!
 * 触摸屏  命令弹出菜单函数
 * @param address 地址
 * @param InputNum    :目标弹出菜单编号
 * @return
 */
void DW_LCD::Keyboard_Up(uint8_t InputNum)
{
    volatile uint8_t LcdOrder[6] = {this->head_address[0],this->head_address[1],0x03,0x80,0x4F,0x01};
    this->Queue_star();
    LcdOrder[5] = InputNum;
    this->USARTX->write((uint8_t*)LcdOrder,6);
    this->Queue_end();
}
/*!
 * 触摸屏  发送RTC读取命令
 * @return
 */
void DW_LCD::RTC_Read()
{
    volatile uint8_t LcdOrder[6] = {this->head_address[0],this->head_address[1],0x03,0x81,0x20,0x10};
    this->Queue_star();
    this->USARTX->write((uint8_t*)LcdOrder,6);
    this->Queue_end();
}
/*!
 * 触摸屏  发送RTC写取命令
 * @param yer mon day 年月日
 * @param hou min sec 时分秒
 * @return
 */
void DW_LCD::RTC_write(uint8_t yer,uint8_t mon,uint8_t day,uint8_t hou,uint8_t min,uint8_t sec)
{
    volatile uint8_t LcdOrder[13] = {this->head_address[0],this->head_address[1],0x0A,0x80,0x1F,0x5A};
    this->Queue_star();
    LcdOrder[6]=((yer/10)<<4)+(yer%10);
    LcdOrder[7]=((mon/10)<<4)+(mon%10);
    LcdOrder[8]=((day/10)<<4)+(day%10);
    LcdOrder[9]=0;
    LcdOrder[10]=((hou/10)<<4)+(hou%10);
    LcdOrder[11]=((min/10)<<4)+(min%10);;
    LcdOrder[12]=((sec/10)<<4)+(sec%10);

    this->USARTX->write((uint8_t*)LcdOrder,13);
    this->Queue_end();
}
/*!
 * 触摸屏  发送RTC写 数据来源外部RTC
 * @return
 */
void DW_LCD::RTC_Write(){
    if(this->RTX!= nullptr)
    {
        uint16_t yer;
        uint8_t mon,day,hou,min,sec;
        this->RTX->get_date(&yer,&mon,&day);
        this->RTX->get_time(&hou,&min,&sec);
        volatile uint8_t LcdOrder[13] = {this->head_address[0],this->head_address[1],0x0A,0x80,0x1F,0x5A};
        yer%=100;
        this->Queue_star();
        LcdOrder[6]=((yer/10)<<4)+(yer%10);
        LcdOrder[7]=((mon/10)<<4)+(mon%10);
        LcdOrder[8]=((day/10)<<4)+(day%10);
        LcdOrder[9]=0;
        LcdOrder[10]=((hou/10)<<4)+(hou%10);
        LcdOrder[11]=((min/10)<<4)+(min%10);;
        LcdOrder[12]=((sec/10)<<4)+(sec%10);

        this->USARTX->write((uint8_t*)LcdOrder,13);
        this->Queue_end();
    }
}
/*!
 * 触摸屏  发送背光亮度
 * @param light 亮度 (0x00-0x40亮度范围)
 * @param save 保存
 * @return
 */
void DW_LCD::SetBackLight(uint8_t light,bool save)
{
    if(light>0x40)light=0x40;
    volatile uint8_t LcdOrder[6] = {this->head_address[0],this->head_address[1],0x03,0x80,0x01,0x01};
    this->Queue_star();
    LcdOrder[5] = light;
    this->USARTX->write((uint8_t*)LcdOrder,6);
    if(save)this->cur_light=light;
    this->Queue_end();
}
/*!
 * 触摸屏  睡眠 转跳屏保页 亮度0
 * @param Interface 屏保页面 默认0
 * @return
 */
void DW_LCD::Sleep(uint8_t Interface) {
    this->sleep_flag= true;
    this->SetBackLight(0, false);
    this->go_sleep_page= this->curInterface;
    this->Interface_switching(Interface);
}
/*!
 * 触摸屏  唤醒 转跳运行界面
 * @return
 */
void DW_LCD::Wake_up() {
    this->sleep_flag= false;
    this->SetBackLight(this->cur_light, false);
    if(this->go_sleep_page)
        this->Interface_switching(this->go_sleep_page);
    else
        this->Interface_switching(this->curInterface);
}
/*!
 * 触摸屏  曲线
 * @param ch:通道
 * @param X_scale:X轴在DMT的点阵数
 * @param Y_scale:Y轴在DMT的点阵数
 * @param Y_offset:Y轴偏移量
 * @param data_num:待显示数据数量
 * @param max_data:满量程值
 * @param curve_data:数据地址
 * @return
 */
void DW_LCD::DrawCurve(uint8_t ch, uint16_t X_scale, uint16_t Y_scale, int Y_offset, uint16_t data_num, u16 max_data, const u16 * curve_data)
{
    uint16_t tmp;
    uint16_t datapos;
    float datapos_tmp;
    uint8_t framelen;
    uint8_t command_buf[256];

    framelen = 0;
    datapos = 0;                /*从第0个数据开始显示*/
    datapos_tmp = 0.0f;
    this->Queue_star();
    while(true)
    {
        if(framelen < 200)
        {
            framelen += 2;
            tmp = *(curve_data + datapos);     /*装入数据*/
            datapos_tmp += ((float)data_num / (float)X_scale);
            datapos = (u16)datapos_tmp;
            tmp = Y_scale * tmp / max_data;
            tmp = tmp - Y_offset;               /*Y轴偏移*/
            command_buf[ 5 + framelen - 2] = (tmp >> 8) & 0x00FF;
            command_buf[ 5 + framelen - 2 + 1 ] = tmp & 0x00FF;
        }
        else
        {
            /*准备数据并且发送*/
            command_buf[0] = this->head_address[0];
            command_buf[1] = this->head_address[1];
            command_buf[2] = framelen + 2;          /*数据长度*/
            command_buf[3] = 0x84;                  /*指令*/
            command_buf[4] = (1 << ch);             /*通道*/
            this->USARTX->write((uint8_t*)command_buf,framelen + 2 + 3);
            framelen = 0;                           /*新的一帧数据准备*/
        }
        if(datapos >= data_num)                     /*已经到了数据结尾*/
        {
            if(framelen != 0)                       /*最后的数据*/
            {
                /*准备数据并且发送*/
                command_buf[0] = this->head_address[0];
                command_buf[1] = this->head_address[1];
                command_buf[2] = framelen + 2;          /*数据长度*/
                command_buf[3] = 0x84;                  /*指令*/
                command_buf[4] = (1 << ch);             /*通道*/
                this->USARTX->write((uint8_t*)command_buf,framelen + 2 + 3);
            }
            break;
        }
    }
    this->Queue_end();
}
/*!
 * 触摸屏  曲线
 * @param ch:通道
 * @param data_len:X轴在DMT的点阵数
 * @param curve_data:数据地址
 * @return
 */
void DW_LCD::Draw(uint8_t ch, uint16_t data_len,const uint16_t * curve_data)
{
    uint8_t command_buf[256];
    command_buf[0] = this->head_address[0];
    command_buf[1] = this->head_address[1];
    command_buf[2] = data_len*2 + 2;          /*数据长度*/
    command_buf[3] = 0x84;                    /*指令*/
    command_buf[4] = (0x01 << ch);            /*通道*/
    for(uint16_t ii=0;ii<data_len;ii++)
    {
        command_buf[5+ii*2]		=(curve_data[ii]>>8)&0x00ff;
        command_buf[5+ii*2+1]	=(curve_data[ii]>>0)&0x00ff;
    }
    this->USARTX->write((uint8_t*)command_buf,data_len*2 + 2 + 3);
}
/*!
 * 触摸屏  打点
 * @param ch:通道
 * @param data:数据
 * @return
 */
void DW_LCD::DROW_point(uint8_t ch,uint16_t data)
{
    uint8_t command_buf[7];
    command_buf[0] = this->head_address[0];
    command_buf[1] = this->head_address[1];
    command_buf[2] = 4;         		/*数据长度*/
    command_buf[3] = 0x84;         	/*指令*/
    command_buf[4] =(0x01 << ch);		/*通道*/
    command_buf[5] =(data>>8)&0x00ff;
    command_buf[6] =(data>>0)&0x00ff;
    this->USARTX->write((uint8_t*)command_buf,7);
}
/*!
 * 触摸屏  打多个点
 * @param ch:通道
 * @param len:数据长度
 * @param data:数据
 * @return
 */
void DW_LCD::DROW_point_set(uint8_t ch,uint8_t len,uint16_t data)
{
    uint8_t command_buf[256];
    if(len>124)len=124;
    command_buf[0] = this->head_address[0];
    command_buf[1] = this->head_address[1];
    command_buf[2] = len*2+2;         		/*数据长度*/
    command_buf[3] = 0x84;         	/*指令*/
    command_buf[4] =(0x01 << ch);		/*通道*/
    for(uint8_t ii=0;ii<len;ii++)
    {
        command_buf[5+ii*2] =(data>>8)&0x00ff;
        command_buf[6+ii*2] =(data>>0)&0x00ff;
    }
    this->USARTX->write((uint8_t*)command_buf,len*2+5);
}
/*!
 * 触摸屏  指针数据修改
 * @param point:指针
 * @param offset:偏移量
 * @param data:数据
 * @return
 */
void DW_LCD::Write_data(uint16_t point,uint8_t offset,uint16_t DATA)
{
    uint8_t command_buf[8];
    point+=offset;
    command_buf[0] = this->head_address[0];
    command_buf[1] = this->head_address[1];
    command_buf[2] = 5;         	/*数据长度*/
    command_buf[3] = 0x82;         	/*指令*/
    command_buf[4] =(point>>8)&0x00ff;
    command_buf[5] =(point>>0)&0x00ff;
    command_buf[6] =(DATA>>8)&0x00ff;
    command_buf[7] =(DATA>>0)&0x00ff;
    this->USARTX->write((uint8_t*)command_buf,8);
}
/*!
 * 数据可执行
 * @return bool 是否又可以执行数据
 */
#define DMT_G_DATA          this->fifo_data
#define DW_STR_Len 			DMT_G_DATA[2]
#define DW_function 		DMT_G_DATA[3]
/********************KEY*************************/
#define DW_Address_H 		DMT_G_DATA[4]
#define DW_Address_L 		DMT_G_DATA[5]
#define DW_DATA_Len 		DMT_G_DATA[6]
#define DW_DATA_High 		DMT_G_DATA[7]
#define DW_DATA_LoW 		DMT_G_DATA[8]
#define DW_DATA2_High 		DMT_G_DATA[9]
#define DW_DATA2_LoW 		DMT_G_DATA[10]
/********************REG*************************/
#define DW_REG_Address 	    DMT_G_DATA[4]
/*!
 * 接收数据可用
 * @return 可用状态
 */
bool DW_LCD::available()
{
    return (this->uart_get_len>=DW_STR_Len+3);
}
/*!
 * 触摸屏  按键指针返回
 */
void DW_LCD::key_back_value_point() {
    this->ret_key= true;
    this->ret_key_address=(DW_Address_H<<8)+DW_Address_L;
    this->ret_key_data=(DW_DATA_High<<8)+DW_DATA_LoW;
    this->ret_value_data=(float)((DW_DATA_High<<24)+(DW_DATA_LoW<<16)+(DW_DATA2_High<<8)+(DW_DATA2_LoW));
}
/*!
 * 触摸屏  寄存器hex转int
 */
uint8_t DW_LCD::hex_to_int(uint8_t hex)
{
    uint8_t dat;
    dat=(hex&0xf0)>>4;
    dat*=10;
    dat+=(hex&0x0f);
    return dat;
}
/*!
 * 触摸屏  寄存器指针返回
 */
void DW_LCD::register_back_value_point()
{
    #define time_data(x) hex_to_int(DMT_G_DATA[x])
    switch(DW_REG_Address)
    {
        case 0x20:										//RTC寄存器
            if(this->RTX!= nullptr)
            {
                this->RTX->set_time(time_data(10)-12,time_data(11),time_data(12));
                this->RTX->set_date(time_data(6),time_data(7),time_data(8));
            }
            break;
    }
}
/*!
 * 触摸屏  数据执行更新接口
 * @return 是否有数据待处理
 */
bool DW_LCD::setup() {
    if(this->available())
    {
        switch(DW_function)
        {
            case 0x83:										//按键
                key_back_value_point();
                break;
            case 0x81:										//寄存器
                register_back_value_point();
                break;
        }
        this->USARTX->clear();
        this->uart_get_len=0;
        return true;
    }
    return false;
}
/*!
 * 触摸屏  链接RTC时钟
 * @param RTCX:RTC类指针
 * @return
 */
void DW_LCD::RtC_link(RTCBASE *RTCX) {
    this->RTX = RTCX;
}
/*!
 * 触摸屏  链接Timer中断回调
 * @param timerx:定时器类指针
 * @return
 */
void DW_LCD::Timer_Link(Timer *timerx) {
    this->timer=timerx;
    this->timer->upload_extern_fun(this);
}
/*!
 * 获取按键地址  并清除按键状态
 * @return 按键地址
 */
uint16_t DW_LCD::get_key_address() {
    this->ret_key= false;
    return this->ret_key_address;
}
/*!
 * 获取按键值  并清除按键状态
 * @return 按键值
 */
uint16_t DW_LCD::get_key_data() {
    this->ret_key= false;
    return this->ret_key_data;
}
/*!
 * 获取数据值  并清除按键状态
 * @return 按键值
 */
float DW_LCD::get_value_data() {
    this->ret_key= false;
    return this->ret_value_data;
}
/*!
 * 获取按键状态  并清除按键状态
 * @return 状态
 */
bool DW_LCD::get_key_sata() {
    bool ret=this->ret_key;
    this->ret_key= false;
    return ret;
}
/*!
 * 设置屏幕保护时间 0为不息屏
 * @param x 息屏时间x次定时器中断时间（0为不息屏）
 */
void DW_LCD::set_dis_sleep_time(uint16_t x) {
    this->dis_protect_time=x;
}
/*!
 * 获取屏幕保护时间 0为不息屏
 * @return x 息屏时间x次定时器中断时间（0为不息屏）
 */
uint16_t DW_LCD::get_dis_sleep_time() const {
    return this->dis_protect_time;
}
/*!
 * 获取按当前页面
 * @return 页面号
 */
uint8_t DW_LCD::get_curInterface() const {
    return this->curInterface;
}
/*!
 * 获取按当前亮度
 * @return 亮度
 */
uint8_t DW_LCD::get_cur_light() const {
    return this->cur_light;
}
/*!
 * 中断回调
 */
void DW_LCD::Callback(int num ,char** data) {
    static uint8_t head_flag=0;
    static uint16_t TIMX_CALL_times=0;
    switch (data[0][0]) {
        case Call_Back::Name::uart:
//            if(data[1][0]==this->USARTX->get_USART_Num())
            this->uart_get_data = data[2][0];
            if(this->uart_get_data==this->head_address[0])
                head_flag=1;
            else if(head_flag==1)
            {
                if(this->uart_get_data==this->head_address[1])
                {
                    this->uart_get_len=1;
                    this->fifo_data[0]=this->head_address[0];
                }
                head_flag=0;
            }
            this->fifo_data[this->uart_get_len++]=this->uart_get_data;
            if(this->uart_get_len==sizeof (this->fifo_data))
                this->uart_get_len=0;
            break;
        case Call_Back::Name::timer:
            this->timer->set_Cmd(false);
            if(this->setup())
                TIMX_CALL_times=0;
            if((this->dis_protect_time>0)&&(!this->sleep_flag))
            {
                TIMX_CALL_times++;
                if(TIMX_CALL_times>=this->dis_protect_time){
                    TIMX_CALL_times=0;
                    this->Sleep();
                }
            }
            this->timer->set_Cmd(true);
            break;
    }

}





