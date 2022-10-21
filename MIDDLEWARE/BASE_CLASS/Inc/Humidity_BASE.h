/**
* @Author kokirika
* @Name Humidity_BASE
* @Date 2022-10-20
**/

#ifndef KOKIRIKA_HUMIDITY_BASE_H
#define KOKIRIKA_HUMIDITY_BASE_H


class Humidity_BASE {
private:
    float Humi_offset;

public:
    explicit Humidity_BASE(float offset=0);
    ~Humidity_BASE()=default;
    float get_humi();
    virtual float get_sensor_humi();
    void  set_humi_offset(float offset);
    void  calculate_humi_offset(float humi);
};


#endif //KOKIRIKA_HUMIDITY_BASE_H
