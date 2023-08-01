//
// Created by 53105 on 2023/7/31.
//

#include "Kstring.h"
#include "ff.h"



uint8_t Kstring::unicode_to_utf8(unsigned int codepoint, char* out) {
    if (codepoint <= 0x7f) {
        out[0] = codepoint;
        out[1] = '\0';
        return 1;
    } else if (codepoint <= 0x7ff) {
        out[0] = 0xc0 | (codepoint >> 6);
        out[1] = 0x80 | (codepoint & 0x3f);
        out[2] = '\0';
        return 2;
    } else if (codepoint <= 0xffff) {
        out[0] = 0xe0 | (codepoint >> 12);
        out[1] = 0x80 | ((codepoint >> 6) & 0x3f);
        out[2] = 0x80 | (codepoint & 0x3f);
        out[3] = '\0';
        return 3;
    } else {
        out[0] = 0xf0 | (codepoint >> 18);
        out[1] = 0x80 | ((codepoint >> 12) & 0x3f);
        out[2] = 0x80 | ((codepoint >> 6) & 0x3f);
        out[3] = 0x80 | (codepoint & 0x3f);
        out[4] = '\0';
        return 4;
    }
}

std::string Kstring::GBK_to_utf8(std::string str){
    std::string RET;
    char utf8_str[5];
    uint8_t len;
    for (int i = 0; i < str.length()/2; i++) {
        len=unicode_to_utf8(ff_convert(((str[i*2]<<8)+str[i*2+1]),1), utf8_str);
        for (int j = 0;j<len; j++) {
            RET+=utf8_str[j];
        }
    }
    return RET;
}

std::string Kstring::GBK_to_utf8() {
    return GBK_to_utf8(*this);
}
