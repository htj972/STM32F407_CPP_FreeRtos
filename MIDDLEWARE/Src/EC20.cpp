/**
* @Author kokirika
* @Name EC20
* @Date 2023-07-18
*/
#include <regex>
#include "EC20.h"
#include "delay.h"

EC20::EC20(USART_TypeDef *USARTx, int32_t bound) {
    this->new_flag = true;
    this->USART=new _USART_(USARTx,bound);
    this->CallBack_flag=false;
    this->init_flag=false;
    set_freetime(5);
}

EC20::EC20(_USART_ *USARTx) {
    this->new_flag = false;
    this->USART=USARTx;
    this->CallBack_flag=false;
    this->init_flag=false;
    set_freetime(5);
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
    set_freetime(5);
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
    this->Call_back_set(false);
    this->USART->clear();
    do{
        this->USART->write(CMD+"\r\n");
        delay_ms(1000);
        data+=this->USART->read_data();
        if(Compare(data,REC))
        {
            this->debug(data);
            this->Call_back_set(true);
            return true;
        }
    } while (delay_time--);
    this->debug(data);
    this->Call_back_set(true);
    return false;
}

void EC20::debug(const string &str) {
    if(this->Debug_USART!= nullptr)
        this->Debug_USART->write(str);
}

void EC20::Call_back_set(bool ON_OFF) {
    if(this->init_flag) {
        static bool flag=true;
        if (ON_OFF) {
            this->CallBack_flag = flag;
            if (this->CallBack_flag)
                this->USART->upload_extern_fun(this);
        } else {
            flag= this->CallBack_flag;
            this->CallBack_flag = false;
            this->USART->upload_extern_close();
        }
    }
}

void EC20::Call_back_set(bool ON_OFF, bool init) {
    this->init_flag=init;
    if(this->init_flag)
        this->CallBack_flag=ON_OFF;
    this->Call_back_set(ON_OFF);
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

void EC20::setdebug(_USART_ *USARTx) {
    this->Debug_USART=USARTx;
}

bool EC20::reset() {
    return this->sendcom("AT+CFUN=1,1","OK");
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
            case 7:
                this->Link_UART_CALLback();
                Steps++;
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

bool EC20::mqttsub(uint8_t id, const string &topic, uint8_t qos) {
    return this->sendcom("AT+QMTSUB="+to_string(id)+",1,\""+topic+"\","+to_string(qos),"OK");
}

bool EC20::mqttunsub(uint8_t id, const string &topic) {
    return this->sendcom("AT+QMTUNS="+to_string(id)+",1,\""+topic+"\"","OK");
}

void EC20::Link_UART_CALLback() {
    this->init_flag=true;
    this->CallBack_flag=true;
    this->USART->upload_extern_fun(this);
}

void EC20::Callback(int, char **gdata) {
    if(gdata[0][0]==Call_Back::Name::uart)
    {
        this->getstring+=gdata[2][0];
    }
    else if(gdata[0][0]==Call_Back::Name::timer)
    {

        this->TIMERX->set_Cmd(false);
        uint16_t len_t=this->getstring.length();
        if(this->reveive_len!=len_t) {
            this->reveive_len = len_t;
            this->freetime_t=0;
        }
        else if(len_t!=0)
        {
            this->freetime_t++;
            if(this->freetime_t==this->freetime) {
                this->Receive_data();
                this->getstring.clear();
                this->freetime_t=0;
            }
        }
        this->TIMERX->set_Cmd(true);
    }
}

void EC20::Link_TIMER_CALLback(Timer *TIMX) {
    this->TIMERX=TIMX;
    TIMX->upload_extern_fun(this);
}

void EC20::set_freetime(uint16_t sfreetime) {
    this->freetime=sfreetime;
}

void EC20::Receive_data() {
    // 创建一个正则表达式来匹配主题和信息
    std::regex r("\\+QMTRECV: ([\\d,]+),\"([^\"]*)\",(\".*\")");

    std::smatch match;
    if (std::regex_search(getstring, match, r)) {
        string info=match[3];
        this->topic=match[2];
        this->message = info.substr(1, info.size() - 2);
        this->Otherstring="+QMTRECV:";
        this->Otherstring.append(match[1]);
//        this->debug("Other: "+this->Otherstring+"\r\n");
//        this->debug("Topic: "+this->topic+"\r\n");
//        this->debug("Info: "+this->message+"\r\n");
    }
    getstring.clear();
}

bool EC20::Connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port) {
    return mqttopen(0,to_string(ip1)+"."+to_string(ip2)+"."+to_string(ip3)+"."+to_string(ip4),port);
}

bool EC20::config(const std::string &ClientID, const std::string &Username, const std::string &Password) {
    return mqttconn(0,ClientID,Username,Password);
}

bool EC20::SubscribeTopic(const EC20::Subscribe &subscribe) {
    return mqttsub(0,subscribe.getTopic(),subscribe.getQos());
}

bool EC20::PublishData(const EC20::Publish &publish) {
    return mqttpub(0,publish.getTopic(),publish.getMessage());
}

bool EC20::PublishData(const EC20::Publish &publish, const std::string &Message) {
    return mqttpub(0,publish.getTopic(),Message);
}

bool EC20::PublishData(const std::string &publish, const std::string &Message, uint8_t qos) {
    return mqttpub(0,publish,Message);
}

bool EC20::available() {
    return !this->message.empty();
}

bool EC20::islink() {
    return this->init_flag;
}

string EC20::GetRxbuf() {
    std::string str = this->message;
    this->message.clear();
    return str;
}

void EC20::Disconnect() {
    this->mqttdisc(0);
}

bool EC20::PHY_status() {
    return this->init_flag;
}










