//
// Created by 53105 on 2023/7/31.
//

#ifndef KOKIRIKA_CODE_CONVERSION_H
#define KOKIRIKA_CODE_CONVERSION_H

#include "sys.h"
#include "string"

#include <utility>


class Kstring: public std::string {
private:
    static std::string GBK_to_utf8_one(std::string &str);
public:
    Kstring()= default;
    //可以使用  = 传递参数

    Kstring(const char * str):std::string(str){}
    Kstring(std::string str):std::string(std::move(str)){}
    static uint8_t unicode_to_utf8(unsigned int codepoint, char* out);
    static std::string GBK_to_utf8(std::string gbk_str);
    std::string GBK_to_utf8();
    Kstring& operator<<(const std::string& Str1)
    {
        this->append(Str1);
        return *this;
    }
    Kstring& operator<<(int integer)
    {
        this->append(std::to_string(integer));
        return *this;
    }
    Kstring& operator<<(const char *s)
    {
        this->append(s);
        return *this;
    }
    Kstring& operator<<(char s)
    {
        this->append(1,s);
        return *this;
    }
    Kstring& operator<<(double s)
    {
        this->append(std::to_string(s));
        return *this;
    }
    Kstring& operator=(const std::string& Str)
    {
        this->assign(Str);
        return *this;
    }
    Kstring& operator=(std::string Str)
    {
        this->assign(Str);
        return *this;
    }
    Kstring& operator=(const char *s)
    {
        this->assign(s);
        return *this;
    }
    Kstring& operator=(char s)
    {
        this->assign(1,s);
        return *this;
    }
    Kstring& operator=(int integer)
    {
        this->assign(std::to_string(integer));
        return *this;
    }
    Kstring& operator=(double s)
    {
        this->assign(std::to_string(s));
        return *this;
    }
    Kstring& operator+=(const std::string& Str)
    {
        this->append(Str);
        return *this;
    }
    Kstring& operator+=(const char *s)
    {
        this->append(s);
        return *this;
    }
    Kstring& operator+=(char s)
    {
        this->append(1,s);
        return *this;
    }

    Kstring& operator+=(int integer)
    {
        this->append(std::to_string(integer));
        return *this;
    }
    Kstring& operator+=(double s)
    {
        this->append(std::to_string(s));
        return *this;
    }

};


#endif //KOKIRIKA_CODE_CONVERSION_H
