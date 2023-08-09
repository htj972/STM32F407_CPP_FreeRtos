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

//信息分析
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
    //识别mqtt通配主题 # 代表多级通配符 + 代表单级通配符
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
    // 将"+"替换为正则表达式的任意字符匹配（非/）
    std::string pattern = std::regex_replace(subscribed_topic, std::regex("\\+"), "[^/]+");
    // 将"#"替换为正则表达式的任意字符匹配（包括/）
    pattern = std::regex_replace(pattern, std::regex("#"), ".*");
    // 创建一个完全匹配（从开始到结束）的正则表达式
    pattern = "^" + pattern + "$";
    // 使用正则表达式匹配接收到的主题
    std::regex r(pattern);
    return std::regex_match(received_topic, r);
}

bool mqtt_base::check_topic(const std::string &subscribed_topic) {
    return this->check_topic(subscribed_topic,this->topic);
}

bool mqtt_base::check_topic(const mqtt_base::Subscribe& subscribe) {
    return this->check_topic(subscribe.getTopic(),this->topic);
}

