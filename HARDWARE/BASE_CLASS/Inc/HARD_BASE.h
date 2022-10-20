/**
* @Author kokirika
* @Name HARD_BASE
* @Date 2022-10-20
**/

#ifndef KOKIRIKA_HARD_BASE_H
#define KOKIRIKA_HARD_BASE_H

#if SYSTEM_SUPPORT_OS == ON
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
    HARD_BASE::Queue lock_mode;
    bool lock_flag{};
#if SYSTEM_SUPPORT_OS == ON
    QueueHandle_t spi_Queue{};
#endif
public:
    void set_Queue_mode(Queue mode);
    void Queue_star();
    void Queue_end();
    bool get_Lock() const;
};


#endif //KOKIRIKA_HARD_BASE_H
