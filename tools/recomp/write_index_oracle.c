/* Independent sorted-sequence oracle for raw-address first/last write queries. */
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
typedef struct { uint32_t address; uint8_t old; } LogEntry;
#include "recomp_write_index.h"
typedef struct { uint32_t address; size_t position; } Ordered;
static uint32_t seed=0xc1017eu;
static uint32_t random_value(void) { seed^=seed<<13; seed^=seed>>17; seed^=seed<<5; return seed; }
static int order(const void *a,const void *b) {
    const Ordered *x=a,*y=b;
    if(x->address!=y->address) return x->address<y->address?-1:1;
    return x->position<y->position?-1:x->position!=y->position;
}
int main(void) {
    LogEntry log[2048]; Ordered sorted[2048]; unsigned scenario;
    for(scenario=0;scenario<4096;++scenario) {
        size_t count=scenario%2049u,i; WriteIndex index;
        for(i=0;i<count;++i) {
            uint32_t value=random_value();
            log[i].address=(scenario%3u)==0?(value&31u)*0x10000u:
                (scenario%3u)==1?value&0xffffffu:(value&31u)|0xc00000u;
            if(i==0) log[i].address=0;
            if(i==1) log[i].address=0xffffffu;
            sorted[i].address=log[i].address; sorted[i].position=i+1;
        }
        qsort(sorted,count,sizeof *sorted,order); index=write_index_build(log,count);
        for(i=0;i<count;) {
            size_t j=i+1;
            while(j<count && sorted[j].address==sorted[i].address) ++j;
            if(write_index_first(&index,sorted[i].address)!=sorted[i].position ||
                write_index_last(&index,sorted[i].address)!=sorted[j-1].position) return 1;
            i=j;
        }
        /* Every indexed key is 24-bit; these absent keys also collide with
         * populated buckets. Address zero is a real key, not an empty slot. */
        for(i=0;i<64;++i) if(write_index_first(&index,0x1000000u+(uint32_t)i) ||
            write_index_last(&index,0x1000000u+(uint32_t)i)) return 1;
        write_index_free(&index);
    }
    puts("write index: 4096 sorted-sequence cases matched first/last writes, raw aliases, zero/absent keys and collisions");
    return 0;
}
