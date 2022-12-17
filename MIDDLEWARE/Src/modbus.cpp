/**
* @Author kokirika
* @Name Modbus
* @Date 2022-09-20
**/

#include "modbus.h"

#include <utility>
#include "string"
#include "FreeRTOS.h"
#include "task.h"


/****************************************
 * @structs : Slave
 */
typedef struct k_modbus_Slave_03_receive_{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t len[2];
    uint8_t CRC16[2];
}modbus_Slave_03_receive_;

typedef struct k_modbus_Slave_06_send_receive_{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t data[2];
    uint8_t CRC16[2];
}modbus_Slave_06_send_receive_;

typedef struct k_modbus_Slave_16_receive_{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t len[2];
    uint8_t num;
    uint8_t data[256];
}modbus_Slave_16_receive_;


/****************************************
 * @structs : Host
 */
typedef struct k_modbus_Host_03_send{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t len[2];
    uint8_t CRC16[2];
}modbus_Host_03_send;

typedef struct k_modbus_Host_03_receive{
    uint8_t id;
    uint8_t mode;
    uint8_t num;
    std::string data;;
    uint8_t CRC16[2];
}modbus_Host_03_receive ;

typedef struct k_modbus_Host_06_send_receive_{
    uint8_t id;
    uint8_t mode;
    uint16_t address;
    uint16_t data;
    uint16_t CRC16;
}modbus_Host_06_send_receive_;

typedef struct k_modbus_Host_16_send{
    uint8_t id{};
    uint8_t mode{};
    uint8_t address[2]{};
    uint8_t len[2]{};
    uint8_t num{};
    std::string data;;
    uint8_t CRC16[2]{};
}modbus_Host_16_send;

typedef struct k_modbus_Host_16_receive{
    uint8_t id;
    uint8_t mode;
    uint16_t address;
    uint16_t len;
    uint8_t num;
    uint16_t CRC16;
}modbus_Host_16_receive;

/****************************************
 * @structs : END
 */

modbus::modbus(_USART_* USART,uint8_t mode,uint8_t id,uint16_t stimeout,uint16_t sfreetime) {
    this->send_flag= 0;
    this->init_flag= false;
    this->init(USART,mode,id,stimeout,sfreetime);
}

modbus::~modbus() {
    this->USARTx= nullptr;
    delete this;
}

void modbus::config(uint16_t* address,uint16_t len,uint16_t start) {
    this->data_list=address;
    this->data_start_end[0]=start;
    this->data_start_end[1]=start+len;
    init_flag= true;
}

void modbus::init(_USART_* USART,uint8_t mode,uint8_t id,uint16_t stimeout,uint16_t sfreetime) {
    this->USARTx=USART;
    this->run_mode=mode;
    this->timeout=stimeout;
    this->freetime=sfreetime;
    if(this->run_mode==modbus::SLAVE)
        this->own_id=id;
    else
    {
        this->run_mode=modbus::HOST;
        this->slave_id=id;
    }
}

void modbus::send_data_fun(uint8_t *data, uint16_t len) {
    if(USARTx!= nullptr)
        this->USARTx->write(data,len);
}

void modbus::send_data_fun(string str) {
    if(USARTx!= nullptr)
        this->USARTx->write(std::move(str));
}

uint16_t modbus::read_data(uint16_t address) {
    if(!this->init_flag)
        return 0;
    else
    {
        if((address>=this->data_start_end[0])&&(address<=(this->data_start_end[0]+this->data_start_end[1])))
            return *(this->data_list+address-this->data_start_end[0]);
        else
            return 0;
    }
}

bool modbus::write_data(uint16_t address, const uint8_t* data) {
    if(!this->init_flag)
        return false;
    else
    {
        if((address>=this->data_start_end[0])&&(address<=(this->data_start_end[0]+this->data_start_end[1])))
        {
            *(this->data_list+address-this->data_start_end[0])=(data[0]<<8)+data[1];
            return true;
        }
        else
            return false;
    }
}

bool modbus::modbus_wait_rec() const {
    uint16_t time_t=this->timeout;
    while((this->send_flag!=0)&&(time_t>=10))
    {
        vTaskDelay(10/portTICK_RATE_MS );
        time_t-=10;
    }
    if(this->send_flag==0)
        return true;
    else
        return false;
}

void modbus::set_id(uint8_t id) {
    if(this->run_mode==modbus::SLAVE)
        this->own_id=id;
    else
    {
        this->run_mode=modbus::HOST;
        this->slave_id=id;
    }
}

