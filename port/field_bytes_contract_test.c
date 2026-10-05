#include "field_bytes.h"
#include <assert.h>

typedef struct { uint16_t value; unsigned reads,writes; } LogicalWord;
static int read_logical(void *context,uint16_t *value) {
    LogicalWord *word=context; ++word->reads; *value=word->value; return 1;
}
static int write_logical(void *context,uint16_t value) {
    LogicalWord *word=context;
    if(value%512) return 0;
    ++word->writes; word->value=value; return 1;
}

int main(void) {
    static const uint32_t longs[]={0,1,0x7fffffff,0x80000000,0xffffffff,0xdeadbeef};
    uint16_t unsigned_word; int16_t word;
    uint32_t longword; int32_t signed_longword;
    uint8_t value,byte=0; unsigned i,j,k;
    PortFieldByte field={0};
    assert(!port_write_field_byte(&field,7));
    assert(!port_read_field_byte(&field,&byte) && !byte);
    field=(PortFieldByte){.byte=&byte,.shift=8}; assert(!port_field_byte_valid(&field));
    field=(PortFieldByte){.byte=&byte,.word=&word}; assert(!port_field_byte_valid(&field));
    for(i=0;i<65536;++i) for(j=0;j<2;++j) {
        uint16_t expected;
        unsigned_word=(uint16_t)i;
        word=i<0x8000u?(int16_t)i:(int16_t)((int)i-0x10000);
        field=(PortFieldByte){.word=&word,.shift=8*j};
        assert(port_read_field_byte(&field,&value) && value==(uint8_t)(i>>(8*j)));
        value=(uint8_t)(i^0xa5);
        expected=(uint16_t)((i&~(0xffu<<(8*j)))|((unsigned)value<<(8*j)));
        assert(port_write_field_byte(&field,value) && (uint16_t)word==expected);
        field=(PortFieldByte){.unsigned_word=&unsigned_word,.shift=8*j};
        assert(port_write_field_byte(&field,value) && unsigned_word==expected);
    }
    for(i=0;i<sizeof longs/sizeof longs[0];++i) for(j=0;j<4;++j) for(k=0;k<256;++k) {
        uint32_t expected=(longs[i]&~(UINT32_C(0xff)<<(8*j)))|((uint32_t)k<<(8*j));
        longword=longs[i];
        signed_longword=longword<0x80000000u?(int32_t)longword:(int32_t)((int64_t)longword-INT64_C(4294967296));
        field=(PortFieldByte){.longword=&longword,.shift=8*j};
        assert(port_read_field_byte(&field,&value) && value==(uint8_t)(longword>>(8*j)));
        assert(port_write_field_byte(&field,(uint8_t)k) && longword==expected);
        field=(PortFieldByte){.signed_longword=&signed_longword,.shift=8*j};
        assert(port_write_field_byte(&field,(uint8_t)k) && (uint32_t)signed_longword==expected);
    }
    {
        PortFieldByte aliases[3]={{.unsigned_word=&unsigned_word,.shift=8},
                                 {.unsigned_word=&unsigned_word},{0}};
        unsigned_word=0xffff;
        assert(!port_fill_field_bytes(aliases,3,0) && !unsigned_word);
        assert(!port_fill_field_bytes(NULL,0,0));
    }
    field=(PortFieldByte){.longword=&longword,.shift=32}; assert(!port_field_byte_valid(&field));
    field=(PortFieldByte){.word=&word,.shift=1}; assert(!port_field_byte_valid(&field));
    {
        LogicalWord logical={0x101,0,0};
        PortFieldWordValue owner={read_logical,write_logical,&logical};
        PortFieldByte pair[2]={{.word_value=&owner,.shift=8},{.word_value=&owner}};
        assert(port_fill_field_words(pair,1,0));
        assert(!logical.value && !logical.reads && logical.writes==1);
        logical.value=0x200;
        assert(port_read_field_byte(pair,&value) && value==2);
        assert(port_write_field_byte(pair,4) && logical.value==0x400);
        pair[1]=(PortFieldByte){.byte=&byte};
        assert(!port_fill_field_words(pair,1,0) && logical.value==0x400);
        assert(!port_fill_field_words(NULL,0,0));
    }
    return 0;
}
