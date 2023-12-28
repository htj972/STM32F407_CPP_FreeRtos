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
//    this.debug("Compare: "+target+"\r\n");
    if(target.find(data)!=string::npos)
        return true;
    else
        return false;
}

bool EC20::sendcom(const string& CMD,const string& REC,uint8_t delay_time) {
    string data;
    this->Call_back_set(false);
    this->USART->clear();
    this->USART->write(CMD+"\r\n");
    do{
        delay_ms(100);
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
    if(RST_Pin!= nullptr)
    {
        RST_Pin->set(ON);
        delay_ms(1000);
        RST_Pin->set(OFF);
        return true;
    }
    else
        return this->sendcom("AT+CFUN=1,1","OK");
}

bool EC20::attest() {
    return this->sendcom("AT","OK");
}

bool EC20::getcpin() {
    return this->sendcom("AT+CPIN?","+CPIN: READY");
}

bool EC20::getcsq() {
    return this->sendcom("AT+CSQ","+CSQ: ");
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
        delay_ms(500);
        switch (Steps) {
            case 0:
                if(this->getcpin())
                    Steps++;
                else
                    continue;
                break;
            case 1:
                if(this->getcsq())
                    Steps++;
                else
                    continue;
                break;
            case 2:
                if(this->getcreg())
                    Steps++;
                else
                    continue;
                break;
            case 3:
                if(this->getcgreg())
                    Steps++;
                else
                    continue;
                break;
            case 4:
                if(this->setqicgsp(apn))
                    Steps++;
                else
                    continue;
                break;
            case 5:
                if(this->setqideact())
                    Steps++;
                else
                    continue;
                break;
            case 6:
                if(this->setqiact())
                    Steps++;
                else
                    continue;
                break;
            case 7:
                this->Link_UART_CALLback();
                Steps++;
                break;
            default:    //注册成功
                return true;
        }
        delay_ms(100);
    }

}

bool EC20::setGPS(bool EN) {
    if(EN)
        return this->sendcom("AT+QGPS=1","OK");
    else
        return this->sendcom("AT+QGPSEND","OK");
}

bool EC20::mqttopen(uint8_t id, const string &ip, uint16_t port) {
    if(this->sendcom("AT+QMTOPEN="+to_string(id)+",\""+ip+"\","+to_string(port),"+QMTOPEN: "+to_string(id)+",0"))
    {
        this->Link_s= true;
        return true;
    }
    else
    {
        this->Link_s= false;
        return false;
    }
}

bool EC20::mqttconn(uint8_t id, const string &clientid, const string &username, const string &password) {
    return this->sendcom("AT+QMTCONN="+to_string(id)+",\""+clientid+"\",\""+username+"\",\""+password+"\"","+QMTCONN: "+to_string(id)+",0,0");
}

//bool EC20::mqttconn(uint8_t id, const string &clientid, const string &token) {
//    return this->sendcom("AT+QMTCONN="+to_string(id)+",\""+clientid+"\",\""+token+"\"","OK");
//}

bool EC20::mqttpub(uint8_t id, const string &topic, const string &message) {
    uint16_t len=message.length();
    if(this->sendcom("AT+QMTPUBEX="+to_string(id)+",0,0,0,\""+topic+"\","+to_string(len)+"\r\n",">"))
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
    if(Compare(getstring,"+QMTSTAT: 1,1"))
        this->Link_s= false;
    else if(getstring.find("AT")!=string::npos){
            this->AT_get=getstring;
    }else{
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
    }
}

bool EC20::Connect(uint8_t ip1, uint8_t ip2, uint8_t ip3, uint8_t ip4, uint16_t port) {
    return mqttopen(1,to_string(ip1)+"."+to_string(ip2)+"."+to_string(ip3)+"."+to_string(ip4),port);
}

bool EC20::config(const std::string &ClientID, const std::string &Username, const std::string &Password) {
    return mqttconn(1,ClientID,Username,Password);
}

