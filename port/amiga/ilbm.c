/* Native ILBM/ByteRun1 resource decoding. C0E078 loads pix/splsh/inst5/frnt5;
 * these are asset bytes, never a captured display or an emulated blit. */
#include "ilbm.h"
#include "hunk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int fail(char *error,size_t cap,const char *why) { if(cap) snprintf(error,cap,"ILBM: %s",why); return 0; }
void amiga_ilbm_free(AmigaIlbm *image) { free(image->indices); memset(image,0,sizeof *image); }
int amiga_ilbm_decode(AmigaIlbm *out,const uint8_t *b,size_t size,char *error,size_t cap) {
    const uint8_t *header=NULL,*body=NULL,*palette=NULL; size_t body_size=0,palette_size=0,end,pos;
    unsigned masking,compression,row_bytes,y,p,x; uint8_t *row;
    *out=(AmigaIlbm){0};
    if(size<12 || memcmp(b,"FORM",4) || memcmp(b+8,"ILBM",4)) return fail(error,cap,"expected FORM ILBM");
    end=(size_t)amiga_be32(b+4)+8;
    if(end>size || end<12) return fail(error,cap,"truncated FORM");
    for(pos=12;pos+8<=end;) {
        size_t length=amiga_be32(b+pos+4),payload=pos+8;
        if(length>end-payload) return fail(error,cap,"truncated chunk");
        if(!memcmp(b+pos,"BMHD",4)) { if(length!=20) return fail(error,cap,"invalid BMHD"); header=b+payload; }
        if(!memcmp(b+pos,"CMAP",4)) { palette=b+payload; palette_size=length; }
        if(!memcmp(b+pos,"BODY",4)) { body=b+payload; body_size=length; }
        pos=payload+length+(length&1);
    }
    if(!header || !body || !palette) return fail(error,cap,"missing BMHD, CMAP or BODY");
    memcpy(out->header,header,sizeof out->header);
    out->width=amiga_be16(header); out->height=amiga_be16(header+2); out->planes=header[8]; masking=header[9]; compression=header[10];
    if(!out->width || !out->height || out->width>320 || out->height>256 || !out->planes || out->planes>5 || masking==1 || masking>2 || compression>1 || palette_size!=3u*(1u<<out->planes))
        return fail(error,cap,"unsupported resource geometry/format");
    for(x=0;x<(1u<<out->planes);++x) out->palette[x]=(uint16_t)((palette[3*x]>>4)<<8|(palette[3*x+1]>>4)<<4|(palette[3*x+2]>>4));
    out->indices=calloc((size_t)out->width*out->height,1); row_bytes=((out->width+15)/16)*2; row=malloc(row_bytes);
    if(!out->indices || !row) { free(row); amiga_ilbm_free(out); return fail(error,cap,"allocation failed"); }
    pos=0;
    for(y=0;y<out->height;++y) for(p=0;p<out->planes;++p) {
        size_t used=0;
        while(used<row_bytes) {
            unsigned count; int control;
            if(pos>=body_size) goto bad;
            if(!compression) { count=row_bytes; if(count>body_size-pos) goto bad; memcpy(row,body+pos,count); pos+=count; used=count; break; }
            control=(int8_t)body[pos++]; if(control==-128) continue;
            count=(unsigned)(control>=0?control+1:1-control);
            if(count>row_bytes-used) goto bad;
            if(control>=0) { if(count>body_size-pos) goto bad; memcpy(row+used,body+pos,count); pos+=count; }
            else { if(pos>=body_size) goto bad; memset(row+used,body[pos++],count); }
            used+=count;
        }
        for(x=0;x<out->width;++x) if(row[x/8]&(0x80u>>(x&7))) out->indices[(size_t)y*out->width+x]|=(uint8_t)(1u<<p);
    }
    free(row); return 1;
bad:
    free(row); amiga_ilbm_free(out); return fail(error,cap,"invalid planar row data");
}
