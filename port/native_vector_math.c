/* Authority: C2574A/C25754 through C257DA and actual C1D974. */
#include "native_vector_math.h"

static uint32_t vector_shift(uint32_t value,unsigned count) {
    count&=63u;
    if(!count) return value;
    if(count>=32) return value&UINT32_C(0x80000000)?UINT32_MAX:0;
    return (value>>count)|((value&UINT32_C(0x80000000))?UINT32_MAX<<(32-count):0);
}
static int16_t vector_absolute(int16_t value) {
    return value<0?(int16_t)(uint16_t)(0u-(uint16_t)value):value;
}
int fa18_normalize_native_vector_with_direction(const FA18NativeVectorMath *s,
    int16_t scale,int16_t direction,const int32_t components[3],uint32_t *axis) {
    int16_t input[3],output[3]={0,0,0}; uint32_t length,factor; unsigned shift=8,i;
    if(!s || !s->magnitude || !s->normalized || !components || !axis) return 0;
    for(i=0;i<3;++i) input[i]=(int16_t)components[i];
    if(scale) {
        if(fa18_scene_component_magnitude_window_value(s->table,vector_absolute(input[0]),
            vector_absolute(input[1]),vector_absolute(input[2]),&length,axis)!=0) return 0;
        *s->magnitude=(uint16_t)length;
        if((uint16_t)length) {
            factor=(uint32_t)(int32_t)vector_absolute(scale);
            while((int32_t)factor<=(int32_t)length) {
                /* A zero factor repeats the original upward search forever. */
                if(!factor) return 0;
                factor<<=2; shift=(uint16_t)(shift+2u);
            }
            do {
                factor=vector_shift(factor,2); shift=(uint16_t)(shift-2u);
            } while((int16_t)shift>1 && (int32_t)factor>(int32_t)length);
            factor<<=2; shift=(uint16_t)(shift+2u); factor<<=8;
            { uint32_t quotient=factor/(uint16_t)length;
              if(quotient<=UINT16_MAX) factor=((factor%(uint16_t)length)<<16)|quotient; }
            for(i=0;i<3;++i) {
                uint32_t product=(uint32_t)((int32_t)input[i]*(int16_t)factor);
                output[i]=(int16_t)vector_shift(product,shift);
                if(direction<0) output[i]=(int16_t)(uint16_t)(0u-(uint16_t)output[i]);
            }
        }
    }
    for(i=0;i<3;++i) s->normalized[i]=output[i];
    return 1;
}
int fa18_normalize_native_vector(const FA18NativeVectorMath *s,int16_t scale,
    const int32_t components[3],uint32_t *axis) {
    return fa18_normalize_native_vector_with_direction(s,scale,scale,components,axis);
}