void modbus::set_mode(uint8_t mode) {
    this->run_mode=mode;
}

void modbus::set_freetime(uint16_t sfreetime) {
    this->freetime=sfreetime;
}

void modbus::set_timeout(uint16_t stimeout) {
    this->timeout=stimeout;
}

/*!
 * @从机03解码
 */
void modbus::modbus_Slave_03_uncoding() {
    uint16_t crc16_check;       //校验
    uint8_t data_num=0;

    modbus_Slave_03_receive_ *modebus_data_temp;	//声明接受结构体指针
    modebus_data_temp=(modbus_Slave_03_receive_*)this->modbus_receive_data.data();
    crc16_check=modbus::Compute(modbus_receive_data,6);
    modbus_send_data.clear();
    if(crc16_check==(modebus_data_temp->CRC16[0]<<8)+(modebus_data_temp->CRC16[1]))//比较校验
    {
        modbus_send_data+=modebus_data_temp->id;     //设置ID
        modbus_send_data+=modebus_data_temp->mode;	//设置模式
        uint16_t num = (modebus_data_temp->len[0]<<8)+(modebus_data_temp->len[1]);//获取寄存器个数
        modbus_send_data+=(uint8_t)(num*2);
        uint16_t address_t=(modebus_data_temp->address[0]<<8)+modebus_data_temp->address[1];
        for(uint16_t ii=0;ii<num;ii++)							//循环数据长度
        {
            uint16_t get_data_t=this->read_data(address_t+ii);
            modbus_send_data+=(get_data_t>>8)&0x00ff; //放入发送结构体
            modbus_send_data+=(get_data_t>>0)&0x00ff; //放入发送结构体
        }
        crc16_check=modbus::Compute(modbus_send_data);//计算CRC   通过数组指针
        //校验应紧接着数据而不是放入结构体位置
        modbus_send_data+=(crc16_check>>8)&0x00ff;	//设置CRC
        modbus_send_data+=(crc16_check>>0)&0x00ff;	//设置CRC
        this->send_data_fun(modbus_send_data);
    }
}

void modbus::modbus_Host_03_coding(uint8_t ID,uint16_t address,uint16_t num) {
    uint8_t * modbus_send_data;
    modbus_Host_03_send send_data_temp;		//声明发送结构体
    modbus_send_data=(uint8_t*)&send_data_temp;
    send_data_temp.id=ID;
    send_data_temp.mode=0x03;
    send_data_temp.address[0]=(address>>8)&0x00ff;
    send_data_temp.address[1]=(address>>0)&0x00ff;
    send_data_temp.len[0]=(num>>8)&0x00ff;
    send_data_temp.len[1]=(num>>0)&0x00ff;
    uint16_t crc16_check=modbus::Compute(modbus_send_data,6);
    send_data_temp.CRC16[0]=(crc16_check>>8)&0x00ff;
    send_data_temp.CRC16[1]=(crc16_check>>0)&0x00ff;
    this->slave_address=address;
    this->send_data_fun(modbus_send_data,8);
    this->send_flag=3;
}

void modbus::modbus_Host_03_uncoding() {
    modbus_Host_03_receive *modbus_03_receive;
    modbus_03_receive=(modbus_Host_03_receive*)this->modbus_receive_data.data();
    uint8_t num=modbus_03_receive->num;
    uint16_t crc16_check=modbus::Compute(this->modbus_receive_data,5+num);
    if(crc16_check==(modbus_03_receive->CRC16[0]<<8)+(modbus_03_receive->CRC16[1]))//比较校验
    {
        for(uint8_t ii=0;ii<modbus_03_receive->num/2;ii++)
        {
            uint8_t data[3];
            data[0]=(modbus_03_receive->data[ii]>>8)&0x00ff;
            data[1]=(modbus_03_receive->data[ii++]>>0)&0x00ff;
            this->write_data(this->slave_address+ii,data);
        }
    }
    if(this->send_flag==3)
        this->send_flag=0;
}
/*!
 * @从机06解码
 */
