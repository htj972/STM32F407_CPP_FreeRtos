/**
* @Author kokirika
* @Name Modbus
* @Date 2022-09-20
**/

#ifndef Kokirika_FERTILIZER_H
#define Kokirika_FERTILIZER_H

#include "sys.h"
#include <string>

using namespace std;

class fertilizer {
public:
    enum WATER_MODE : uint8_t {
        press = 0,//压力
        power,  //功率
    };
    enum fertilizer_MODE : uint8_t {
        flow = 0,//流量
        ratio,  //比例
    };
private:
    uint8_t water_mode=WATER_MODE::press;
    float water_value=1;
    uint8_t fertilizer_mode=fertilizer_MODE::flow;
    float fertilizer_value=1;
    float pial_fertilizer=0;//肥水比
    //肥质量
    float fertilizer_mass=0;
    //桶容积
    float bucket_volume=0;
    //桶剩余量
    float bucket_remaining=0;
    //桶剩余时间
    float bucket_remaining_time=0;
    //肥设置标志
    bool fertilizer_flag=false;
    //水设置标志
    bool water_flag=false;
public:
    //设置水泵模式
    void set_water_mode(uint8_t mode, float value);
    //设置肥泵模式
    void set_fertilizer_mode(uint8_t mode, float value);
    //配置施肥桶参数
    void set_pial_fertilizer(float pial,float volume);
    //获取指令参数
    void get_cmd_str(const string& cmd);
    //获取肥设置参数
    bool get_fertilizer_flag(uint16_t* mode, float* value);
    //获取水设置参数
    bool get_water_flag(uint16_t* mode, float* value);



    //设置配肥命令
    string set_fertilizer_cmd() const;


    string fertilizer_data() const;

};


#endif //Kokirika_FERTILIZER_H