bool EC20::SubscribeTopic(const EC20::Subscribe &subscribe) {
    return mqttsub(1,subscribe.getTopic(),subscribe.getQos());
}

bool EC20::PublishData(const EC20::Publish &publish) {
    return mqttpub(1,publish.getTopic(),publish.getMessage());
}

bool EC20::PublishData(const EC20::Publish &publish, const std::string &Message) {
    return mqttpub(1,publish.getTopic(),Message);
}

bool EC20::PublishData(const std::string &publish, const std::string &Message, uint8_t qos) {
    return mqttpub(1,publish,Message);
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

void EC20::Link_RST_Pin(_OutPut_ *RST_Pinx) {
    this->RST_Pin=RST_Pinx;
    this->RST_Pin->set(OFF);
}

bool EC20::RSSI_Status() {
    uint8_t delay_time=50;
    string data;
    this->Call_back_set(false);
    this->USART->clear();
    this->USART->write("AT+CSQ\r\n");
    do{
        delay_ms(100);
        data+=this->USART->read_data();
        if(Compare(data,"\r\nOK"))
        {
            std::regex r(R"(\+CSQ: (\d+),(\d+))");
            std::smatch match;

            if (std::regex_search(data, match, r) && match.size() > 2) {
                std::string signalStrength = match.str(1);
//                std::string variable = match.str(2);
                this->_4G_RSSI=std::stoi(signalStrength);
//                this->GPRS_RSSI=std::stoi(variable);
//                this->debug("Signal strength is: "+signalStrength);
//                this->debug("Variable is: " + variable);
            }
            this->Call_back_set(true);
            return true;
        }
    } while (delay_time--);
    this->debug(data);
    this->Call_back_set(true);
    return false;
}

uint8_t EC20::get_4G_RSSI() const {
    return this->_4G_RSSI;
}

bool EC20::get_Link_Status() const {
    return this->Link_s;
}

bool EC20::getGPS() {
    this->USART->clear();
    this->USART->write("AT+QGPSLOC=2\r\n");
    uint16_t time=0;
    string data;
    while(time<5){
        if(!this->AT_get.empty()){
            //AT+QGPSLOC=2 +QGPSLOC: 024158.0,40.90340,111.78228,1.5,1125.0,2,307.37,0.0,0.0,271223,04
            string das=this->AT_get;
            if(das.find("ERROR")!=string::npos)
                return false;
            else {
                size_t startPos = das.find("+QGPSLOC:");
                if (startPos != std::string::npos) {
                    // 截取 +QGPSLOC: 后面的部分，去掉 "AT+QGPSLOC=2  " 字符串
                    std::string gpsInfo = das.substr(startPos + 9);
                    // 使用字符串流分割数据
                    std::istringstream ss(gpsInfo);
                    std::string token;
                    // 使用逗号作为分隔符
                    std::vector<std::string> tokens;
                    while (std::getline(ss, token, ',')) {
                        tokens.push_back(token);
                    }
                    // 打印分隔的数据
//                    for (const auto& t : tokens) {
//                        std::cout << t << std::endl;
//                    }
                    //按顺序将tokens放入GPS_D结构体的每个数据
                    this->GPS_D.UTC=tokens[0];
                    this->GPS_D.latitude=tokens[1];
                    this->GPS_D.longitude=tokens[2];
                    this->GPS_D.HDOP=tokens[3];
                    this->GPS_D.altitude=tokens[4];
                    this->GPS_D.fix=tokens[5];
                    this->GPS_D.cog=tokens[6];
                    this->GPS_D.spkm=tokens[7];
                    this->GPS_D.spkn=tokens[8];
                    this->GPS_D.date=tokens[9];
                    this->GPS_D.nsat=tokens[10];
                    return true;
                } else {
                    return false;
                }
            }
            return false;
        }else{
            delay_ms(100);
            time++;
        }
    }
    return false;
}













