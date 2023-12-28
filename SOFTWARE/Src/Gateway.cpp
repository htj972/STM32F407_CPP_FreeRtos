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
        this->read(0,ip,4);
        this->read(4,(uint8_t *)&port,2);
        this->read(6,&len,1);
        if(len>36)
        {
            this->readstr(7,&clientID,len);
        } else{
            this->clientID.clear();
        }
        this->read(43,&len,1);
        if(len>36)
        {
            this->readstr(44,&username,len);
        } else{
            this->username.clear();
        }
        this->read(84,&len,1);
        if(len>36)
        {
            this->readstr(85,&password,len);
        } else{
            this->password.clear();
        }


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
        } else
        {
            uint16_t len_l;
            this->read(128,(uint8_t*)&len_l,2);
            if(len_l>128)
                this->readstr(130,&cmd_str,len_l);
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
    this->writestr(8,clientID);

    len=username.length();
    this->write(8+len,(uint8_t *)&len,2);
    this->writestr(10+len,username);

    len=password.length();
    this->write(10+len+len,(uint8_t *)&len,2);
    this->writestr(12+len+len,password);

    len=cmd_str.length();
    this->write(128,(uint8_t *)&len,2);
    this->writestr(130,cmd_str);
}

void Gateway::print_env(_USART_ *Debug) {
    Debug->print("IP:%d.%d.%d.%d\r\n",ip[0],ip[1],ip[2],ip[3]);
    Debug->print("PORT:%d\r\n",port);
    Debug->print("CLIENTID:%s\r\n",clientID.c_str());
    Debug->print("USERNAME:%s\r\n",username.c_str());
    Debug->print("PASSWORD:%s\r\n",password.c_str());
    Debug->print("CMD:%s\r\n",cmd_str.c_str());
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

std::string Gateway::Command(const std::string &COM) {
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
            return "set server " + std::to_string(ip[0])+ "." + std::to_string(ip[1]) + "."
            + std::to_string(ip[2]) + "." + std::to_string(ip[3]) + ":"
            + std::to_string(port) + " success\r\n";
        }else if (name == "mqtt") {
            std::string cId, user, passd;
            if (!(iss >> cId >> user >> passd)) {
                return "Error: MQTT settings should include client ID, username, and password\r\n";
            }
            if (cId.length() > 36 || user.length() > 36 || passd.length() > 36) {
                return "Error: Client ID, username, and password should not be longer than 36 characters\r\n";
            }
            clientID = cId;
            this->username = user;
            this->password = passd;
            return "MQTT settings set successfully\r\n";
        }
        // You can add more conditions here for other 'name' values
    } else
        return Device_Node_Def::Command(COM);
    // You can add more conditions here for other 'command' values

    return "Unknown command";
}

