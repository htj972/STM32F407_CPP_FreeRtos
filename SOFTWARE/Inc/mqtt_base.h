/**
* @Author kokirika
* @Name MQTT_BASE
* @Date 2023-06-09
*/

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
        ~Subscribe() = default;
        explicit Subscribe(std::string topic,uint8_t qos=0,uint8_t whether=1):topic(std::move(topic)),qos(qos),whether(whether){}
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
    class Publish{
    protected:
        std::string topic;
        std::string message;
        uint8_t qos{};
    public:
        Publish()= default;
        ~Publish() = default;
        explicit Publish(std::string topic,uint8_t qos=0):topic(std::move(topic)),qos(qos){}
        std::string getTopic() const {
            return topic;
        }
        std::string getMessage() const {
            return message;
        }
        uint8_t getQos() const {
            return qos;
        }
        void setQos(uint8_t Qos) {
            this->qos = Qos;
        }
        void setMessage(const std::string& Message) {
            this->message = Message;
        }
        void setTopic(const std::string& Topic) {
            this->topic = Topic;
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
