/**
* @Author kokirika
* @Name Gateway
* @Date 2023-11-24
*/
#include "Gateway.h"
#include <utility>
#include <sstream>
#include <vector>

bool Gateway::inital() {
    if(FM24Cxx::init())
    {
        uint8_t len;
        this->cmd_len=0;
        this->read(0,ip,4);
        this->read(4,(uint8_t *)&port,2);
        this->read(6,&len,1);
        if(len<36)
        {
            this->readstr(7,&clientID,len);
        } else{
            this->clientID.clear();
        }
        this->read(43,&len,1);
        if(len<36)
        {
            this->readstr(44,&username,len);
        } else{
            this->username.clear();
        }
        this->read(84,&len,1);
        if(len<36)
        {
            this->readstr(85,&password,len);
        } else{
            this->password.clear();
        }
        this->read(128,(uint8_t *)&cmd_length,2);
        if(clientID.empty()||username.empty()||password.empty())
        {
            this->clientID="Kokirika";
            this->username="XCSZ";
            this->password="admin";
            len=clientID.length();
            this->write(6,&len,1);
            this->writestr(7,clientID);
            len=username.length();
            this->write(43,&len,1);
            this->writestr(44,username);
            len=password.length();
            this->write(84,&len,1);
            this->writestr(85,password);

            cmd_length=0;
            this->write(128,(uint8_t*)&cmd_length,2);
        }
        return true;
    }
    return false;
}

void Gateway::save_config() {
    uint16_t len;
    len=clientID.length();
    this->write(0,ip,4);
    this->write(4,(uint8_t *)&port,2);
    this->write(6,(uint8_t *)&len,2);
    this->writestr(7,clientID);
    len=username.length();
    this->write(43,(uint8_t *)&len,2);
    this->writestr(44,username);
    len=password.length();
    this->write(84,(uint8_t *)&len,2);
    this->writestr(85,password);
}

void Gateway::print_env(_USART_ *Debug) {
    Debug->print("IP:%d.%d.%d.%d\r\n",ip[0],ip[1],ip[2],ip[3]);
    Debug->print("PORT:%d\r\n",port);
    Debug->print("CLIENTID:%s\r\n",clientID.c_str());
    Debug->print("USERNAME:%s\r\n",username.c_str());
    Debug->print("PASSWORD:%s\r\n",password.c_str());
}

void Gateway::set_ip(const uint8_t *ip_t) {
    for (int i = 0; i < 4; ++i) {
        this->ip[i]=ip_t[i];
    }
    this->write(0,ip,4);
}

void Gateway::set_port(uint16_t port_t) {
    this->port=port_t;
    this->write(4,(uint8_t *)&port,2);
}

void Gateway::set_clientID(std::string clientID_t) {
    this->clientID=std::move(clientID_t);
    uint8_t len=clientID.length();
    this->write(6,&len,1);
    this->writestr(7,clientID);
}

void Gateway::set_username(std::string username_t) {
    this->username=std::move(username_t);
    uint8_t len=username.length();
    this->write(43,&len,1);
    this->writestr(44,username);
}

void Gateway::set_password(std::string password_t) {
    this->password=std::move(password_t);
    uint8_t len=password.length();
    this->write(84,&len,1);
    this->writestr(85,password);
}

