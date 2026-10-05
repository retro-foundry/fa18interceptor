#include "postflight_text.h"
#include "hex_field.h"
#include "audio_selection.h"
#include "menu_record.h"

int fa18_bind_native_postflight_text_assets(FA18NativePostflightText *s,
                                            const FA18Hunks *exe,
                                            const FA18DisplayPaletteAssets *palettes) {
    FA18MenuRecord record;
    const FA18HunkSegment *segment;
    size_t offset;
    if(!s || !palettes || !exe || exe->count<=FA18_MENU_TEXT_HUNK ||
       !exe->segments[FA18_MENU_TEXT_HUNK].data ||
       fa18_menu_select_message_record(exe,97,&record) ||
       record.text_length<23) return 0;
    segment=&exe->segments[FA18_MENU_TEXT_HUNK];
    offset=(size_t)(record.text-segment->data)-4;
    s->text_descriptor=segment->data+offset;
    s->text_bytes=segment->size-offset;
    s->palette_seed=palettes->mode_words;
    return 1;
}

int fa18_publish_native_postflight_text(FA18NativePostflightText *s,
                                        const FA18NativePostflightTextOps *ops) {
    unsigned i;
    uint32_t selected=0;
    uint16_t value;
    uint16_t *destination;
    if(!s || !ops || !ops->bootstrap_scene || !ops->bootstrap_scene(ops->context)) return 0;
    if(!s->display || !s->display->stable_palette || !s->palette_seed ||
       !s->flight || !s->flight->commands || !s->countdown || !s->callback ||
       !s->text_descriptor || s->text_bytes<27 || !s->checksums[0] ||
       !s->checksums[1] || !s->checksums[2]) return 0;
    /* The original captures this pointer AFTER the bootstrap call. Copy in
     * source order, preserving overlaps; memcpy/memmove changes that behavior. */
    destination=s->display->stable_palette;
    for(i=0;i<32;++i) destination[i]=s->palette_seed[i];
    s->flight->commands->indexed.mode=0;
    s->flight->pause=0xff;
    s->transition=0xff;
    *s->countdown=3;
    *s->callback=FA18_STAGE_C11446;
    value=*s->checksums[0];
    if(value!=0xc560u) {
        s->text_descriptor[21]=0x31;
        selected=value;
    }
    value=*s->checksums[1];
    if(value!=0x7e70u) {
        s->text_descriptor[21]|=0x32;
        selected=value;
    }
    value=*s->checksums[2];
    if(value!=0x4de8u) {
        s->text_descriptor[21]|=0x34;
        selected=value;
    }
    if(selected) {
        if(!fa18_format_native_hex_field(s->text_descriptor,s->text_bytes,22,selected,4)) return 0;
        *s->callback=FA18_STAGE_C113E4;
        return 1;
    }
    return fa18_start_native_menu_sound(s->audio,s->sounds,s->sound_count,50);
}
