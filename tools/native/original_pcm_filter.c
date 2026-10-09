/* Test-only original audio.c filter/RC functions and original SoftFloat.
 * No CPU or chipset, and no link into the playable native runner. */
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "softfloat/softfloat.h"
#define DENORMAL_OFFSET (1E-10)
struct filter_state { float rc1,rc2,rc3,rc4,rc5; };
static float a500e_filter1_a0,a500e_filter2_a0,filter_a0;
static int sound_use_filter,led_filter_on;
enum { FILTER_NONE,FILTER_MODEL_A500,FILTER_MODEL_A1200,FILTER_MODEL_A500_FIXEDONLY };
static double softfloat_tan(double value) {
    /* fpp_native.c:fp_init_native/softfloat_tan use these original
     * conversions and rounding. memcpy is its fp_from/to_double bit copy. */
    uint64_t bits;memcpy(&bits,&value,sizeof bits);
    struct float_status status={0};
    set_floatx80_rounding_precision(80,&status);
    set_float_rounding_mode(float_round_to_zero,&status);
    floatx80 extended=float64_to_floatx80(bits,&status);
    extended=floatx80_tan(extended,&status);
    bits=floatx80_to_float64(extended,&status);
    memcpy(&value,&bits,sizeof value);return value;
}
#include "original_pcm_filter.h"
int main(int argc,char **argv) {
    const int plain=argc==7 && !strcmp(argv[6],"--plain-pcm");
    if(argc!=6 && !plain) return 1;
    FILE *input=fopen(argv[1],"rb"),*output=fopen(argv[2],"wb");
    if(!input || !output) return 2;
    unsigned rate=(unsigned)strtoul(argv[3],NULL,10);
    sound_use_filter=atoi(argv[4]);led_filter_on=atoi(argv[5]);
    if(!rate || sound_use_filter<0 || sound_use_filter>3 || led_filter_on<0 || led_filter_on>1) return 2;
    a500e_filter1_a0=rc_calculate_a0(rate,6200);
    a500e_filter2_a0=rc_calculate_a0(rate,20000);
    filter_a0=rc_calculate_a0(rate,7000);
    printf("%.9g %.9g %.9g\n",a500e_filter1_a0,a500e_filter2_a0,filter_a0);
    struct filter_state states[2]={0};
    int16_t source[2],result[2];
    size_t count;
    while((count=fread(source,sizeof *source,2,input))==2) {
        for(unsigned channel=0;channel<2;++channel) {
            if(plain) {
                result[channel]=(int16_t)filter(source[channel],&states[channel]);
                continue;
            }
            /* driveclick.c:driveclick_mix scales even unfiltered Paula
             * input by 2/3 when its wave resources are initialized. With
             * muted clicks and original x2 FINISH_DATA, invert uniquely;
             * reject ambiguity rather than estimating lost source samples. */
            int restored=0,matches=0;
            for(int value=source[channel]*3/2-4;value<=source[channel]*3/2+4;++value)
                if(!(value&1) && value*2/3==source[channel]) {restored=value;++matches;}
            if(matches!=1 || restored<-32768 || restored>32767) {
                fprintf(stderr,"Cannot uniquely restore original pre-mix sample %d\n",source[channel]);return 3;
            }
            result[channel]=(int16_t)(filter(restored,&states[channel])*2/3);
        }
        if(fwrite(result,sizeof result,1,output)!=1) return 4;
    }
    if(count || ferror(input) || fclose(input) || fclose(output)) return 4;
    return 0;
}
