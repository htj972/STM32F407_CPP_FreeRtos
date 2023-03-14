/**
* @Author kokirika
* @Name TMC220XUART_H
* @Date 2022-11-29
**/

#ifndef KOKIRIKA_TMC220XUART_H
#define KOKIRIKA_TMC220XUART_H

#include "sys.h"
#include "USART.h"
#include "Out_In_Put.h"
#include "Timer.h"

class TMC220xUart: public Call_Back{
protected:
    class TIMER_STEP{
    public:
        uint8_t time_num;
        Timer*   TimX;
        uint64_t tar_num;
        uint64_t cur_num;
        uint8_t  speed_num;
        uint8_t  steps_num;
        uint16_t add;
        uint16_t add_t;
        bool     over;
    }Tim_link{};
    _USART_ *uart{};// 设置串口
    _OutPut_ STEP;
    _OutPut_ EN;
    _InPut_  DIAG;
private:
    float minMoveDistance{};// 电机一圈移动距离
    uint16_t div{};// 细分数
    uint16_t maxDistance{};// 丝杆最大行程
    float stepAngle{};// 步距角
    uint32_t stepsPerRevolution{};
    float site{};
    float angle{};

    bool stop{};     //停止标志
    bool ZERO_flag{};//复位归零
    bool reverse{};

    static void calcCrc(uint8_t *datagram, uint8_t datagramLength);
    static int fastLog2(int x);
    void uartWriteInt(unsigned char address, unsigned int value);
    uint32_t readReg(uint8_t regAddr);
    void clearGSTAT();
    void setStepResolutionRegSelect(bool en);
    void setMicrosteppingResolution(uint16_t mres);
    void setVactual(uint32_t vactual, int acceleration);
    void setVactualRps(float rps);
    void setVactualRpm(uint32_t rpm);
    void moveStepUart(uint64_t Step);
    void set_stop_flag(bool flag);
    void stopMotor();
    void startMotor();

public:

#define TMC_acc  100
#define TMC_len  100
enum Direction:uint8_t{
    Back    = 0,
    Forward,
};
    TMC220xUart()=default;
    TMC220xUart(_USART_ *uartx,GPIO_TypeDef *STEP_Port, uint16_t STEP_Pin,
                GPIO_TypeDef *EN_Port, uint16_t EN_Pin,
                GPIO_TypeDef *DIAG_Port,uint16_t DIAG_Pin);
    TMC220xUart(_USART_ *uartx,uint32_t STEP_Pin,uint32_t EN_Pin,uint32_t DIAG_Pin);

    void init(_USART_ *uartx,GPIO_TypeDef *STEP_Port, uint16_t STEP_Pin,
              GPIO_TypeDef *EN_Port, uint16_t EN_Pin,
              GPIO_TypeDef *DIAG_Port,uint16_t DIAG_Pin);
    void init(_USART_ *uartx,uint32_t STEP_Pin,uint32_t EN_Pin,uint32_t DIAG_Pin);
    void config(uint16_t mres,uint16_t maxdistance, uint16_t minMovedistance,
                uint32_t stallGuardThreshold, bool Reverse= false);
    void config(uint16_t mres, bool Reverse= false);
    void Link_TIMX(Timer* TIMX);
    void Return_to_zero();

    void stallGuard(uint32_t threshold);
    void setStepDirRegSelect(uint8_t dir);
    bool get_stop_flag() const;
    void setSpreadCycle(bool en_spread);

    void moveTo(uint8_t DIR_Flag, float moveDistance);
    void turnTo(float Angle);

    void set_site(float Site);
    void set_angle(float Angle,bool continuation= true);

    virtual void send_data(TMC220xUart *TMX,uint8_t *str, uint16_t len);
    void Callback(int,char** data) override;
};


#endif //KOKIRIKA_TMC220XUART_H
