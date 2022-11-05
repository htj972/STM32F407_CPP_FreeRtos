/**
* @Author kokirika
* @Name PID_BASE
* @Date 2022-11-02
**/

#include "PID_BASE.h"

void PID_BASE::init(float P, float I, float D) {
    this->kp=P;
    this->ki=I;
    this->kd=D;
}

void PID_BASE::set_target(float target) {
    this->tar=target;
}

void PID_BASE::clear() {
    this->sumError=0;
    this->lastError=0;
}

float PID_BASE::get_target() const {
    return this->tar;
}

float PID_BASE::calculate(float target, float current) {
    this->tar=target;
    this->Error=target-current;//当前误差 //PID算法第一步 设定转速减去当前转速 赋值给 Error
    this->sumError=this->Error+this->sumError;//误差和
    this->dError=this->Error-this->lastError;//误差偏差
    this->lastError=this->Error;
    this->B=this->kp*this->Error+this->ki*this->sumError+this->kd*this->dError;
    return this->B;
}

float PID_BASE::calculate(float current) {
    this->Error=this->tar-current;//当前误差 //PID算法第一步 设定转速减去当前转速 赋值给 Error
    this->sumError=this->Error+this->sumError;//误差和
    this->dError=this->Error-this->lastError;//误差偏差
    this->lastError=this->Error;
    this->B=this->kp*this->Error+this->ki*this->sumError+this->kd*this->dError;
    return this->B;
}



void Kalman::init(float kQ, float kR) {
    this->A = 1;  //标量卡尔曼
    this->H = 2;  //
    this->P = 0.01;  //后验状态估计值误差的方差的初始值（不要为0问题不大）
    this->Q = kQ;    //预测（过程）噪声方差 影响收敛速率，可以根据实际需求给出
    this->R = kR;    //测量（观测）噪声方差 可以通过实验手段获得
    this->filterValue = 0;// 测量的初始值
}

float Kalman::Filter(float Measure) {
    //预测下一时刻的值
    //x的先验估计由上一个时间点的后验估计值和输入信息给出，此处需要根据基站高度做一个修改
    float predictValue = this->A* this->filterValue;

    //求协方差
    //计算先验均方差 p(n|n-1)=A^2*p(n-1|n-1)+q
    this->P = this->A*this->A*this->P + this->Q;
    //double preValue = this->filterValue;  //记录上次实际坐标的值
    //计算kalman增益
    //Kg(k)= P(k|k-1) H’ / (H P(k|k-1) H’ + R)
    this->kalmanGain = this->P*this->H / (this->P*this->H*this->H + this->R);
    //修正结果，即计算滤波值
    //利用残余的信息改善对x(t)的估计，给出后验估计，这个值也就是输出  X(k|k)= X(k|k-1)+Kg(k) (Z(k)-H X(k|k-1))
    this->filterValue = predictValue + (Measure - predictValue)*this->kalmanGain;
    //更新后验估计
    //计算后验均方差  P[n|n]=(1-K[n]*H)*P[n|n-1]
    this->P = (1 - this->kalmanGain*this->H)*this->P;

    return  this->filterValue;
}
