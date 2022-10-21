/**
* @Author kokirika
* @Name IIC
* @Date 2022-09-15
**/

#include "IIC.h"
#include "delay.h"

void Software_IIC::SDA_config(GPIOMode_TypeDef Pin_mode) {
    this->SDA.set_pinmode(Pin_mode);
}

void Software_IIC::SDA_OUT(){
    this->SDA_config(GPIO_Mode_OUT);
}

void Software_IIC::SDA_IN(){
    this->SDA_config(GPIO_Mode_IN);
}

void Software_IIC::SDA_ON()	{
    this->SDA.set_output(HIGH);
}

void Software_IIC::SDA_OFF()	{
    this->SDA.set_output(LOW);
}

void Software_IIC::SCL_ON()	{
    this->SCL.set_output(HIGH);
}

void Software_IIC::SCL_OFF()	{
    this->SCL.set_output(LOW);
}

void Software_IIC::delay() const {
    if(this->dalay_mode==Software_IIC::usec)
        delay_us(this->delay_time);
    else
        delay_ms(this->delay_time);
}

Software_IIC::Software_IIC(GPIO_TypeDef *PORT_csl, uint32_t Pin_csl, GPIO_TypeDef *PORT_sda, uint32_t Pin_sda,Queue mode) {
    this->init(PORT_csl,Pin_csl,PORT_sda, Pin_sda,mode);
}

Software_IIC::Software_IIC(uint8_t Pin_Scl, uint8_t Pin_Sda,Queue mode) {
    this->init(Pin_Scl,Pin_Sda,mode);
}

void Software_IIC::init(GPIO_TypeDef *PORT_csl,uint32_t Pin_csl,GPIO_TypeDef *PORT_sda,uint32_t Pin_sda,Queue mode) {
    this->SCL.init(PORT_csl,Pin_csl,GPIO_Mode_OUT);
    this->SDA.init(PORT_sda,Pin_sda,GPIO_Mode_OUT);
    this->set_Queue_mode(mode);
}

void Software_IIC::init(uint8_t Pin_Scl, uint8_t Pin_Sda,Queue mode) {
    this->SCL.init(Pin_Scl,GPIO_Mode_OUT);
    this->SDA.init(Pin_Sda,GPIO_Mode_OUT);
    this->set_Queue_mode(mode);
}

void Software_IIC::config(uint16_t wait, uint16_t delay, uint8_t mode) {
    this->err_times=wait;
    this->delay_time=delay;
    this->dalay_mode=mode;
}

void Software_IIC::pin_config(GPIOOType_TypeDef Pin_mode) {
    this->SCL.set_OTYPE(Pin_mode);
    this->SDA.set_OTYPE(Pin_mode);
}

void Software_IIC::pin_config(GPIOOType_TypeDef SCL_mode, GPIOOType_TypeDef SDA_mode) {
    this->SCL.set_OTYPE(SCL_mode);
    this->SDA.set_OTYPE(SDA_mode);
}

void Software_IIC::set_err_time(uint16_t times) {
    this->err_times=times;
}

void Software_IIC::set_delay(uint16_t delay, uint8_t mode) {
    this->delay_time=delay;
    this->dalay_mode=mode;
}

void Software_IIC::Start() {
    this->SDA_OUT();     //sda线输出
    this->SDA_ON();
    this->SCL_ON();
    this->delay();
    this->SDA_OFF();	//开始：当CLK高时，DATA由高变低
    this->delay();
    this->SCL_OFF(); 	//钳住I2C总线，准备发送或接收数据
}

void Software_IIC::Stop() {
    this->SDA_OUT();    //sda线输出
    this->SCL_OFF();
    this->SDA_OFF(); 	//停止：当CLK高时，DATA由低变高
    this->delay();
    this->SCL_ON();
    this->SDA_ON();	    //发送I2C总线结束信号
    this->delay();
}
//等待应答信号到来
//返回值：false，接收应答失败
//       true，接收应答成功
bool Software_IIC::Wait_Ack() {
    uint8_t err_temp=this->err_times;
    this->SCL_OFF();
    this->SDA_IN();      //SDA设置为输入
    this->SDA_ON();
    this->SCL_ON();
    while(this->SDA.get_input())
    {
        err_temp--;
        if(err_temp>0)
        {
            this->Stop();
            return false;
        }
    }
    this->SCL_OFF(); 	//时钟输出0
    return true;
}