std::string Gateway::Command(const std::string &COM,bool save) {
    std::istringstream iss(COM);
    std::string command;
    std::string name;
    std::string value;
    iss >> command >> name >> value;

    if (command == "set") {
        if (name == "server") {
            size_t pos = value.find(':');
            if (pos == std::string::npos) {
                return "Error: Server address should include IP and port, separated by ':'\r\n";
            }
            std::string ip_str = value.substr(0, pos);
            std::string port_str = value.substr(pos + 1);
            std::vector<std::string> parts;
            std::istringstream ip_iss(ip_str);
            std::string part;
            while (std::getline(ip_iss, part, '.')) {
                parts.push_back(part);
            }
            if (parts.size() != 4) {
                return "Error: IP address should have 4 parts\r\n";
            }
            for (int i = 0; i < 4; i++) {
                std::istringstream issp(parts[i]);
                int num;
                if (!(issp >> num) || num < 0 || num > 255) {
                    return "Error: Invalid IP address\r\n";
                }
                ip[i] = num;
            }
            std::istringstream iss_port(port_str);
            int port_num;
            if (!(iss_port >> port_num) || port_num < 0 || port_num > 65535) {
                return "Error: Invalid port number\r\n";
            }
            port = port_num;
            if(save){
                this->write(0,ip,4);
                this->write(4,(uint8_t *)&port,2);
            }
            return "set server " + std::to_string(ip[0])+ "." + std::to_string(ip[1]) + "."
            + std::to_string(ip[2]) + "." + std::to_string(ip[3]) + ":"
            + std::to_string(port) + " success\r\n";
        }else if (name == "mqtt") {
            std::string user, passd;
            if (!(iss >> user >> passd)) {
                return "Error: MQTT settings should include client ID, username, and password\r\n";
            }
            if (value.length() > 36 || user.length() > 36 || passd.length() > 36) {
                return "Error: Client ID, username, and password should not be longer than 36 characters\r\n";
            }
            clientID = value;
            this->username = user;
            this->password = passd;
            if (save){
                uint8_t len=clientID.length();
                this->write(6,&len,1);
                this->writestr(7,clientID);
                len=username.length();
                this->write(43,&len,1);
                this->writestr(44,username);
                len=password.length();
                this->write(84,&len,1);
                this->writestr(85,password);
            }
            return "MQTT settings set successfully\r\n";
        }
        // You can add more conditions here for other 'name' values
    } else if(command == "read") {
        if (name == "server") {
            std::ostringstream oss;
            oss << "Server address: " << std::to_string(ip[0]) << "." << std::to_string(ip[1]) << "."
                << std::to_string(ip[2]) << "." << std::to_string(ip[3]) << ":" << std::to_string(port) << "\r\n";
            return oss.str();
        } else if (name == "mqtt") {
            std::ostringstream oss;
            oss << "MQTT settings: " << clientID << " " << username << " " << password << "\r\n";
            return oss.str();
        }
        // You can add more conditions here for other 'name' values
    } else if(command == "save") {
        save_config();
        return "Configuration saved\r\n";
    } else if(command == "reboot") {
        NVIC_SystemReset();
    } else if(command == "init") {
        this->ip[0]=192;
        this->ip[1]=168;
        this->ip[2]=1;
        this->ip[3]=1;
        this->port=1883;
        this->clientID="";
        this->username="";
        this->password="";
        this->save_config();
        NVIC_SystemReset();
    } else if(command == "lora") { //lora send 0012A920 0 1   name:send value:0012A920
        std::string Nu, Sa;
        if (!(iss >> Nu >> Sa)) {
            return "Error: lora data should include number and data\r\n";
        }
        //value:0012A920 转换到uint8_t id[4]里面
        if(value.length()==8){
            if(this->lora!= nullptr){
                uint8_t id[4];
                for (int i = 0; i < 4; ++i) {
                    char num=0;
                    uint8_t issp =value[i*2];
                    if ((issp>='A')&&(issp<='Z')){
                        num=issp-'A'+0x0a;
                    }
                    else if ((issp>='a')&&(issp<='z')){
                        num=issp-'a'+0x0a;
                    }
                    else if ((issp>='0')&&(issp<='9')){
                        num=issp-'0'+0x0;
                    }
                    num<<=4;
                    issp =value[i*2+1];
                    if ((issp>='A')&&(issp<='Z')){
                        num|=(issp-'A'+0x0a);
                    }
                    else if ((issp>='a')&&(issp<='z')){
                        num|=(issp-'a'+0x0a);
                    }
                    else if ((issp>='0')&&(issp<='9')){
                        num|=(issp-'0'+0x0);
                    }
                    id[i] = num;
                }
                //Nu转换num
                uint8_t num = stoi(Nu);
                if(num>1)
                    return "num need Less than 2\r\n";
                this->lora->set_id(id);
                uint16_t va = stoi(Sa);
                this->lora->data_set(&this->lora->data_BUS.to_u16[num],va);
                if(this->lora->data_sync())
                    return "lora send ok\r\n";
                return "lora send error\r\n";
            } else{
                return "Error: lora not inital\r\n";
            }
        }
        else
        {
            return "Error: lora data should include 8 char\r\n";
        }
    }else if(command == "help") {
        return "Available commands:\r\n"
               "  set server <ip>:<port> - set ThingsBoard server address\r\n"
               "  set mqtt <client_id> <username> <password> - set MQTT settings\r\n"
               "  read server - show ThingsBoard server address\r\n"
               "  read mqtt - show MQTT settings\r\n"
               "  save - save configuration to flash\r\n"
               "  help - show this help\r\n"
               "  reboot - reboot device\r\n"
               "  init - reset configuration to default\r\n";

    }else{
        string ret=Device_Node_Def::Command(COM);
        if ((ret.find("success")!=string::npos)&&save)
            save_cmd(COM);
        return ret;
    }
    // You can add more conditions here for other 'command' values

    return "Unknown command\r\n";
}

void Gateway::link_thingsBoard(ThingsBoard *thingsBoard_t) {
    this->thingsBoard=thingsBoard_t;
    this->thingsBoard->Connect(ip[0],ip[1],ip[2],ip[3],port);
    this->thingsBoard->config(clientID,username,password);
    this->thingsBoard->SubscribeTopic();
}

void Gateway::save_cmd(const string &cmd) {
    cmd_length+=cmd.length();
    this->write(128,(uint8_t*)&cmd_length,2);
    this->writestr(130+cmd_length-cmd.length(),cmd);
}
//读取下一条指令，ps每条指令以\r\n结尾 长度不超过64
//cmd_len是临时的命令位置
std::string Gateway::read_next_cmd() {
    std:string cmd_str;
    //读取cmd_len到\r\n
    if(this->cmd_len<cmd_length)
    {
        uint8_t char_t;
        for (int ii = 0; ii < 64; ++ii) {
            this->read(130+this->cmd_len+ii,&char_t,1);
            cmd_str+=char_t;
            if(cmd_str.find("\r\n")!=string::npos)
            {
                this->cmd_len+=cmd_str.length();
                return cmd_str;
            }
        }
        return "";
    }
    return "";
}

bool Gateway::run_cmd(_USART_ *Debug) {
    while(cmd_len<cmd_length)
    {
        std::string cmd=this->read_next_cmd();
        if(!cmd.empty())
        {
            std::string ret=this->Command(cmd,false);
            if(ret.find("success")!=string::npos)
            {
                this->cmd_len+=cmd.length();
                *Debug<<cmd<<"  "<<ret;
            }
        } else{
            return false;
        }
    }
    return true;
}

void Gateway::set_reread() {
    this->cmd_len=0;
}

void Gateway::Link_lora(WH_COM *lorax) {
    this->lora=lorax;
}

