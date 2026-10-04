#include "host_compat.h"
#include "host_graphics.h"
#include "hunk.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static void word(uint8_t *p,uint16_t v) { p[0]=(uint8_t)(v>>8); p[1]=(uint8_t)v; }
static void block_word(uint8_t *p,unsigned i,uint32_t v) { amiga_store_be32(p+4*i,v); }
static void seal(uint8_t *p) {
    block_word(p,5,0); uint32_t sum=0;
    for (unsigned i=0;i<128;++i) sum+=amiga_be32(p+i*4);
    block_word(p,5,0u-sum);
}
static void named(uint8_t *p,uint32_t key,uint32_t parent,int32_t type,const char *name) {
    block_word(p,0,2); block_word(p,1,key); block_word(p,125,parent); block_word(p,127,(uint32_t)type);
    p[432]=(uint8_t)strlen(name); memcpy(p+433,name,strlen(name));
}
static uint16_t copper_value(const uint8_t *list,unsigned reg) {
    for (unsigned i=0;i<128;++i,list+=4) {
        if (amiga_be16(list)==reg) return amiga_be16(list+2);
        if (amiga_be16(list)==0xFFFF) break;
    }
    assert(!"missing Copper move"); return 0;
}
int main(void) {
    AmigaOfs disk={calloc(8,512),8*512}; assert(disk.image);
    memcpy(disk.image,"DOS\0",4);
    uint8_t *root=disk.image+4*512,*dir=disk.image+5*512,*file=disk.image+6*512,*data=disk.image+7*512;
    named(root,0,0,1,"disk"); block_word(root,3,72); block_word(root,15,5);
    named(dir,5,4,2,"D"); block_word(dir,35,6);
    named(file,6,5,-3,"x"); block_word(file,4,7); block_word(file,81,3);
    block_word(data,0,8); block_word(data,1,6); block_word(data,2,1); block_word(data,3,3);
    memcpy(data+24,"abc",3); seal(root); seal(dir); seal(file); seal(data);
    uint8_t *chip=malloc(65536),*fast=malloc(65536); assert(chip && fast);
    memset(chip,0xA5,65536); memset(fast,0xA5,65536);
    AmigaGuestBank banks[]={{0,65536,AMIGA_MEMORY_CHIP,chip},{0x200000,65536,AMIGA_MEMORY_FAST,fast}};
    AmigaGuestMemory memory={banks,2}; AmigaHostRegion reserved[]={{0,0x100,0},{0x200000,0x100,0}};
    AmigaHostCompat *c=calloc(1,sizeof *c); assert(c);
    assert(amiga_host_init(c,&memory,&disk,"host-compat-test-saves",reserved,2));
    uint32_t available=amiga_host_available(c,0),a=amiga_host_alloc(c,9,0x10002),b=amiga_host_alloc(c,9,2);
    assert(a==0x100 && b==0x110 && chip[a]==0 && chip[a+15]==0 && chip[b]==0xA5);
    uint32_t f=amiga_host_alloc(c,31,1); assert(f==0x200100);
    assert(!amiga_host_alloc(c,65536,2) && !amiga_host_alloc(c,8,6));
    assert(!amiga_host_free(c,a,8) && amiga_host_free(c,b,9) && amiga_host_free(c,a,9));
    assert(amiga_host_free(c,f,31) && amiga_host_available(c,0)==available);
    assert(amiga_host_available(c,0x20002)==65280);
    uint32_t lib=amiga_host_library(c,"graphics.library",29); assert(lib && !amiga_host_library(c,"missing.library",0));
    uint8_t *header=amiga_guest_range(&c->memory,lib,34);
    assert(amiga_be16(header+20)==34 && amiga_be16(header+32)==1);
    assert(amiga_host_library(c,"graphics.library",34)==lib && amiga_be16(header+32)==2);
    assert(!amiga_host_library(c,"graphics.library",35));
    uint32_t h=amiga_host_open(c,"DF0:D/X",1005); assert(h);
    char bytes[8]={0}; assert(amiga_host_read(c,h,bytes,8)==3 && !memcmp(bytes,"abc",3));
    assert(amiga_host_read(c,h,bytes,8)==0 && amiga_host_seek(c,h,-1,1)==3);
    assert(amiga_host_read(c,h,bytes,8)==1 && bytes[0]=='c'); assert(amiga_host_file_close(c,h));
    assert(!amiga_host_open(c,"../outside",1006) && !amiga_host_open(c,"D/missing",1005));
    h=amiga_host_open(c,"pilot",1006); assert(h);
    assert(amiga_host_write(c,h,"save\0format",11)==11 && amiga_host_seek(c,h,0,-1)==11);
    assert(amiga_host_read(c,h,bytes,4)==4 && !memcmp(bytes,"save",4)); assert(amiga_host_file_close(c,h));
    h=amiga_host_open(c,"PILOT",1005); assert(h && amiga_host_read(c,h,bytes,4)==4 && !memcmp(bytes,"save",4));
    assert(amiga_host_file_close(c,h));
    uint32_t lock=amiga_host_lock(c,"D"); assert(lock); uint8_t fib[260];
    uint8_t disk_info[40]; memset(disk_info,0xA5,sizeof disk_info);
    assert(amiga_host_info(c,lock,disk_info,sizeof disk_info));
    assert(amiga_be32(disk_info+8)==82 && amiga_be32(disk_info+20)==512 && amiga_be32(disk_info+24)==0x444F5300);
    assert(amiga_be32(disk_info+36)==0xA5A5A5A5 && !amiga_host_info(c,lock,disk_info,35));
    assert(amiga_host_examine(c,lock,fib,260) && amiga_be32(fib+4)==2);
    assert(amiga_host_exnext(c,lock,fib,260) && !strcmp((char *)fib+8,"x") && amiga_be32(fib+124)==3);
    assert(!amiga_host_exnext(c,lock,fib,260) && c->error==232);
    assert(amiga_host_examine(c,lock,fib,260) && amiga_host_exnext(c,lock,fib,260));
    assert(amiga_host_unlock(c,lock));
    h=amiga_host_open(c,"D/X",1005); assert(h);
    assert(amiga_host_seek(c,h,1,-1)==0 && amiga_host_write(c,h,"Z",1)==1);
    assert(amiga_host_file_close(c,h));
    h=amiga_host_open(c,"d/x",1005); assert(h);
    assert(amiga_host_read(c,h,bytes,8)==3 && !memcmp(bytes,"aZc",3));
    assert(amiga_host_seek(c,h,0,-1)==3 && amiga_host_write(c,h,"Q",1)==1);
    assert(amiga_host_file_close(c,h));
    size_t original_size; uint8_t *original=amiga_ofs_read(&disk,"D/X",&original_size);
    assert(original && original_size==3 && !memcmp(original,"abc",3)); free(original);
    lock=amiga_host_lock(c,""); assert(lock && amiga_host_examine(c,lock,fib,260));
    int overlay=0; while (amiga_host_exnext(c,lock,fib,260)) overlay|=!strcmp((char *)fib+8,"pilot");
    assert(overlay && amiga_host_unlock(c,lock));
    assert(amiga_host_queue_key(c,0x20,1) && (c->keyboard_matrix[4]&1));
    assert(amiga_host_queue_key(c,0x20,0) && !(c->keyboard_matrix[4]&1) && c->keys[0]==0x20 && c->keys[1]==0xA0);
    uint32_t structs=amiga_host_alloc(c,256,0x10004),plane=amiga_host_alloc(c,8000,0x10002);
    assert(structs && plane); uint32_t view=structs,vp=view+18,ri=vp+40,bm=ri+12,cm=bm+40;
    uint8_t *v=amiga_guest_range(&c->memory,structs,256),*p=v+18,*r=p+40,*bitmap=r+12,*colors=bitmap+40;
    amiga_store_be32(v,vp); word(v+12,16); word(v+14,129);
    amiga_store_be32(p+36,ri); amiga_store_be32(p+4,cm); word(p+24,320); word(p+26,200);
    amiga_store_be32(r+4,bm); word(bitmap,40); word(bitmap+2,200); bitmap[5]=1; amiga_store_be32(bitmap+8,plane);
    word(colors+2,2); amiga_store_be32(colors+4,cm+8); word(colors+8,0); word(colors+10,0xABC);
    size_t before=c->used_count;
    assert(amiga_host_make_viewport(c,view,vp) && amiga_host_merge_view(c,view));
    uint32_t cl=amiga_be32(p+8),cp=amiga_be32(v+4);
    uint8_t *cp_header=amiga_guest_range(&c->memory,cp,16);
    uint32_t hardware=amiga_be32(cp_header+4); uint8_t *hw=amiga_guest_range(&c->memory,hardware,160);
    assert(copper_value(hw,0x100)==0x1200 && copper_value(hw,0x08E)==0x2A81);
    assert(copper_value(hw,0xE2)==(uint16_t)plane && copper_value(hw,0x182)==0xABC);
    word(colors+10,0x123); assert(amiga_host_load_rgb4(c,vp,cm+8,2)); assert(copper_value(hw,0x182)==0x123);
    assert(amiga_host_free_copper(c,cl) && amiga_host_free_copper(c,cp) && c->used_count==before);
    assert(remove("host-compat-test-saves/pilot")==0);
    assert(remove("host-compat-test-saves/d/x")==0);
    amiga_host_close(c); free(c); free(chip); free(fast); amiga_ofs_close(&disk);
    puts("Host memory, libraries, ADF/overlay files, directory enumeration, keyboard and Copper contracts pass"); return 0;
}
