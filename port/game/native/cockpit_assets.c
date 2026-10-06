/* C0E57E/C0E59E load the immutable instrument/frame ILBMs; C16982 publishes
 * their planes and builds the panel mask. All pixels come from the ADF. */
#include "cockpit_assets.h"
#include "storage.h"
#include "../../amiga/ilbm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { ASSET_FIRST=0x64000,ASSET_END=0x68000,
       INSTRUMENT_OBJECT=0xc1ab08,INSTRUMENT_HEADER=0xc1ab0c,INSTRUMENT_PLANES=0xc1ab20,
       FRAME_OBJECT=0xc1ab38,FRAME_HEADER=0xc1ab3c,FRAME_PLANES=0xc1ab50,
       PANEL_MASK=0xc1ab34,PANEL_WORKSPACE=0xc1ab60,PANEL_SUBIMAGES=0xc1ab64 };
static gaddr reserve(gaddr *cursor,size_t bytes) {
    gaddr result=*cursor;
    if(bytes>ASSET_END-result) abort();
    *cursor+=(gaddr)((bytes+1)&~(size_t)1);
    memset(native_storage_range(result,bytes),0,bytes);
    return result;
}
static int load_bitmap(const AmigaOfs *disk,const char *path,gaddr slot,gaddr header,
                       gaddr *cursor,char *error,size_t capacity) {
    size_t size=0;uint8_t *bytes=amiga_ofs_read(disk,path,&size);AmigaIlbm image={0};
    if(!bytes) { if(capacity) snprintf(error,capacity,"missing cockpit asset %s",path);return 0; }
    int ok=amiga_ilbm_decode(&image,bytes,size,error,capacity);free(bytes);
    if(!ok) return 0;
    unsigned expected_width=slot==INSTRUMENT_OBJECT?320:288;
    unsigned expected_height=slot==INSTRUMENT_OBJECT?55:12;
    if(image.width!=expected_width || image.height!=expected_height || image.planes!=5) {
        if(capacity) snprintf(error,capacity,"unexpected cockpit geometry in %s",path);
        amiga_ilbm_free(&image);return 0;
    }
    gaddr object=reserve(cursor,40);
    unsigned row_bytes=((image.width+15)/16)*2,plane_bytes=row_bytes*image.height;
    wr_u32(slot,object);wr_u16(object,(uint16_t)row_bytes);
    wr_u16(object+2,(uint16_t)image.height);wr_u8(object+5,(uint8_t)image.planes);
    memcpy(native_storage_range(header,20),image.header,20);
    for(unsigned p=0;p<image.planes;++p) {
        gaddr plane=reserve(cursor,plane_bytes);wr_u32(object+8+4*p,plane);
        uint8_t *pixels=native_storage_range(plane,plane_bytes);
        for(unsigned y=0;y<image.height;++y) for(unsigned x=0;x<image.width;++x)
            if(image.indices[y*image.width+x]&(1u<<p)) pixels[y*row_bytes+x/8]|=(uint8_t)(0x80u>>(x&7));
    }
    for(unsigned i=0;i<32;++i) wr_u16(0xc1aa9c+2*i,image.palette[i]);
    amiga_ilbm_free(&image);return 1;
}
void native_cockpit_prepare(gaddr workspace,gaddr mask) {
    /* C16982..C16ACC: source cache offsets and four-plane mask composition.
     * C168E0 releases the unused fifth planes in the original allocator;
     * host asset storage retains them outside all writable render buffers. */
    for(unsigned p=0;p<4;++p) {
        gaddr plane=rd_u32(rd_u32(INSTRUMENT_OBJECT)+8+4*p);
        wr_u32(INSTRUMENT_PLANES+4*p,plane);wr_u32(PANEL_SUBIMAGES+4*p,plane+0x2d0);
        wr_u32(FRAME_PLANES+4*p,rd_u32(rd_u32(FRAME_OBJECT)+8+4*p));
    }
    wr_u32(PANEL_WORKSPACE,workspace);wr_u32(PANEL_MASK,mask);
    unsigned bytes=(unsigned)rd_u16(FRAME_HEADER)*rd_u16(FRAME_HEADER+2)>>3;
    for(unsigned i=0;i<bytes;++i) {
        uint8_t bits=0;
        for(unsigned p=0;p<4;++p) bits|=rd_u8(rd_u32(FRAME_PLANES+4*p)+i);
        wr_u8(mask+i,bits); /* C16B2A copies plane zero; C16B58 ORs the rest. */
    }
}
int native_cockpit_load(const AmigaOfs *disk,char *error,size_t capacity) {
    gaddr cursor=ASSET_FIRST;
    if(!load_bitmap(disk,"pix/inst5",INSTRUMENT_OBJECT,INSTRUMENT_HEADER,&cursor,error,capacity) ||
       !load_bitmap(disk,"pix/frnt5",FRAME_OBJECT,FRAME_HEADER,&cursor,error,capacity)) return 0;
    gaddr workspace=reserve(&cursor,32);
    unsigned bytes=(unsigned)rd_u16(FRAME_HEADER)*rd_u16(FRAME_HEADER+2)>>3;
    gaddr mask=reserve(&cursor,bytes);
    native_cockpit_prepare(workspace,mask);return 1;
}
