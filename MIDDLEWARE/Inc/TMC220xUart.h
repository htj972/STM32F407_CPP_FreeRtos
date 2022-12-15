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

class TMC220xUart: public Call_Back{
protected:
    _USART_ *uart{};// 设置串口
    _OutPut_ STEP;
    _OutPut_ EN;
    _InPut_  DIAG;
private:
    uint16_t minMoveDistance{};// 电机一圈移动距离
    uint16_t div{};// 细分数
    uint16_t maxDistance{};// 丝杆最大行程
    float stepAngle{};// 步距角
    uint32_t stepsPerRevolution{};

    bool stop{};     //停止标志
    bool ZERO_flag{};//复位归零
    bool reverse{};

    static void calcCrc(uint8_t *datagram, uint8_t datagramLength);
    static int fastLog2(int x);
    void uartWriteInt(unsigned char address, unsigned int value);
    uint32_t readReg(uint8_t regAddr);
    void clearGSTAT();
    void setStepResolutionRegSelect(bool en);
    void setMicrosteppingResolution(uint8_t mres);
    void setVactual(uint32_t vactual, int acceleration);
    void setVactualRps(float rps);
    void setVactualRpm(uint32_t rpm);
    void moveStepUart(uint64_t Step);
    void set_stop_flag(bool flag);

public:

#define TMC_acc  100
#define TMC_len  100
enum Direction:uint8_t{
    Back    = 0,
    Forward,
};

    TMC220xUart(_USART_ *uartx,GPIO_TypeDef *STEP_Port, uint16_t STEP_Pin,
                GPIO_TypeDef *EN_Port, uint16_t EN_Pin,
                GPIO_TypeDef *DIAG_Port,uint16_t DIA_Pin);
    TMC220xUart(_USART_ *uartx,uint32_t STEP_Pin,uint32_t EN_Pin,uint32_t DIAG_Pin);
    void initTMC2209(uint8_t mres,uint16_t maxdistance, uint16_t minMovedistance,
                     uint8_t stallGuardThreshold, bool Reverse= false);
    void Return_to_zero();
    void moveTo(uint8_t DIR_Flag, uint32_t moveDistance);
    void stopMotor();
    void startMotor();
    void stallGuard(uint32_t threshold);
    bool get_stop_flag() const;

    void setSpreadCycle(bool en_spread);

    void Callback(int,char** data) override;
};


#endif //KOKIRIKA_TMC220XUART_H
