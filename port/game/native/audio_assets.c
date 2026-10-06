/* C503E8/C5058E load whole sample files; C5046C initializes a 64-byte voice,
 * and C50614 duplicates its named fields while sharing sample storage.
 * The host owns these buffers; no Exec allocator or audio.device is modeled. */
#include "audio_assets.h"
#include "storage.h"
#include "../sound_resources.h"
#include "../input_display_setup.h"
#include "../globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    const AmigaOfs *disk;
    gaddr cursor[3];
    char *error;size_t capacity;int failed;
} AudioAssets;
static gaddr allocate(AudioAssets *assets,size_t bytes) {
    static const gaddr ends[]={0x12000,0xc7e000,0x80000};
    bytes=(bytes+3)&~(size_t)3;
    for(unsigned bank=0;bank<3;++bank) if(bytes<=ends[bank]-assets->cursor[bank]) {
        gaddr result=assets->cursor[bank];assets->cursor[bank]+=(gaddr)bytes;
        memset(native_storage_range(result,bytes),0,bytes);return result;
    }
    assets->failed=1;
    if(assets->capacity) snprintf(assets->error,assets->capacity,"native sound asset storage exhausted (%zu bytes)",bytes);
    return 0;
}
static gaddr sample_voice(unsigned slot) { return rd_u32(SOUND_VOICES+4*slot); }
static int create_voice(AudioAssets *assets,size_t bytes,unsigned slot) {
    gaddr record=allocate(assets,64),samples=allocate(assets,bytes);
    if(!record || !samples) return 0;
    wr_u32(SOUND_VOICES+4*slot,record);
    wr_u32(record,samples);wr_u32(record+4,(uint32_t)bytes);
    wr_u32(record+8,0x1660000);wr_u32(record+12,0x3f0000);wr_u32(record+16,1);
    return 1;
}
static int load(void *context,const char *path,unsigned slot) {
    AudioAssets *assets=context;size_t bytes=0;uint8_t *data=amiga_ofs_read(assets->disk,path,&bytes);
    if(!data || !bytes) {
        free(data);
        /* C5058E returns zero here. C1787A may try textegn2, and each
         * original resource owner decides which availability bit to set. */
        fprintf(stderr,"native sound resource unavailable: %s\n",path);
        return 0;
    }
    if(!create_voice(assets,bytes,slot)) {free(data);return 0;}
    memcpy(native_storage_range(rd_u32(sample_voice(slot)),bytes),data,bytes);free(data);
    return 1;
}
static int duplicate(void *context,unsigned source,unsigned destination) {
    AudioAssets *assets=context;gaddr original=sample_voice(source);
    if(!original) return 0;
    gaddr record=allocate(assets,64);if(!record) return 0;
    wr_u32(SOUND_VOICES+4*destination,record);
    static const unsigned fields[]={0,8,12,16,24,28,32,44,56,60};
    for(unsigned i=0;i<sizeof fields/sizeof *fields;++i)
        wr_u32(record+fields[i],rd_u32(original+fields[i]));
    wr_u32(record+4,rd_u32(original+4)|0x80000000u);return 1;
}
static void release(void *context,unsigned slot) {
    /* Source C507D6 releases its allocation and clears the slot. Host storage
     * belongs to this frontend lifetime; failed startup discards it together. */
    (void)context;wr_u32(SOUND_VOICES+4*slot,0);
}
static int32_t setup_sound_child(void *context,enum InputDisplayChild child) {
    AudioAssets *assets=context;gaddr record;
    switch(child) {
    case IDS_LOAD_TEXT_2: return load(assets,"text/textegn",2);
    case IDS_LOAD_TEXT_2_ALTERNATE: return load(assets,"text/textegn2",2);
    case IDS_DUPLICATE_TEXT_2: return duplicate(assets,2,3);
    case IDS_ALLOCATE_TEXT_4: return create_voice(assets,32,4);
    case IDS_CLEAR_TEXT_4:
        record=sample_voice(4);initialize_square_wave_samples(rd_u32(record),rd_s32(record+4));return 0;
    case IDS_ALLOCATE_TEXT_6: return create_voice(assets,0x800,6);
    case IDS_CLEAR_TEXT_6:
        record=sample_voice(6);initialize_noise_samples(rd_u32(record),rd_s32(record+4));return 0;
    case IDS_LOAD_TEXT_5: return load(assets,"text/texttre",5);
    case IDS_DUPLICATE_TEXT_6: return duplicate(assets,6,10);
    case IDS_LOAD_TEXT_11: return load(assets,"text/textger",11);
    case IDS_DUPLICATE_TEXT_2_TO_12: return duplicate(assets,2,12);
    case IDS_LOAD_TEXT_0: return load(assets,"text/textcpt",0);
    case IDS_DUPLICATE_TEXT_0: return duplicate(assets,0,1);
    case IDS_LOAD_TEXT_8: return load(assets,"text/textwnd",8);
    case IDS_DUPLICATE_TEXT_8: return duplicate(assets,8,9);
    default: fprintf(stderr,"unexpected native sound setup child %u\n",(unsigned)child);abort();
    }
}
int native_audio_load_resources(const AmigaOfs *disk,char *error,size_t capacity) {
    AudioAssets assets={disk,{0x8000,0xc55000,0x68000},error,capacity,0};
    const SoundResourceHooks hooks={load,duplicate,release,&assets};
    load_intro_sound_resources(&hooks);load_menu_sound_resources(&hooks);
    const InputDisplayHooks setup={setup_sound_child,NULL,&assets};
    load_setup_text_resources(0x4a20,&setup); /* C1787A's two-byte local. */
    return !assets.failed;
}
