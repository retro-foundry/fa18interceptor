#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ofs.h"
#include "guest_memory.h"
#include "hunk.h"
static void word(uint8_t *block,unsigned at,uint32_t value) { amiga_store_be32(block+4*at,value); }
static void seal(uint8_t *block) {
    word(block,5,0); uint32_t sum=0;
    for (unsigned i=0;i<128;++i) sum+=amiga_be32(block+4*i);
    word(block,5,0u-sum);
}
static void named(uint8_t *block,uint32_t key,uint32_t parent,int32_t type,const char *name) {
    word(block,0,2); word(block,1,key); word(block,125,parent); word(block,127,(uint32_t)type);
    block[432]=(uint8_t)strlen(name); memcpy(block+433,name,strlen(name));
}
static int count_entry(void *context,const AmigaOfsEntry *entry) {
    unsigned *count=context; assert(entry->type==2); ++*count; return 1;
}
static void contracts(void) {
    AmigaOfs disk={calloc(8,512),8*512}; assert(disk.image);
    uint8_t *root=disk.image+4*512,*dir=disk.image+5*512,*file=disk.image+6*512,*data=disk.image+7*512;
    memcpy(disk.image,"DOS\0",4);
    named(root,0,0,1,"disk"); word(root,3,72); word(root,6+9,5);
    named(dir,5,4,2,"D"); word(dir,6+29,6);
    named(file,6,5,-3,"x"); word(file,4,7); word(file,81,3);
    word(file,80,15); word(file,105,7); word(file,106,12); word(file,107,44);
    word(data,0,8); word(data,1,6); word(data,2,1); word(data,3,3);
    data[24]='A'; data[25]=0; data[26]=0xFF;
    seal(root); seal(dir); seal(file); seal(data);
    AmigaOfsEntry info;
    assert(amiga_ofs_find(&disk,"d/X",&info) && info.block==6 && info.size==3 && info.type==-3);
    assert(info.protection==15 && info.days==7 && info.minutes==12 && info.ticks==44);
    size_t size; uint8_t *bytes=amiga_ofs_read(&disk,"D/x",&size);
    assert(bytes && size==3 && !memcmp(bytes,data+24,3)); free(bytes);
    unsigned entries=0; assert(amiga_ofs_list(&disk,"",count_entry,&entries) && entries==1);
    uint8_t out[8]; memset(out,0xA5,sizeof out);
    assert(amiga_ofs_read_range(&disk,"D/x",2,out,sizeof out,&size) && size==1 && out[0]==0xFF && out[1]==0xA5);
    assert(amiga_ofs_read_range(&disk,"D/x",SIZE_MAX,NULL,0,&size) && !size);
    assert(!amiga_ofs_read(&disk,"D",&size) && !size);
    assert(!amiga_ofs_read_range(&disk,"D/missing",0,out,sizeof out,&size) && !size);
    assert(!amiga_ofs_find(&disk,"D/x/sub",&info));
    assert(!amiga_ofs_find(&disk,"D//x",&info));
    word(data,2,2); seal(data); /* Correct checksum, wrong data sequence. */
    assert(!amiga_ofs_read(&disk,"D/x",&size));
    word(data,2,1); word(data,4,7); seal(data); /* Extra/cyclic data chain. */
    assert(!amiga_ofs_read(&disk,"D/x",&size));
    word(data,4,0); seal(data); data[24]^=1; /* Bad data checksum. */
    assert(!amiga_ofs_read(&disk,"D/x",&size)); data[24]^=1;
    word(file,4,UINT32_MAX); seal(file); /* Must not wrap into the boot block. */
    assert(!amiga_ofs_read(&disk,"D/x",&size));
    word(dir,124,5); seal(dir); /* Hash cycle terminates with failure. */
    entries=0; assert(!amiga_ofs_list(&disk,"",count_entry,&entries) && entries<=8);
    amiga_ofs_close(&disk);
}
int main(int argc,char **argv) {
    contracts();
    if (argc==1) { puts("OFS metadata, short-read, corruption and cycle contracts pass"); return 0; }
    assert(argc==4); AmigaOfs disk; AmigaOfsEntry info; size_t size;
    assert(amiga_ofs_open(&disk,argv[1])); assert(amiga_ofs_find(&disk,argv[2],&info));
    uint8_t *data=amiga_ofs_read(&disk,argv[2],&size); assert(data && size==info.size);
    FILE *out=fopen(argv[3],"wb"); assert(out);
    assert(fwrite(data,1,size,out)==size && !fclose(out));
    printf("{\"block\":%u,\"size\":%u,\"type\":%d,\"protection\":%u,\"days\":%u,\"minutes\":%u,\"ticks\":%u}\n",
        info.block,info.size,info.type,info.protection,info.days,info.minutes,info.ticks);
    free(data); amiga_ofs_close(&disk); return 0;
}
