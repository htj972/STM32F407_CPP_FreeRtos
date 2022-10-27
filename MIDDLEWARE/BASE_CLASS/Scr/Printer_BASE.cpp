/**
* @Author kokirika
* @Name PRINTER_BASE
* @Date 2022-10-26
**/

#include "Printer_BASE.h"
#include <cstdarg>

void Printer_BASE::config_Paper(uint16_t width, uint16_t length) {
    this->Width=width;
    this->Length=length;
}

void Printer_BASE::config_Paper_width(uint16_t width) {
    this->Width=width;
}

void Printer_BASE::config_Paper_length(uint16_t length) {
    this->Length=length;
}

uint16_t Printer_BASE::get_Paper_width() const {
    return this->Width;
}

uint16_t Printer_BASE::get_Paper_length() const {
    return this->Length;
}

void Printer_BASE::write(const char *str, uint16_t len) {
    this->write((uint8_t *)str,len);
}

void Printer_BASE::write(uint8_t *str, uint16_t len) {

}

void Printer_BASE::write(const std::string &String) {
    this->write(String.c_str(),String.length());
}

void Printer_BASE::write(int integer) {

}

uint16_t Printer_BASE::print(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const auto len = vsnprintf(nullptr, 0, fmt, args);
    va_end(args);
    std::string r;
    r.resize(static_cast<size_t>(len) + 1);
    va_start(args, fmt);
    vsnprintf(&r.front(), len + 1, fmt, args);
    va_end(args);
    r.resize(static_cast<size_t>(len));
    this->write(r);
    return r.length();
}

uint16_t Printer_BASE::print(const std::string &String) {
    this->write(String);
    return String.length();
}

uint16_t Printer_BASE::print(const char *s) {
    return this->print("%s",s);
}

uint16_t Printer_BASE::print(char s) {
    return this->print("%c",s);
}

uint16_t Printer_BASE::print(int integer) {
    return this->print("%d",integer);
}


