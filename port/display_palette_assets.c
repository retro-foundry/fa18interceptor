#include "display_palette_assets.h"
#include <string.h>

int fa18_import_display_palette_assets(FA18DisplayPaletteAssets *s,const FA18Hunks *exe,
                                          const uint8_t *file,size_t bytes) {
    FA18DisplayPaletteAssets loaded;
    const uint8_t *colors=NULL;
    const FA18HunkSegment *segment;
    size_t at=12,i,mode;
    if(!s || !exe || exe->count<=21 || !file || bytes<12 ||
       memcmp(file,"FORM",4) || memcmp(file+8,"ILBM",4) || fa18_be32(file+4)!=bytes-8) return 0;
    segment=&exe->segments[21];
    if(!segment->data || segment->size<0x80+16*32) return 0;
    while(at<bytes) {
        size_t count;
        if(bytes-at<8) return 0;
        count=fa18_be32(file+at+4);
        if(count>bytes-at-8) return 0;
        if(!memcmp(file+at,"CMAP",4)) {
            if(colors || count!=96) return 0;
            colors=file+at+8;
        }
        at+=8+count;
        if(count&1) { if(at==bytes) return 0; ++at; }
    }
    if(!colors) return 0;
    for(i=0;i<32;++i) {
        loaded.initial[i]=(uint16_t)((unsigned)(colors[3*i]>>4)<<8 |
             (unsigned)(colors[3*i+1]>>4)<<4 | (unsigned)(colors[3*i+2]>>4));
        loaded.static_words[i]=fa18_be16(segment->data+0x40+2*i);
    }
    for(mode=0;mode<16;++mode) for(i=0;i<16;++i)
        loaded.mode_words[mode][i]=fa18_be16(segment->data+0x80+(15-mode)*32+2*i);
    /* Pointer members are bound AFTER copying the owned values. */
    memset(loaded.modes,0,sizeof loaded.modes); *s=loaded;
    for(mode=0;mode<16;++mode) s->modes[mode]=s->mode_words[mode];
    return 1;
}
