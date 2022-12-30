/**
* @Author kokirika
* @Name TEMP_CTRL
* @Date 2022-11-02
**/

#ifndef KOKIRIKA_TEMP_CTRL_H
#define KOKIRIKA_TEMP_CTRL_H

#include "Temperature_BASE.h"
#include "PWM_BASE.h"
#include "PID_BASE.h"

class Temp_ctrl {
protected:
    Temperature_BASE *SENSOR{};
    PWM_BASE         *CONTROLLER{};
    PID_BASE         PID{};
    Kalman           karl{};
    bool             kal_f{};
    float            range{};
    uint8_t          chx{};
    float            cur{};
public:
    Temp_ctrl(Temperature_BASE *sensor,PWM_BASE *ctrl,uint8_t ch);
    Temp_ctrl()=default;
    void init(Temperature_BASE *sensor,PWM_BASE *ctrl,uint8_t ch);
    void init();
    void config_PID(float P, float I, float D);
    void config_kalman(float kQ, float kR);
    void set_karl(bool en);
    void set_target(float tar);
    void PID_ENABLE_Range(float Range);
    void upset(bool en= true);
    float get_cur() const;
};


#endif //KOKIRIKA_TEMP_CTRL_H
