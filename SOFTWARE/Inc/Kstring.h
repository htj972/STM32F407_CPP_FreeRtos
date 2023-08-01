//
// Created by 53105 on 2023/7/31.
//

#ifndef KOKIRIKA_CODE_CONVERSION_H
#define KOKIRIKA_CODE_CONVERSION_H

#include "sys.h"
#include "string"

#include <utility>


class Kstring: public std::string {
public:
    Kstring()= default;
    //可以使用  = 传递参数

    Kstring(const char * str):std::string(str){}
    static uint8_t unicode_to_utf8(unsigned int codepoint, char* out);
    static std::string GBK_to_utf8(std::string str);
    std::string GBK_to_utf8();
    Kstring& operator<<(const std::string& Str1)
    {
        this->append(Str1);
        return *this;
    }
};


#endif //KOKIRIKA_CODE_CONVERSION_H
