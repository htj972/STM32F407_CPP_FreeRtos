/**
* @Author kokirika
* @Name HARD_BASE
* @Date 2022-10-20
**/

#include "HARD_BASE.h"

void HARD_BASE::set_Queue_mode(Queue mode) {
    this->lock_mode= mode;
    this->lock_flag= false;
}

void HARD_BASE::Queue_star() {
    if(this->lock_mode==Queue::OWN_Queue){
        while(this->lock_flag);
        this->lock_flag=true;
    }
#if SYSTEM_SUPPORT_OS == ON
    else{
        xSemaphoreTake(HARD_Queue,portMAX_DELAY);
    }
#endif
}

void HARD_BASE::Queue_end() {
    if(this->lock_mode==Queue::OWN_Queue)
        this->lock_flag= false;
#if SYSTEM_SUPPORT_OS == ON
    else{
        xSemaphoreGive(HARD_Queue);
    }
#endif
}

bool HARD_BASE::get_Lock() const {
    return this->lock_flag;
}


