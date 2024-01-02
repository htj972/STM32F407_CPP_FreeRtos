/**
* @Author kokirika
* @Name WH_L101
* @Date 2023-12-06
*/
#include <sstream>
#include "WH_L101.h"
#include "delay.h"


WH_L101::WH_L101(_USART_ *UARTx) {
    this->init(UARTx);
}

void WH_L101::init(_USART_ *UARTx) {
    this->USARTx=UARTx;
    this->run_mode=HOST;
    this->timeout=500;
    this->freetime=20;
}

void WH_L101::send_cmd(const std::string& cmd) {
    if(this->USARTx!= nullptr){
        this->USARTx->write(cmd);
    }
}

void WH_L101::set_mode(uint16_t channel, uint16_t ADDR) {
    /*
     * AT+PMODE=WU  //设置为无线唤醒模式
     * AT+WTM=500   //设置唤醒时间为500ms
     * AT+CH=4700   //设置频点为470MHz
     * AT+ADDR=10   //设置地址为10
     * AT+LBT=ON    //设置空闲检测为ON
     * AT+LORACSQ=2 //设置信号强度显示
     */
    send_cmd("+++");
    delay_ms(50);
    send_cmd("a");
    delay_ms(50);
    char cmds[20];
    send_cmd("AT+PMODE=WU\r\n");
    delay_ms(50);
    send_cmd("AT+WTM=500\r\n");
    delay_ms(50);
    send_cmd("AT+LBT=ON\r\n");
    delay_ms(50);
    send_cmd("AT+LORACSQ=2\r\n");
    delay_ms(50);
    sprintf(cmds,"AT+CH=%d\r\n",channel);
    send_cmd(cmds);
    delay_ms(50);
    sprintf(cmds,"AT+ADDR=%d\r\n",ADDR);
    send_cmd(cmds);
    delay_ms(50);
    this->USARTx->clear();
    send_cmd("AT+NID?\r\n");
    delay_ms(50);
    get_id();
    send_cmd("AT+ENTM\r\n");
}

void WH_L101::get_id() {
    if(this->USARTx->available()==32){
        std::string getbuf = this->USARTx->read_data();
        for(uint8_t ii=0;ii<4;ii++){
            uint8_t idt=0x00;
            if((getbuf[16+ii*2]>='0')&&(getbuf[16+ii*2]<='9'))
                idt=getbuf[16+ii*2]-'0'+0x00;
            else if((getbuf[16+ii*2]>='A')&&(getbuf[16+ii*2]<='F'))
                idt=getbuf[16+ii*2]-'A'+0x0a;
            idt=idt<<4;
            if((getbuf[17+ii*2]>='0')&&(getbuf[17+ii*2]<='9'))
                idt|=getbuf[17+ii*2]-'0'+0x00;
            else if((getbuf[17+ii*2]>='A')&&(getbuf[17+ii*2]<='F'))
                idt|=getbuf[17+ii*2]-'A'+0x0a;
            dID[ii]=idt;
        }
    }
}

void WH_L101::set_id(const uint8_t *id) {
    for (int i = 0; i < 4; ++i) {
        slave_id[i]=id[i];
    }
}

//typedef struct k_WH_Host_03_receive{
//    uint8_t id[4];
//    uint8_t mode;
//    uint8_t num;
//    uint8_t data[258];;
////    uint8_t CRC16[2];
//}modbus_Host_03_receive ;

//typedef struct k_WH_Host_06_receive_{
//    uint8_t id[4];
//    uint8_t mode;
//    uint16_t address;
//    uint16_t data;
//    uint16_t CRC16;
//}modbus_Host_06_receive_;

//typedef struct k_WH_Host_16_receive{
//    uint8_t id[4];
//    uint8_t mode;
//    uint16_t address;
//    uint16_t len;
//    uint8_t num;
//    uint16_t CRC16;
//}modbus_Host_16_receive;

void WH_L101::modbus_Host_03_coding(uint8_t *ID, uint16_t address, uint16_t num) {
    modbus_send_data.clear();
    modbus_send_data+=ID[0];
    modbus_send_data+=ID[1];
    modbus_send_data+=ID[2];
    modbus_send_data+=ID[3];
    modbus_send_data+=(char)0x03;
    modbus_send_data+=(address>>8)&0x00ff;
    modbus_send_data+=(address>>0)&0x00ff;
    modbus_send_data+=(num>>8)&0x00ff;
    modbus_send_data+=(num>>0)&0x00ff;
    uint16_t crc16_check=modbus::Compute(modbus_send_data);
    modbus_send_data+=(crc16_check>>8)&0x00ff;
    modbus_send_data+=(crc16_check>>0)&0x00ff;
    this->slave_address=address;
    this->send_data_fun(modbus_send_data);
    this->send_flag=3;
}

void WH_L101::modbus_Host_06_coding(uint8_t *ID, uint16_t address, uint16_t data) {
    modbus_send_data.clear();
    modbus_send_data+=ID[0];
    modbus_send_data+=ID[1];
    modbus_send_data+=ID[2];
    modbus_send_data+=ID[3];
    modbus_send_data+=(char)0x06;
    modbus_send_data+=(address>>8)&0x00ff;
    modbus_send_data+=(address>>0)&0x00ff;
    modbus_send_data+=(data>>8)&0x00ff;
    modbus_send_data+=(data>>0)&0x00ff;
    uint16_t crc16_check=modbus::Compute(modbus_send_data);
    modbus_send_data+=(crc16_check>>8)&0x00ff;
    modbus_send_data+=(crc16_check>>0)&0x00ff;
    this->slave_address=address;
    this->send_data_fun(modbus_send_data);
    this->send_flag=6;
}

