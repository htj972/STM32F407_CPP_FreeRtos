/**
* @Author kokirika
* @Name PID_BASE
* @Date 2022-11-02
**/

#ifndef KOKIRIKA_PID_BASE_H
#define KOKIRIKA_PID_BASE_H

#include "sys.h"

class PID_BASE {
private:
    float kp;
    float ki;
    float kd;
    float B;
    float sumError;
    float lastError;
    float dError;
    float Error;
protected:
    float tar;
public:
    void init(float P, float I, float D);
    void set_target(float target);
    void clear();
    float get_target() const;
    float calculate(float target,float current);
    float calculate(float current);
};

class Kalman{
private:
    float filterValue;  //k-1ʱ�̵��˲�ֵ������k-1ʱ�̵�ֵ
    float kalmanGain;   //   Kalamn����
    float A;   // x(n)=A*x(n-1)+u(n),u(n)~N(0,Q)
    float H;   // z(n)=H*x(n)+w(n),w(n)~N(0,R)
    float Q;   //Ԥ���������ƫ��ķ���
    float R;   //��������ƫ�(ϵͳ����Ժ�ͨ������ͳ��ʵ����)
    float P;   //�������Э����
public:
    void init(float kQ, float kR);
    float Filter(float Measure);
};


#endif //KOKIRIKA_PID_BASE_H
