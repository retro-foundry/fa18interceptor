#include "native_vector_math.h"
#include <assert.h>

int main(void) {
    uint8_t bytes[2]={0x40,0}; uint16_t magnitude=0x1234;
    int16_t normalized[3]={11,22,33}; int32_t input[3]={-256,0,0};
    uint32_t axis=0xabcdef01;
    PortFieldWindow table={.bytes=bytes,.byte_count=2};
    FA18NativeVectorMath math={&table,&magnitude,normalized};
    assert(fa18_normalize_native_vector(&math,192,input,&axis));
    assert(magnitude==256 && normalized[0]==-192 && !normalized[1] && !normalized[2]);
    assert(fa18_normalize_native_vector(&math,-192,input,&axis) && normalized[0]==192);
    assert(fa18_normalize_native_vector_with_direction(&math,-192,0,input,&axis) && normalized[0]==-192);
    math.table=NULL; magnitude=0x1234; axis=0xabcdef01;
    assert(fa18_normalize_native_vector(&math,0,input,&axis));
    assert(magnitude==0x1234 && axis==0xabcdef01 && !normalized[0]);
    normalized[0]=11;
    assert(!fa18_normalize_native_vector(&math,192,input,&axis) && normalized[0]==11);
    math.table=&table; input[0]=1; normalized[0]=11; normalized[1]=22; normalized[2]=33;
    /* Original -32768 scale reaches a repeating zero-factor search. Native
     * failure retains magnitude publication and leaves final output unwritten. */
    assert(!fa18_normalize_native_vector(&math,INT16_MIN,input,&axis));
    assert(magnitude==1 && normalized[0]==11 && normalized[1]==22 && normalized[2]==33);
    return 0;
}