void modbus::modbus_Slave_06_uncoding() {
//    uint8_t * modbus_send_data;
    modbus_Slave_06_send_receive_ *modebus_data_temp;				//声明接受结构体指针
    modebus_data_temp=(modbus_Slave_06_send_receive_*)this->modbus_receive_data.data();//接收数据放入结构体指针
    uint16_t crc16_check=modbus::Compute(modbus_receive_data,6);									//计算接收数据校验
    if(crc16_check==(modebus_data_temp->CRC16[0]<<8)+(modebus_data_temp->CRC16[1]))//比较校验
    {
        uint16_t address_t=(modebus_data_temp->address[0]<<8)+modebus_data_temp->address[1];
        this->write_data(address_t,modebus_data_temp->data);//写入数据
        this->send_data_fun(modbus_receive_data);		//发送
    }
}

void modbus::modbus_Host_06_coding(uint8_t ID, uint16_t address, uint16_t data) {
    uint8_t * modbus_send_data;
    modbus_Host_06_send_receive_  modebus_data_temp;				//声明接受结构体指针
    modbus_send_data=(uint8_t*)&modebus_data_temp;
    modebus_data_temp.id=ID;
    modebus_data_temp.mode=0x06;
    modebus_data_temp.address=address;
    modebus_data_temp.data=data;
    modebus_data_temp.CRC16=modbus::Compute(modbus_send_data,8);
    this->send_data_fun(modbus_send_data,10);		//发送
    this->send_flag=6;
}

void modbus::modbus_Host_06_uncoding() {
    modbus_Host_06_send_receive_ *modebus_data_temp;				//声明接受结构体指针
    modebus_data_temp=(modbus_Host_06_send_receive_*)this->modbus_receive_data.data();    //接收数据放入结构体指针
    uint16_t crc16_check=modbus::Compute(this->modbus_receive_data,8);									//计算接收数据校验
    if(crc16_check==modebus_data_temp->CRC16)//比较校验
    {

    }
    if(this->send_flag==6)
        this->send_flag=0;
}
/*!
 * @从机16解码
 */
void modbus::modbus_Slave_10_uncoding() {
    modbus_Slave_16_receive_    *modebus_data_temp;	//声明接受结构体指针
//    modbus_Slave_16_send_       send_data_temp;		//声明发送结构体
    modebus_data_temp=(modbus_Slave_16_receive_*)this->modbus_receive_data.data(); //接收数据放入结构体指针
    uint16_t crc16_check=modbus::Compute(this->modbus_receive_data,this->modbus_receive_data.length()-2);//计算接收数据校验
    if(crc16_check==(modbus_receive_data[modebus_data_temp->num+6+1]<<8)+
                    modbus_receive_data[modebus_data_temp->num+6+2])//比较校验
    {
        modbus_send_data.clear();
        modbus_send_data+=modebus_data_temp->id;			//设置ID
        modbus_send_data+=modebus_data_temp->mode;		    //设置模式
        modbus_send_data+=modebus_data_temp->address[0];    //设置地址
        modbus_send_data+=modebus_data_temp->address[1];    //设置地址
        modbus_send_data+=modebus_data_temp->len[0];		//设置数量
        modbus_send_data+=modebus_data_temp->len[1];		//设置数量

        uint16_t address_t=(modebus_data_temp->address[0]<<8)+modebus_data_temp->address[1];
        for(uint8_t ii=0;ii<modebus_data_temp->num/2;ii++)
            this->write_data(address_t+ii,&(modebus_data_temp->data[ii*2]));
        crc16_check=modbus::Compute(modbus_send_data);
        modbus_send_data+=(crc16_check>>8)&0x00ff;
        modbus_send_data+=(crc16_check>>0)&0x00ff;
        this->send_data_fun(modbus_send_data);
    }
}

void modbus::modbus_Host_10_coding(uint8_t ID,uint16_t address,const uint16_t *data,uint16_t num) {
    uint8_t * modbus_send_data;
    modbus_Host_16_send     modbus_16_send;
    modbus_send_data=(uint8_t*)&modbus_16_send;
    modbus_16_send.id=ID;
    modbus_16_send.mode=0x10;
    modbus_16_send.address[0]=(address>>8)&0x00ff;
    modbus_16_send.address[1]=(address>>0)&0x00ff;
    modbus_16_send.len[0]=(num>>8)&0x00ff;
    modbus_16_send.len[1]=(num>>0)&0x00ff;
    modbus_16_send.num=num*2;
    for(uint16_t ii=0;ii<num;ii++)
    {
        modbus_16_send.data[ii*2]=(data[ii]>>8)&0x00ff;
        modbus_16_send.data[ii*2+1]=(data[ii]>>0)&0x00ff;
    }
    uint8_t  crc16_check=modbus::Compute(modbus_send_data,6+1+modbus_16_send.num);
    modbus_16_send.CRC16[0]=(crc16_check>>8)&0x00ff;
    modbus_16_send.CRC16[1]=(crc16_check>>0)&0x00ff;
    modbus_send_data[6+modbus_16_send.num*2+1]=modbus_16_send.CRC16[0];
    modbus_send_data[6+modbus_16_send.num*2+2]=modbus_16_send.CRC16[1];
    this->send_data_fun(modbus_send_data,6+1+modbus_16_send.num*2+2);
    this->send_flag=0x10;
}

