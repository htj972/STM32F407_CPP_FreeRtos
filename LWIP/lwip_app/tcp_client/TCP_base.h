/**
* @Author kokirika
* @Name TCP_BASE
* @Date 2023-06-12
*/
#ifndef kokirika_TCP_BASE_H
#define kokirika_TCP_BASE_H

#include "sys.h"
#include "HARD_BASE.h"

class TCP_BASE {
    public:
    virtual void send_data(char *data,uint16_t len){};
    virtual void upload_extern_fun(Call_Back *extx){};
};

#endif //kokirika_TCP_BASE_H
