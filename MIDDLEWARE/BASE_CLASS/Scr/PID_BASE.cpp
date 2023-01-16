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
    this->ladder=0;
    this->last_B=0;
}

void PID_BASE::set_target(float target) {
    this->tar=target;
}

void PID_BASE::clear() {
    this->sumError=0;
    this->lastError=0;
    this->last_B=0;
}

void PID_BASE::set_ladder(float ladderx) {
    if(ladderx<0)
        ladderx=-ladderx;
    this->ladder=ladderx;
}

float PID_BASE::get_target() const {
    return this->tar;
}

float PID_BASE::calculate(float target, float current) {
    this->tar=target;
    return this->calculate(current);
}

float PID_BASE::calculate(float current) {
    this->Error=this->tar-current;//��ǰ��� //PID�㷨��һ�� �趨ת�ټ�ȥ��ǰת�� ��ֵ�� Error
    this->sumError=this->Error+this->sumError;//����
    this->dError=this->Error-this->lastError;//���ƫ��
    this->lastError=this->Error;
    this->B=this->kp*this->Error+this->ki*this->sumError+this->kd*this->dError;
    if(this->ladder!=0){
        if(this->B>this->last_B+this->ladder) {
            this->B = this->last_B + this->ladder;
        }else
        if(this->B<this->last_B-this->ladder) {
            this->B = this->last_B - this->ladder;
        }
    }
    this->last_B = this->B;
    return this->B;
}


void Kalman::init(float kQ, float kR) {
    this->A = 1;  //����������
    this->H = 2;  //
    this->P = 0.01;  //����״̬����ֵ���ķ���ĳ�ʼֵ����ҪΪ0���ⲻ��
    this->Q = kQ;    //Ԥ�⣨���̣��������� Ӱ���������ʣ����Ը���ʵ���������
    this->R = kR;    //�������۲⣩�������� ����ͨ��ʵ���ֶλ��
    this->filterValue = 0;// �����ĳ�ʼֵ
}

float Kalman::Filter(float Measure) {
    //Ԥ����һʱ�̵�ֵ
    //x�������������һ��ʱ���ĺ������ֵ��������Ϣ�������˴���Ҫ���ݻ�վ�߶���һ���޸�
    float predictValue = this->A* this->filterValue;

    //��Э����
    //������������� p(n|n-1)=A^2*p(n-1|n-1)+q
    this->P = this->A*this->A*this->P + this->Q;
    //double preValue = this->filterValue;  //��¼�ϴ�ʵ�������ֵ
    //����kalman����
    //Kg(k)= P(k|k-1) H�� / (H P(k|k-1) H�� + R)
    this->kalmanGain = this->P*this->H / (this->P*this->H*this->H + this->R);
    //����������������˲�ֵ
    //���ò������Ϣ���ƶ�x(t)�Ĺ��ƣ�����������ƣ����ֵҲ�������  X(k|k)= X(k|k-1)+Kg(k) (Z(k)-H X(k|k-1))
    this->filterValue = predictValue + (Measure - predictValue)*this->kalmanGain;
    //���º������
    //������������  P[n|n]=(1-K[n]*H)*P[n|n-1]
    this->P = (1 - this->kalmanGain*this->H)*this->P;

    return  this->filterValue;
}