void modbus::modbus_Host_10_uncoding() {
    u8 ii;
    modbus_Host_16_receive *modbus_16_receive;
    modbus_16_receive=(modbus_Host_16_receive*)this->modbus_receive_data.data();
    uint8_t  crc16_check=modbus::Compute(modbus_receive_data,6);
    if(modbus_16_receive->CRC16==crc16_check)
    {

    }
    if(this->send_flag==0x10)
        this->send_flag=0;
}

uint8_t modbus::modbus_03_send(uint16_t address, uint16_t num) {
    modbus_Host_03_coding(this->slave_id,address,num);
    if(this->modbus_wait_rec())
        return modbus::modbus_success;
    else
    {
        this->send_flag=0;
        return modbus::modbus_03_error;
    }
}

uint8_t modbus::modbus_06_send(uint16_t address, uint16_t data) {
    modbus_Host_06_coding(this->slave_id,address,data);
    if(this->modbus_wait_rec())
        return modbus::modbus_success;
    else
    {
        this->send_flag=0;
        return modbus::modbus_06_error;
    }
}

uint8_t modbus::modbus_10_send(uint16_t address, uint16_t *data, uint16_t num) {
    modbus_Host_10_coding(this->slave_id,address,data,num);
    if(this->modbus_wait_rec())
        return modbus::modbus_success;
    else
    {
        this->send_flag=0;
        return modbus::modbus_10_error;
    }
}

void modbus::modbus_receive_upset() {
    uint16_t len_t=this->USARTx->available();
    if(this->reveive_len!=len_t) {
        this->reveive_len = len_t;
        this->freetime_t=0;
    }
    else
    {
        if(!this->send_flag)
            this->freetime_t++;
        if(this->freetime_t==this->freetime) {
//            this->modbus_receive_data.clear();
            this->modbus_receive_data=this->USARTx->read_data();
            this->receive_data_channel();
            this->freetime_t=0;
        }
    }
}

void modbus::receive_data_channel() {
    if(this->run_mode==modbus::SLAVE)
    {
        if(this->modbus_receive_data[0]==this->own_id)
        switch(modbus_receive_data[1])
        {
            case 0x03:
                this->modbus_Slave_03_uncoding();
                break;
            case 0x06:
                this->modbus_Slave_06_uncoding();
                break;
            case 0x10:
                this->modbus_Slave_10_uncoding();
                break;
        }
    }
    else
    {
        if(this->modbus_receive_data[0]==this->slave_id)
            switch(modbus_receive_data[1])
            {
                case 0x03:
                    this->modbus_Host_03_uncoding();
                    break;
                case 0x06:
                    this->modbus_Host_06_uncoding();
                    break;
                case 0x10:
                    this->modbus_Host_10_uncoding();
                    break;
            }
    }
}

void modbus::Callback(int, char **gdata) {
    if(gdata[0][0]==Call_Back::Name::uart)
    {
        this->modbus_receive_data+=gdata[2][0];
    }
    else if(gdata[0][0]==Call_Back::Name::timer)
    {
        this->TIMERX->set_Cmd(false);
        uint16_t len_t=this->modbus_receive_data.length();
        if(this->reveive_len!=len_t) {
            this->reveive_len = len_t;
            this->freetime_t=0;
        }
        else if(len_t!=0)
        {
            if(!this->send_flag)
                this->freetime_t++;
            if(this->freetime_t==this->freetime) {
                this->receive_data_channel();
                this->modbus_receive_data.clear();
                this->freetime_t=0;
            }
        }
        this->TIMERX->set_Cmd(true);
    }

}

void modbus::Link_UART_CALLback() {
    this->USARTx->upload_extern_fun(this);
}

void modbus::Link_TIMER_CALLback(Timer *TIMX) {
    this->TIMERX=TIMX;
    TIMX->upload_extern_fun(this);
}














