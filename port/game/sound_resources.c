/* Source C17510/C1756A. Loading/descriptor services are explicit boundaries;
 * repetition counts, links and availability bits remain original game logic. */
#include "sound_resources.h"
#include "globals.h"
#include "fixed_math.h"
static gaddr voice(unsigned slot) { return rd_u32(SOUND_VOICES+4*slot); }
void load_intro_sound_resources(const SoundResourceHooks *h) {
    if(!h->load(h->context,"text/text201",35) || !h->duplicate(h->context,35,36)) return;
    wr_u32(voice(35)+16,0xffffffffu);wr_u32(voice(36)+16,0xffffffffu);
    wr_u32(voice(35)+8,0xb30000);wr_u32(voice(36)+8,0xb50000);
    wr_u8(SOUND_FLAGS-1,rd_u8(SOUND_FLAGS-1)|4u);
}
void load_menu_sound_resources(const SoundResourceHooks *h) {
    h->load(h->context,"text/texti0a",13);h->duplicate(h->context,13,14);
    h->load(h->context,"text/texti0b",15);h->duplicate(h->context,15,16);
    h->load(h->context,"text/texti1",17);h->duplicate(h->context,17,18);
    h->load(h->context,"text/texti2",19);h->duplicate(h->context,19,20);
    h->duplicate(h->context,17,21);h->duplicate(h->context,17,22);
    h->duplicate(h->context,19,23);h->duplicate(h->context,19,24);
    h->load(h->context,"text/texti3",25);h->duplicate(h->context,25,26);
    h->load(h->context,"text/texti4",27);h->duplicate(h->context,27,28);
    h->duplicate(h->context,25,29);h->duplicate(h->context,25,30);
    h->duplicate(h->context,27,31);h->duplicate(h->context,27,32);
    h->load(h->context,"text/texti5",33);h->duplicate(h->context,33,34);
    for(unsigned slot=13;slot<35;++slot) if(!voice(slot)) {
        for(unsigned release=13;release<35;++release) h->release(h->context,release);
        return;
    }
    for(unsigned slot=13;slot<35;++slot) {
        wr_u32(voice(slot)+16,0);wr_u32(voice(slot)+20,0);
        wr_u32(voice(slot)+32,voice(slot+2));
    }
    wr_u32(voice(13)+16,13);wr_u32(voice(14)+16,13);
    wr_u32(voice(15)+16,1);wr_u32(voice(16)+16,1);
    static const unsigned repeated[]={17,18,21,22,25,26,29,30};
    for(unsigned i=0;i<sizeof repeated/sizeof *repeated;++i) {
        wr_u32(voice(repeated[i])+16,2);wr_u32(voice(repeated[i])+20,2);
    }
    wr_u32(voice(33)+16,3);wr_u32(voice(33)+20,3);
    wr_u32(voice(34)+16,3);wr_u32(voice(34)+20,3);
    wr_u32(voice(33)+32,voice(17));wr_u32(voice(34)+32,voice(18));
    wr_u8(SOUND_FLAGS,rd_u8(SOUND_FLAGS)|0x80u);
}
void initialize_square_wave_samples(gaddr samples,int32_t bytes) {
    for(int32_t i=0;i<bytes/2;++i) wr_u8(samples++,0x7f);
    for(int32_t i=0;i<bytes/2;++i) wr_u8(samples++,0x82);
}
void initialize_noise_samples(gaddr samples,int32_t bytes) {
    for(int32_t i=0;i<bytes;++i) wr_u8(samples++,(uint8_t)random_bits(8));
}
