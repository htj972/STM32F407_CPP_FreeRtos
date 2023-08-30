/**
* @Author kokirika
* @Name EC20
* @Date 2023-07-18
*/
#include "EC20.h"
#include "delay.h"

EC20::EC20(USART_TypeDef *USARTx, int32_t bound) {
    this->new_flag = true;
    this->USART=new _USART_(USARTx,bound);
}

EC20::EC20(_USART_ *USARTx) {
    this->new_flag = false;
    this->USART=USARTx;
}

EC20::~EC20() {
    if (this->new_flag)
        delete this->USART;
}

bool EC20::init(USART_TypeDef *USARTx, int32_t bound) {
    this->new_flag = true;
    this->USART=new _USART_(USARTx,bound);
    return this->init();
}

bool EC20::init(_USART_* USARTx) {
    this->new_flag = false;
    this->USART=USARTx;
    return this->init();
}

bool EC20::init() {
    if(this->USART!= nullptr)
        return this->attest();
    return false;
}

bool EC20::Compare(const string& target,const string& data) {
    if(target.find(data)>=0)
        return true;
    else
        return false;
}

bool EC20::sendcom(const string& CMD,const string& REC,uint8_t delay_time) {
    string data;
    this->USART->clear();
    do{
        this->USART->write(CMD+"\r\n");
        delay_ms(1000);
        data+=this->USART->read_data();
        if(Compare(data,REC))
        {
            this->debug(data);
            return true;
        }
    } while (delay_time--);
    this->debug(data);
    return false;
}

void EC20::debug(const string &str) {
    if(this->Debug_USART!= nullptr)
        this->Debug_USART->write(str);
    else if(this->Debug!= nullptr)
        this->Debug(str);
}

string EC20::getAPN_Name(const EC20::APN &apn) {
    switch (apn) {
        case APN::APN_CMNET:
            return "CMNET";
        case APN::APN_CMWAP:
            return "CMWAP";
        case APN::APN_UNINET:
            return "UNINET";
        case APN::APN_UNIWAP:
            return "UNIWAP";
        case APN::APN_3GWAP:
            return "3GWAP";
        case APN::APN_3GNET:
            return "3GNET";
        case APN::APN_CTWAP:
            return "CTWAP";
        case APN_CTNET:
            return "CTNET";
        default:
            return "CMNET";
    }
}

bool EC20::getrdy() {
    getstring.clear();
    getstring+=this->USART->read_data();
    if(!getstring.empty()) {
        if (Compare(getstring, "RDY")) {
            this->debug("Copare:" + getstring);
            return true;
        } else {
            this->debug(getstring);
            return false;
        }
    }
    return false;
}

void EC20::setdebug(void (*debug)(const string& str)) {
    this->Debug=debug;
}

void EC20::setdebug(_USART_ *USARTx) {
    this->Debug_USART=USARTx;
}

bool EC20::attest() {
    return this->sendcom("AT","AT OK");
}

bool EC20::getcpin() {
    return this->sendcom("AT+CPIN?","+CPIN: READY");
}

bool EC20::getcsq() {
    return this->sendcom("AT+CSQ","+CSQ:");
}

bool EC20::getcreg() {
    return this->sendcom("AT+CREG?","+CREG: 0,1");
}

bool EC20::getcgreg() {
    return this->sendcom("AT+CGREG?","+CGREG: 0,1");
}

bool EC20::setqicgsp(EC20::APN apn) {
    return this->sendcom("AT+QICSGP=1,1,\""+getAPN_Name(apn)+"\"","OK");
}

bool EC20::setqideact() {
    return this->sendcom("AT+QIDEACT=1","OK");
}

bool EC20::setqiact() {
    return this->sendcom("AT+QIACT=1","OK");
}
//运行所有步骤
bool EC20::Register(APN apn) {
    uint8_t Steps=0;
    while (true){
        switch (Steps) {
            case 0:
                if(this->getcpin())
                    Steps++;
                else
                    return false;
                break;
            case 1:
                if(this->getcsq())
                    Steps++;
                else
                    return false;
                break;
            case 2:
                if(this->getcreg())
                    Steps++;
                else
                    return false;
                break;
            case 3:
                if(this->getcgreg())
                    Steps++;
                else
                    return false;
                break;
            case 4:
                if(this->setqicgsp(apn))
                    Steps++;
                else
                    return false;
                break;
            case 5:
                if(this->setqideact())
                    Steps++;
                else
                    return false;
                break;
            case 6:
                if(this->setqiact())
                    Steps++;
                else
                    return false;
                break;
            default:    //注册成功
                return true;
        }
    }

}

bool EC20::mqttopen(uint8_t id, const string &ip, uint16_t port) {
    return this->sendcom("AT+QMTOPEN="+to_string(id)+",\""+ip+"\","+to_string(port),"OK");
}

bool EC20::mqttconn(uint8_t id, const string &clientid, const string &username, const string &password) {
    return this->sendcom("AT+QMTCONN="+to_string(id)+",\""+clientid+"\",\""+username+"\",\""+password+"\"","OK");
}

//bool EC20::mqttconn(uint8_t id, const string &clientid, const string &token) {
//    return this->sendcom("AT+QMTCONN="+to_string(id)+",\""+clientid+"\",\""+token+"\"","OK");
//}

bool EC20::mqttpub(uint8_t id, const string &topic, const string &message) {
    uint16_t len=message.length();
    if(this->sendcom("AT+QMTPUBEX="+to_string(id)+",0,0,0,\""+topic+"\","+to_string(len)+"\r\n"+message,">"))
        return this->sendcom(message,"OK");
    else
        return false;
}

bool EC20::mqttclose(uint8_t id) {
    return this->sendcom("AT+QMTCLOSE="+to_string(id),"OK");
}

bool EC20::mqttdisc(uint8_t id) {
    return this->sendcom("AT+QMTDISC="+to_string(id),"OK");
}

string EC20::read_data() {
    return this->USART->read_data();
}



















