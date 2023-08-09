/**
* @Author kokirika
* @Name MQTT_BASE
* @Date 2023-06-09
*/

#include <regex>
#include "mqtt_base.h"

void mqtt_base::Message_analyze(const std::vector<unsigned char> &bytes) {
    std::string str;
    for (auto byte : bytes) {
        str += byte;
    }
    this->Message_analyze(str);
}

//��Ϣ����
void mqtt_base::Message_analyze(const std::string &str) {
    if(isValidMQTT(str)){
        int topic_length = str[2] * 256 + str[3];
        topic = str.substr(4, topic_length);
        message = str.substr(4 + topic_length);
    }
}

bool mqtt_base::isValidMQTT(const std::string& str) {
    // Check if the message is a PUBLISH message
    if ((str[0] >> 4) != 3) {
        return false;
    }

    // Check the length of the message
    int remaining_length = str[1];
    if (str.size() != remaining_length + 2) {
        return false;
    }

    return true;
}

bool mqtt_base::Wildcard_recognition(const char *str) {
    //ʶ��mqttͨ������ # ����༶ͨ��� + ������ͨ���
    for (int i = 0; str[i]; i++) {
        if (str[i] == '+') {
            // Check if '+' is correctly used
            if ((i == 0 || str[i-1] == '/') && (str[i+1] == '\0' || str[i+1] == '/')) {
                return true;
            }
        }
    }
    return false;
}

bool mqtt_base::Wildcard_recognition(const std::string& str) {
    return this->Wildcard_recognition(str.c_str());
}

bool mqtt_base::check_topic(const std::string& subscribed_topic, const std::string& received_topic) {
    // ��"+"�滻Ϊ������ʽ�������ַ�ƥ�䣨��/��
    std::string pattern = std::regex_replace(subscribed_topic, std::regex("\\+"), "[^/]+");
    // ��"#"�滻Ϊ������ʽ�������ַ�ƥ�䣨����/��
    pattern = std::regex_replace(pattern, std::regex("#"), ".*");
    // ����һ����ȫƥ�䣨�ӿ�ʼ����������������ʽ
    pattern = "^" + pattern + "$";
    // ʹ��������ʽƥ����յ�������
    std::regex r(pattern);
    return std::regex_match(received_topic, r);
}

bool mqtt_base::check_topic(const std::string &subscribed_topic) {
    return this->check_topic(subscribed_topic,this->topic);
}

bool mqtt_base::check_topic(const mqtt_base::Subscribe& subscribe) {
    return this->check_topic(subscribe.getTopic(),this->topic);
}

