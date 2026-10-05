#include "hex_field.h"

static int signed_byte(uint8_t value) { return value<128?value:(int)value-256; }

int fa18_format_native_hex_field(uint8_t *buffer,size_t bytes,size_t anchor,
                                  uint32_t value,uint8_t width) {
    int i,count=signed_byte(width);
    int64_t cursor;
    if(!buffer || (uintmax_t)anchor>(uintmax_t)INT64_MAX-128) return 0;
    cursor=(int64_t)anchor+count;
    for(i=0;i<count;++i) {
        uint8_t digit=(uint8_t)((value&15u)+'0');
        if(cursor<0 || (uint64_t)cursor>=bytes) return 0;
        if(digit>'9') digit+=7;
        buffer[(size_t)cursor--]=digit;
        value>>=4;
    }
    ++cursor;
    count=signed_byte((uint8_t)(width-1u));
    for(i=0;i<count;++i) {
        if(cursor<0 || (uint64_t)cursor>=bytes) return 0;
        if(buffer[(size_t)cursor]!='0') break;
        buffer[(size_t)cursor++]=' ';
    }
    return 1;
}
