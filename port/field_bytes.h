#ifndef PORT_FIELD_BYTES_H
#define PORT_FIELD_BYTES_H
#include <stddef.h>
#include <stdint.h>

/* A byte of a caller-owned integer VALUE, independent of host byte order.
 * Exactly one typed owner must be supplied. shift is 0 for a byte, 0/8 for
 * a word, 0/8/16/24 for a long. Owners must remain live at stable addresses.
 * No packed RAM, address lookup, CPU state or game policy is provided. */
/* An ordinary logical word can own a resolved enum/record reference rather
 * than an integer cell. Accessors must implement its actual value semantics;
 * they are not addresses or CPU/bus callbacks. */
typedef struct {
    int (*read)(void *context,uint16_t *value);
    int (*write)(void *context,uint16_t value);
    void *context;
} PortFieldWordValue;
typedef struct {
    uint8_t *byte;
    int16_t *word;
    unsigned shift;
    uint16_t *unsigned_word;
    uint32_t *longword;
    int32_t *signed_longword;
    PortFieldWordValue *word_value;
} PortFieldByte;

static inline int port_field_byte_valid(const PortFieldByte *field) {
    unsigned owners;
    if(!field) return 0;
    owners=(field->byte!=NULL)+(field->word!=NULL)+(field->unsigned_word!=NULL)+
        (field->longword!=NULL)+(field->signed_longword!=NULL)+(field->word_value!=NULL);
    if(owners!=1 || field->shift%8) return 0;
    if(field->byte) return field->shift==0;
    if(field->word_value) return field->word_value->read && field->word_value->write && field->shift<=8;
    if(field->word || field->unsigned_word) return field->shift<=8;
    return field->shift<=24;
}
static inline int port_read_field_byte(const PortFieldByte *field,uint8_t *value) {
    uint32_t bits;
    if(!value || !port_field_byte_valid(field)) return 0;
    if(field->byte) bits=*field->byte;
    else if(field->word) bits=(uint16_t)*field->word;
    else if(field->unsigned_word) bits=*field->unsigned_word;
    else if(field->longword) bits=*field->longword;
    else if(field->word_value) {
        uint16_t word;
        if(!field->word_value->read(field->word_value->context,&word)) return 0;
        bits=word;
    }
    else bits=(uint32_t)*field->signed_longword;
    *value=(uint8_t)(bits>>field->shift);
    return 1;
}
static inline int port_write_field_byte(const PortFieldByte *field,uint8_t value) {
    uint32_t bits,mask;
    if(!port_field_byte_valid(field)) return 0;
    if(field->byte) { *field->byte=value; return 1; }
    if(field->word) bits=(uint16_t)*field->word;
    else if(field->unsigned_word) bits=*field->unsigned_word;
    else if(field->longword) bits=*field->longword;
    else if(field->word_value) {
        uint16_t word;
        if(!field->word_value->read(field->word_value->context,&word)) return 0;
        bits=word;
    }
    else bits=(uint32_t)*field->signed_longword;
    mask=UINT32_C(0xff)<<field->shift;
    bits=(bits&~mask)|((uint32_t)value<<field->shift);
    if(field->word) *field->word=bits<0x8000u?(int16_t)bits:(int16_t)((int32_t)bits-0x10000);
    else if(field->unsigned_word) *field->unsigned_word=(uint16_t)bits;
    else if(field->longword) *field->longword=bits;
    else if(field->word_value) return field->word_value->write(field->word_value->context,(uint16_t)bits);
    else *field->signed_longword=bits<UINT32_C(0x80000000)?(int32_t)bits:
        (int32_t)((int64_t)bits-INT64_C(4294967296));
    return 1;
}
/* Ordered fill; aliases are read/written live. Missing owners return 0 and
 * preserve prior writes. There are no observers between word-byte writes. */
static inline int port_fill_field_bytes(const PortFieldByte *fields,size_t count,uint8_t value) {
    size_t i;
    if(!fields) return 0;
    for(i=0;i<count;++i) if(!port_write_field_byte(fields+i,value)) return 0;
    return 1;
}
static inline int port_field_word_pair_valid(const PortFieldByte *fields) {
    if(!fields || !port_field_byte_valid(fields) || !port_field_byte_valid(fields+1)) return 0;
    if(fields[0].word_value || fields[1].word_value)
        return fields[0].word_value==fields[1].word_value && fields[0].shift==8 && fields[1].shift==0;
    return 1;
}
/* One whole logical-word write; never read or publish an intermediate value.
 * Plain integer/byte fields have no observers between their two writes. */
static inline int port_fill_field_words(const PortFieldByte *fields,size_t count,uint16_t value) {
    size_t i;
    if(!fields) return 0;
    for(i=0;i<count;++i) {
        const PortFieldByte *pair=fields+2*i;
        if(!port_field_word_pair_valid(pair)) return 0;
        if(pair->word_value) {
            if(!pair->word_value->write(pair->word_value->context,value)) return 0;
        } else if(!port_write_field_byte(pair,(uint8_t)(value>>8)) ||
                  !port_write_field_byte(pair+1,(uint8_t)value)) return 0;
    }
    return 1;
}
#endif
