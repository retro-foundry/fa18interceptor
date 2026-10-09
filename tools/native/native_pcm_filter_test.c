/* Exercise the actual native output filter. Optional reference-mixer gain
 * conversion is test-only; it is never applied by the playable game. */
#include "native/audio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc,char **argv) {
    if(argc!=6) return 1;
    const unsigned rate=(unsigned)strtoul(argv[3],NULL,10),block=(unsigned)strtoul(argv[4],NULL,10);
    const int mixed=!strcmp(argv[5],"reference-mix");
    if(!block || block>2048 || (!mixed && strcmp(argv[5],"plain"))) return 1;
    NativePcmFilter filter;
    if(!native_pcm_filter_begin(&filter,rate)) {
        fputs("Unsupported source-validated PCM filter rate\n",stderr);return 2;
    }
    FILE *input=fopen(argv[1],"rb"),*output=fopen(argv[2],"wb");
    if(!input || !output) return 3;
    int16_t samples[2048*2];size_t count;
    unsigned long long frames=0;
    while((count=fread(samples,sizeof *samples,2*block,input))) {
        if(count&1) return 4;
        if(mixed) for(size_t i=0;i<count;++i) {
            int restored=0,matches=0;
            for(int value=samples[i]*3/2-4;value<=samples[i]*3/2+4;++value)
                if(!(value&1) && value*2/3==samples[i]) {restored=value;++matches;}
            if(matches!=1 || restored<-32768 || restored>32767) return 5;
            samples[i]=(int16_t)restored;
        }
        native_pcm_filter_process(&filter,samples,(unsigned)(count/2),rate);
        if(mixed) for(size_t i=0;i<count;++i) samples[i]=(int16_t)(samples[i]*2/3);
        if(fwrite(samples,sizeof *samples,count,output)!=count) return 6;
        frames+=count/2;
    }
    if(ferror(input) || fclose(input) || fclose(output)) return 7;
    printf("Native A500/LED filter: %u Hz, %zu fixed bytes, %llu frames\n",rate,sizeof filter,frames);
    return 0;
}
