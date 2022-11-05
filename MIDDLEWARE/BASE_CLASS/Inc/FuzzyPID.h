/**
* @Author kokirika
* @Name FUZZYPID
* @Date 2022-11-02
**/

#ifndef KOKIRIKA_FUZZYPID_H
#define KOKIRIKA_FUZZYPID_H


class FuzzyPID {
public:
    FuzzyPID();
    ~FuzzyPID()= default;
    void Get_grad_membership(float erro, float erro_c);
    static float Quantization(float maximum, float minimum, float x);
    static float Inverse_quantization(float maximum, float minimum, float qvalues);
    void GetSumGrad();
    void GetOUT();
    float FuzzyPIDcontroller(float e_max, float e_min, float ec_max, float ec_min, float kp_max, float kp_min, float erro, float erro_c, float ki_max, float ki_min,float kd_max, float kd_min,float erro_pre, float errp_ppre);
    const int  num_area = 8; //�����������
    //float e_max;  //�������ֵ
    //float e_min;  //�����Сֵ
    //float ec_max;  //���仯���ֵ
    //float ec_min;  //���仯��Сֵ
    //float kp_max, kp_min;
    float e_membership_values[7] = {-3,-2,-1,0,1,2,3}; //����e������ֵ
    float ec_membership_values[7] = { -3,-2,-1,0,1,2,3 };//����de/dt������ֵ
    float kp_menbership_values[7] = { -3,-2,-1,0,1,2,3 };//�������kp������ֵ
    float ki_menbership_values[7] = { -3,-2,-1,0,1,2,3 }; //�������ki������ֵ
    float kd_menbership_values[7] = { -3,-2,-1,0,1,2,3 };  //�������kd������ֵ
    float fuzzyoutput_menbership_values[7] = { -3,-2,-1,0,1,2,3 };

    //int menbership_values[7] = {-3,-};
    float kp;                       //PID����kp
    float ki;                       //PID����ki
    float kd;                       //PID����kd
    float qdetail_kp;               //����kp��Ӧ�����е�ֵ
    float qdetail_ki;               //����ki��Ӧ�����е�ֵ
    float qdetail_kd;               //����kd��Ӧ�����е�ֵ
    float qfuzzy_output;
    float detail_kp{};                //�������kp
    float detail_ki{};                //�������ki
    float detail_kd{};                //�������kd
    float fuzzy_output;
    float qerro{};                    //����e��Ӧ�����е�ֵ
    float qerro_c{};                  //����de/dt��Ӧ�����е�ֵ
    float errosum;
    float e_gradmembership[2]{};      //����e��������
    float ec_gradmembership[2]{};     //����de/dt��������
    int e_grad_index[2]{};            //����e�������ڹ���������
    int ec_grad_index[2]{};           //����de/dt�������ڹ���������
    float gradSums[7] = {0,0,0,0,0,0,0};
    float KpgradSums[7] = { 0,0,0,0,0,0,0 };   //�������kp�ܵ�������
    float KigradSums[7] = { 0,0,0,0,0,0,0 };   //�������ki�ܵ�������
    float KdgradSums[7] = { 0,0,0,0,0,0,0 };   //�������kd�ܵ�������
    int NB = -3, NM = -2, NS = -1, ZO = 0, PS = 1, PM = 2, PB = 3; //��������ֵ

    int  Kp_rule_list[7][7] = { {PB,PB,PM,PM,PS,ZO,ZO},     //kp�����
                                {PB,PB,PM,PS,PS,ZO,NS},
                                {PM,PM,PM,PS,ZO,NS,NS},
                                {PM,PM,PS,ZO,NS,NM,NM},
                                {PS,PS,ZO,NS,NS,NM,NM},
                                {PS,ZO,NS,NM,NM,NM,NB},
                                {ZO,ZO,NM,NM,NM,NB,NB} };

    int  Ki_rule_list[7][7] = { {NB,NB,NM,NM,NS,ZO,ZO},     //ki�����
                                {NB,NB,NM,NS,NS,ZO,ZO},
                                {NB,NM,NS,NS,ZO,PS,PS},
                                {NM,NM,NS,ZO,PS,PM,PM},
                                {NM,NS,ZO,PS,PS,PM,PB},
                                {ZO,ZO,PS,PS,PM,PB,PB},
                                {ZO,ZO,PS,PM,PM,PB,PB} };

    int  Kd_rule_list[7][7] = { {PS,NS,NB,NB,NB,NM,PS},    //kd�����
                                {PS,NS,NB,NM,NM,NS,ZO},
                                {ZO,NS,NM,NM,NS,NS,ZO},
                                {ZO,NS,NS,NS,NS,NS,ZO},
                                {ZO,ZO,ZO,ZO,ZO,ZO,ZO},
                                {PB,NS,PS,PS,PS,PS,PB},
                                {PB,PM,PM,PM,PS,PS,PB} };

    int  Fuzzy_rule_list[7][7] = { {PB,PB,PB,PB,PM,ZO,ZO},
                                   {PB,PB,PB,PM,PM,ZO,ZO},
                                   {PB,PM,PM,PS,ZO,NS,NM},
                                   {PM,PM,PS,ZO,NS,NM,NM},
                                   {PS,PS,ZO,NM,NM,NM,NB},
                                   {ZO,ZO,ZO,NM,NB,NB,NB},
                                   {ZO,NS,NB,NB,NB,NB,NB}};

};


#endif //KOKIRIKA_FUZZYPID_H
