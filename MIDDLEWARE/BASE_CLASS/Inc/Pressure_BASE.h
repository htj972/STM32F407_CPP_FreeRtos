/**
* @Author kokirika
* @Name PRESSURE_BASE
* @Date 2022-10-25
**/

#ifndef KOKIRIKA_PRESSURE_BASE_H
#define KOKIRIKA_PRESSURE_BASE_H


class Pressure_BASE {
private:
    float pres_offset{};
    float pres_cache{};
public:
    explicit Pressure_BASE(float offset=0);
    ~Pressure_BASE()=default;
    float get_pres();
    virtual float get_sensor_pres();
    float get_pres_cache() const;
    void  set_pres_offset(float offset);
    float get_pres_offset() const;
    void  calculate_pres_offset(float pres);
};


#endif //KOKIRIKA_PRESSURE_BASE_H