void Software_IIC::Ack() {
    this->SCL_OFF();
    this->SDA_OUT();
    this->SDA_OFF();
    this->delay();
    this->SCL_ON();
    this->delay();
    this->SCL_OFF();
}

void Software_IIC::NAck() {
    this->SCL_OFF();
    this->SDA_OUT();
    this->SDA_ON();
    this->delay();
    this->SCL_ON();
    this->delay();
    this->SCL_OFF();
}

void Software_IIC::Send_Byte(uint8_t data) {
    uint8_t t;
    this->SDA_OUT();
    this->SCL_OFF();//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {
        if(data&0x80)
            this->SDA_ON();
        else
            this->SDA_OFF();
        data<<=1;
        this->delay();
        this->SCL_ON();
        this->delay();
        this->SCL_OFF();
        this->delay();
    }
    this->SDA_ON();
}

uint8_t Software_IIC::Read_Byte(uint8_t ack) {
    unsigned char i,receive=0;
    this->SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
    {
        this->SCL_OFF();
        this->delay();
        this->SCL_ON();
        this->delay();
        receive<<=1;
        if(this->SDA.get_input())
            receive++;
        this->delay();
    }
    this->SCL_OFF();
    if (ack)
        this->Ack();//发送ACK
    else
        this->NAck(); //发送nACK
    return receive;
}

void Software_IIC::Write_One_Byte(u8 daddr,u8 addr,u8 data)
{
    this->Start();
    this->Send_Byte(daddr);
    this->Wait_Ack();
    this->Send_Byte(addr);
    this->Wait_Ack();
    this->Send_Byte(data);
    this->Wait_Ack();
    this->Stop();
}

uint8_t Software_IIC::Read_One_Byte(uint8_t daddr, uint8_t addr,uint8_t draddr)
{
    uint8_t ret;
    this->Start();
    this->Send_Byte(daddr);
    this->Wait_Ack();
    this->Send_Byte(addr);
    this->Wait_Ack();
    this->Start();
    this->Send_Byte(draddr);
    this->Wait_Ack();
    ret=this->Read_Byte(0);
    this->Stop();
    return ret;
}

uint8_t Software_IIC::Read_One_Byte(uint8_t daddr,uint8_t addr){
    return this->Read_One_Byte(daddr,addr,daddr+1);
}

bool Software_IIC::Read_Data(uint8_t daddr, uint8_t addr, uint8_t draddr, uint8_t *data, uint8_t len) {
    this->Start();
    this->Send_Byte(daddr);
    if(!this->Wait_Ack())goto false_label;
    this->Send_Byte(addr);
    if(!this->Wait_Ack())goto false_label;
    this->Start();
    this->Send_Byte(draddr);
    if(!this->Wait_Ack())goto false_label;
    for(uint8_t ii=0;ii<len;ii++)
        data[ii]=this->Read_Byte((ii==len-1)?0:1);
    this->Stop();
    return true;
    false_label:
    return false;
}

bool Software_IIC::Read_Data(uint8_t daddr, uint8_t addr, uint8_t *data, uint8_t len) {
    return this->Read_Data(daddr,addr,daddr+1,data,len);
}


/*******************************************************/

void HARD_IIC::config(GPIO_TypeDef *PORT_csl,uint32_t Pin_csl,GPIO_TypeDef *PORT_sda,uint32_t Pin_sda) {
    this->SCL.init(PORT_csl,Pin_csl,GPIO_Mode_AF);
    this->SDA.init(PORT_sda,Pin_sda,GPIO_Mode_AF);
    this->config_flag=1;
}

