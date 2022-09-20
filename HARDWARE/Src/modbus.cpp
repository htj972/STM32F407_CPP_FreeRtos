/**
* @Author kokirika
* @Name Modbus
* @Date 2022-09-20
**/

#include "modbus.h"
#include "string"

/****************************************
 * @structs : Slave
 */
typedef struct{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t len[2];
    uint8_t CRC16[2];
}modbus_Slave_03_receive_;

typedef struct{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t num;
    std::string data;
    uint8_t CRC16[2];
}modbus_Slave_03_send_;

typedef struct{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t data[4];
    uint8_t CRC16[2];
}modbus_Slave_06_send_receive_;

typedef struct{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t len[2];
    uint8_t num;
    std::string data;;
    uint8_t CRC16[2];
}modbus_Slave_16_receive_;

typedef struct{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t len[2];
    uint8_t CRC16[2];
}modbus_Slave_16_send_;

/****************************************
 * @structs : Host
 */
typedef struct{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t len[2];
    uint8_t CRC16[2];
}modbus_Host_03_send;

typedef struct{
    uint8_t id;
    uint8_t mode;
    uint8_t num;
    std::string data;;
    uint8_t CRC16[2];
}modbus_Host_03_receive ;

typedef struct{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t len[2];
    uint8_t num;
    std::string data;;
    uint8_t CRC16[2];
}modbus_Host_16_send;

typedef struct{
    uint8_t id;
    uint8_t mode;
    uint8_t address[2];
    uint8_t len[2];
    uint8_t num;
    uint8_t CRC16[2];
}modbus_Host_16_receive;

/****************************************
 * @structs : END
 */

modbus::modbus() {
    this->USARTx= nullptr;
}

modbus::~modbus() {
    this->USARTx= nullptr;
    delete this;
}

void modbus::send_data_fun(uint8_t *data, uint16_t len) {
    if(USARTx!= nullptr)
        this->USARTx->write(data,len);
}

uint16_t modbus::read_data(const uint8_t *address) {
    if(this->init_flag==0)
        return 0;
    else
    {
        uint16_t address_temp=(address[0]<<8)+address[1];
        if((address_temp>=this->data_start_end[0])&&(address_temp<=this->data_start_end[1]))
            return *(this->data_list+address_temp);
        else
            return 0;
    }
}

uint8_t modbus::write_data(uint8_t *address, uint8_t* data) {
    if(this->init_flag==0)
        return 0;
    else
    {
        uint16_t address_temp=(address[0]<<8)+address[1];
        if((address_temp>=this->data_start_end[0])&&(address_temp<=this->data_start_end[1]))
        {
            *(this->data_list+address_temp)=(data[0]<<8)+data[1];
            return 1;
        }
        else
            return 0;
    }
}

void modbus::init(uint16_t* address,uint16_t len,uint16_t start) {
    this->data_list=address;
    this->data_start_end[0]=start;
    this->data_start_end[1]=start+len;
    init_flag=1;
}

/****************************************
 * @MODE : 0x03
 */
void modbus::modbus_Slave_03_uncoding() {
    uint16_t crc16_check;       //校验
    uint8_t data_num=0;
    uint8_t * modbus_send_data;
    modbus_Slave_03_receive_ *modebus_data_temp;	//声明接受结构体指针
    modbus_Slave_03_send_    send_data_temp;		//声明发送结构体
    modebus_data_temp=(modbus_Slave_03_receive_*)this->modbus_receive_data;
    crc16_check=modbus::Compute(modbus_receive_data,6);
    if(crc16_check==(modebus_data_temp->CRC16[1]<<8)+(modebus_data_temp->CRC16[0]))//比较校验
    {
        modbus_send_data=(uint8_t*)&send_data_temp;	//发送结构体放入数组指针
        send_data_temp.id	=modebus_data_temp->id;     //设置ID
        send_data_temp.mode	=modebus_data_temp->mode;	//设置模式
        send_data_temp.address[0]=modebus_data_temp->address[0];	//设置地址
        send_data_temp.address[1]=modebus_data_temp->address[1];	//设置地址
        uint16_t num = (modebus_data_temp->len[0]<<8)+(modebus_data_temp->len[1]);//获取寄存器个数
        //modebus_data_temp->address;
        for(uint16_t ii=0;ii<num*4;ii++)							//循环数据长度
        {
            uint16_t get_data_t=this->read_data(modebus_data_temp->address+ii);
            send_data_temp.data[data_num++]=(get_data_t>>8)&0x00ff; //放入发送结构体
            send_data_temp.data[data_num++]=(get_data_t>>0)&0x00ff; //放入发送结构体
        }
        send_data_temp.num=data_num;							//设置字节数
        crc16_check=modbus::Compute(modbus_send_data,data_num+5);//计算CRC   通过数组指针
        //校验应紧接着数据而不是放入结构体位置
        modbus_send_data[data_num+5]=(crc16_check>>0)&0x00ff;	//设置CRC
        modbus_send_data[data_num+6]=(crc16_check>>8)&0x00ff;	//设置CRC
        this->send_data_fun(modbus_send_data,data_num+5+2);
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
}

void modbus::modbus_03_uncoding() {
    modbus_Host_03_receive *modbus_03_receive;
    modbus_03_receive=(modbus_Host_03_receive*)this->modbus_receive_data;
    uint8_t num=modbus_03_receive->num;
    uint16_t crc16_check=modbus::Compute(this->modbus_receive_data,5+num);
    if(crc16_check==(modbus_03_receive->CRC16[0]<<8)+(modbus_03_receive->CRC16[1]))//比较校验
    {
        for(uint8_t ii=0;ii<modbus_03_receive->num/2;ii++)
        {
            uint8_t address[3];
            address[0]=(this->slave_address>>8)&0x00ff;
            address[1]=(this->slave_address>>0)&0x00ff;
            uint8_t data[3];
            data[0]=(modbus_03_receive->data[ii]>>8)&0x00ff;
            data[1]=(modbus_03_receive->data[ii++]>>0)&0x00ff;
            this->write_data(address,data);
        }
    }
}