void WH_L101::modbus_Host_16_coding(uint8_t *ID, uint16_t address, const uint16_t *data, uint16_t num) {
    modbus_send_data.clear();
    modbus_send_data+=ID[0];
    modbus_send_data+=ID[1];
    modbus_send_data+=ID[2];
    modbus_send_data+=ID[3];
    modbus_send_data+=(char)0x10;
    modbus_send_data+=(address>>8)&0x00ff;
    modbus_send_data+=(address>>0)&0x00ff;
    modbus_send_data+=(num>>8)&0x00ff;
    modbus_send_data+=(num>>0)&0x00ff;
    modbus_send_data+=(char)(num*2);
    for(uint16_t ii=0;ii<num;ii++)
    {
        modbus_send_data+=(data[ii]>>8)&0x00ff;
        modbus_send_data+=(data[ii]>>0)&0x00ff;
    }
    uint16_t  crc16_check=modbus::Compute(modbus_send_data);
    modbus_send_data+=(crc16_check>>8)&0x00ff;
    modbus_send_data+=(crc16_check>>0)&0x00ff;
    this->slave_address=address;
    this->send_data_fun(modbus_send_data);
    this->send_flag=0x10;
}

void WH_L101::modbus_Host_03_uncoding() {
    uint8_t num=this->modbus_receive_data[5];
    uint16_t CRC16=modbus_receive_data[6+num]<<8|modbus_receive_data[7+num];
    uint16_t crc16_check=modbus::Compute(this->modbus_receive_data,6+num);
    if(crc16_check==CRC16)//比较校验
    {
        for(uint8_t ii=0;ii<num/2;ii++)
        {
            this->write_data(this->slave_address+ii,(uint8_t*)&this->modbus_receive_data[ii*2+6]);
        }

        if(this->send_flag==3)
            this->send_flag=0;
        string str=modbus_receive_data.substr(6+num,modbus_receive_data.size()-8-num);
        get_rssi(str);
    }
}

void WH_L101::modbus_Host_06_uncoding() {
    uint16_t CRC16=modbus_receive_data[9]<<8|modbus_receive_data[10];
    uint16_t crc16_check=modbus::Compute(this->modbus_receive_data,6+3);									//计算接收数据校验
    if(crc16_check==CRC16)//比较校验
    {
        if(this->send_flag==6)
            this->send_flag=0;
        string str=modbus_receive_data.substr(11,modbus_receive_data.size()-13);
        get_rssi(str);
    }
}
extern _USART_ DEBUG;
void WH_L101::modbus_Host_16_uncoding() {
    uint16_t CRC16=modbus_receive_data[10]<<8|modbus_receive_data[11];
    uint16_t  crc16_check=modbus::Compute(modbus_receive_data,7+3);
    if(CRC16==crc16_check)
    {
        if(this->send_flag==0x10)
            this->send_flag=0;
        string str=modbus_receive_data.substr(12,modbus_receive_data.size()-14);
        get_rssi(str);
    }
}

uint8_t WH_L101::modbus_03_send(uint16_t address, uint16_t num) {
    modbus_Host_03_coding(this->slave_id,address,num);
    if(this->modbus_wait_rec())
        return modbus::modbus_success;
    else
    {
        this->send_flag=0;
        return modbus::modbus_03_error;
    }
}

uint8_t WH_L101::modbus_06_send(uint16_t address, uint16_t data) {
    modbus_Host_06_coding(this->slave_id,address,data);
    if(this->modbus_wait_rec())
        return modbus::modbus_success;
    else
    {
        this->send_flag=0;
        return modbus::modbus_06_error;
    }
}

uint8_t WH_L101::modbus_16_send(uint16_t address, uint16_t *data, uint16_t num) {
    modbus_Host_16_coding(this->slave_id,address,data,num);
    if(this->modbus_wait_rec())
        return modbus::modbus_success;
    else
    {
        this->send_flag=0;
        return modbus::modbus_10_error;
    }
}

void WH_L101::receive_data_channel() {
    //判断 this->modbus_receive_data,this->slave_id数组完全
    if((this->modbus_receive_data[0]==this->slave_id[0])&&
         (this->modbus_receive_data[1]==this->slave_id[1])&&
         (this->modbus_receive_data[2]==this->slave_id[2])&&
         (this->modbus_receive_data[3]==this->slave_id[3]))
     {
          switch(this->modbus_receive_data[4])
          {
                case 0x03:
                 this->modbus_Host_03_uncoding();
                 break;
                case 0x06:
                 this->modbus_Host_06_uncoding();
                 break;
                case 0x10:
                 this->modbus_Host_16_uncoding();
                 break;
          }
     }
}

void WH_L101::get_rssi(string  str) {
    size_t snr_pos = str.find("SNR:");
    if (snr_pos != std::string::npos) {
        str=str.substr(snr_pos + 4);
        size_t rssi_pos = str.find(" ,RSSI:");
        if (rssi_pos != std::string::npos) {
            string snr=str.substr(0,rssi_pos);
            string rssi=str.substr(rssi_pos+7);
            this->SNR=std::stoi(snr);
            this->RSSI=std::stoi(rssi);
        }
    }
}




