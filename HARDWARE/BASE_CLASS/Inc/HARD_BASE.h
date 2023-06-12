/**
* @Author kokirika
* @Name HARD_BASE
* @Date 2022-10-20
**/

#ifndef KOKIRIKA_HARD_BASE_H
#define KOKIRIKA_HARD_BASE_H

#if SYSTEM_SUPPORT_OS == ON

#include <string>
#include "FreeRTOS.h"
#include "semphr.h"
#endif

class HARD_BASE {
public:
    enum Queue{
        OWN_Queue,
        OS_Queue,
    };
private:
    HARD_BASE::Queue lock_mode{};
    bool lock_flag{};
#if SYSTEM_SUPPORT_OS == ON
    QueueHandle_t HARD_Queue{};
#endif
public:
    void set_Queue_mode(Queue mode);
    void Queue_star();
    void Queue_end();
    bool get_Lock() const;
};

class Call_Back{
public:
    enum Name:uint8_t{
        exit = 0,
        uart,
        timer,
        timer_cc,
        Intnel,
    };
    enum MODE{
        C_fun,
        C_fun_r,
        CPP_fun,
        class_fun,
    };
    virtual void (Callback)(int ,char **){};
    virtual void (Callback)(std::string str){};
};





#endif //KOKIRIKA_HARD_BASE_H
