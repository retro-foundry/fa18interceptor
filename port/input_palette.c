#include "input_palette.h"

int fa18_load_native_display_palette(FA18NativeInputPalette *s,
                                        const uint16_t *words,size_t count) {
    AmigaRgb4CopperList *list;
    uint8_t bytes[64];
    size_t i;
    if(!s || !s->display || (count && !words) || !s->color_map || count>32) return 0;
    list=s->display->saved_pair.display_list;
    for(i=0;i<count;++i) { bytes[2*i]=(uint8_t)(words[i]>>8); bytes[2*i+1]=(uint8_t)words[i]; }
    return amiga_rgb4_load(s->color_map,bytes,2*count,count,list);
}
int fa18_load_native_input_palette(void *context,FA18ViewportPalettePhase phase,
                                      const uint16_t *words) {
    (void)phase;
    return fa18_load_native_display_palette(context,words,16);
}
int fa18_bind_native_input_palette(FA18NativeInputPalette *s,FA18NativeInputDisplay *display,
                                     AmigaRgb4Palette *color_map) {
    if(!s || !display || !color_map) return 0;
    s->display=display; s->color_map=color_map;
    display->load_palette=fa18_load_native_input_palette;
    display->context=s;
    return 1;
}
