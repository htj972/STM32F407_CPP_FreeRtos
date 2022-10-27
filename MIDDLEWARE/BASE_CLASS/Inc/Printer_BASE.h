/**
* @Author kokirika
* @Name PRINTER_BASE
* @Date 2022-10-26
**/

#ifndef KOKIRIKA_PRINTER_BASE_H
#define KOKIRIKA_PRINTER_BASE_H

#include "sys.h"
#include "string"

class Printer_BASE {
private:
    uint16_t Width{};
    uint16_t Length{};
public:
    void config_Paper(uint16_t width,uint16_t length);
    void config_Paper_width(uint16_t width);
    void config_Paper_length(uint16_t length);
    uint16_t get_Paper_width() const;
    uint16_t get_Paper_length() const;

    virtual void write(const char *str,uint16_t len);
    virtual void write(uint8_t *str,uint16_t len);
    virtual void write(const std::string& String);
    virtual void write(int integer);
    uint16_t print(const char *fmt, ...);
    uint16_t print(const std::string& String);
    uint16_t print(const char *s);
    uint16_t print(char s);
    uint16_t print(int integer);

};


#endif //KOKIRIKA_PRINTER_BASE_H
