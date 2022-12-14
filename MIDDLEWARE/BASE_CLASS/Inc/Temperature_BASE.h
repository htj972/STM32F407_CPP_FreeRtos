/**
* @Author kokirika
* @Name TEMPERATURE_BASE
* @Date 2022-10-20
**/

#ifndef KOKIRIKA_TEMPERATURE_BASE_H
#define KOKIRIKA_TEMPERATURE_BASE_H


class Temperature_BASE {
protected:
    float temp_offset{};
    float temp_cache{};
public:
    explicit Temperature_BASE(float offset=0);
    ~Temperature_BASE()=default;
    float get_temp();
    virtual float get_sensor_temp();
    float get_temp_cache() const;
    void  set_temp_offset(float offset);
    float get_temp_offset() const;
    void  calculate_temp_offset(float temp);
};


#endif //KOKIRIKA_TEMPERATURE_BASE_H