void HARD_IIC::config(uint8_t Pin_Scl, uint8_t Pin_Sda) {
    this->SCL.init(Pin_Scl,GPIO_Mode_AF);
    this->SDA.init(Pin_Sda,GPIO_Mode_AF);
    this->config_flag=1;
}

void HARD_IIC::default_config() {
    if (this->IIC == I2C1) {
        if (this->config_flag == 0)
            this->config(GPIOB6, GPIOB7);
        this->SCL.set_AFConfig(GPIO_AF_I2C1);
        this->SDA.set_AFConfig(GPIO_AF_I2C1);
    }
    else if (this->IIC == I2C2) {
        if (this->config_flag == 0)
            this->config(GPIOB10, GPIOB11);
        this->SCL.set_AFConfig(GPIO_AF_I2C2);
        this->SDA.set_AFConfig(GPIO_AF_I2C2);
    }
    else if (this->IIC == I2C3) {
        if (this->config_flag == 0)
            this->config(GPIOA8, GPIOC9);
        this->SCL.set_AFConfig(GPIO_AF_I2C3);
        this->SDA.set_AFConfig(GPIO_AF_I2C3);
    }
}

void HARD_IIC::init(I2C_TypeDef* IICx,uint8_t Address,uint32_t Speed) {

    this->IIC=IICx;
    this->default_config();
    if(IICx==I2C1)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    if(IICx==I2C2)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    if(IICx==I2C3)
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C3, ENABLE);

    I2C_DeInit(IICx);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = Address;
    I2C_InitStructure.I2C_ClockSpeed = Speed;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(IICx, &I2C_InitStructure);
    // 使能IIC
    I2C_Cmd(IICx, ENABLE);
}

void HARD_IIC::Start() {
    uint16_t err_temp=this->err_times;
    I2C_GenerateSTART(this->IIC, ENABLE);
    while(!I2C_CheckEvent(this->IIC,I2C_EVENT_MASTER_MODE_SELECT)){
        if((err_temp--)==0){
            break;
        }
    }
}

void HARD_IIC::Stop() {
    I2C_GenerateSTOP(this->IIC, ENABLE);
}

void HARD_IIC::Send_Byte_address(uint8_t daddr) {
    uint8_t  err_temp=this->err_times;
    I2C_Send7bitAddress(this->IIC, daddr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(this->IIC, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)){
        if((err_temp--)==0){
            break;
        }
    }
}

void HARD_IIC::Send_Byte(uint8_t data) {
    uint8_t  err_temp=this->err_times;
    I2C_SendData(this->IIC, data);
    while(!I2C_CheckEvent(this->IIC, I2C_EVENT_MASTER_BYTE_TRANSMITTED)){
        if((err_temp--)==0){
            break;
        }
    }
}

void HARD_IIC::Read_Byte_address(uint8_t daddr) {
    uint8_t  err_temp=this->err_times;
    I2C_Send7bitAddress(this->IIC, daddr, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(this->IIC, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){
        if((err_temp--)==0){
            break;
        }
    }
}

uint8_t HARD_IIC::Read_Byte() {
    uint8_t  err_temp=this->err_times;
    uint8_t  ret=I2C_ReceiveData(this->IIC);
    while(!I2C_CheckEvent(this->IIC, I2C_EVENT_MASTER_BYTE_RECEIVED)){
        if((err_temp--)==0){
            return 0XFF;
        }
    }
    return ret;
}

void HARD_IIC::Write_One_Byte(uint8_t daddr, uint8_t addr, uint8_t data) {
    this->Start();
    this->Send_Byte_address(daddr);
    this->Send_Byte(addr);
    this->Send_Byte(data);
    this->Stop();
}

uint8_t HARD_IIC::Read_One_Byte(uint8_t daddr, uint8_t addr) {
    this->Start();
    this->Read_Byte_address(daddr);
    this->Send_Byte(addr);
    uint8_t ret= this->Read_Byte();
    this->Stop();
    return  ret;
}

