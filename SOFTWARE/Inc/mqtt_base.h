//
// Created by 53105 on 2023/8/2.
//

#ifndef KOKIRIKA_MQTT_BASE_H
#define KOKIRIKA_MQTT_BASE_H

#include <string>
#include <vector>
#include "sys.h"

class mqtt_base {
protected:
    std::string topic;
    std::string message;
public:
    class Subscribe{
    protected:
        std::string topic;
        uint8_t qos{};
        uint8_t whether{};
        std::string message;
    public:
        Subscribe()= default;
        Subscribe(std::string topic,uint8_t qos,uint8_t whether):topic(std::move(topic)),qos(qos),whether(whether){}
        Subscribe(std::string topic,uint8_t qos,uint8_t whether,std::string message):topic(std::move(topic)),qos(qos),whether(whether),message(std::move(message)){}
        std::string getTopic() const {
            return topic;
        }
        uint8_t getQos() const {
            return qos;
        }
        uint8_t getWhether() const {
            return whether;
        }
        std::string getMessage() const {
            return message;
        }

    };
    void Message_analyze(const std::vector<unsigned char>& bytes);
    void Message_analyze(const std::string& str);
    static bool isValidMQTT(const std::string& str);
    static bool Wildcard_recognition(const char *str);
    bool Wildcard_recognition(const std::string& str);
    static bool check_topic(const std::string& subscribed_topic, const std::string& received_topic);
    bool check_topic(const std::string& subscribed_topic);
    bool check_topic(const Subscribe& subscribe);
    std::string getMsg() const {
        return message;
    }
};


#endif //KOKIRIKA_MQTT_BASE_H
