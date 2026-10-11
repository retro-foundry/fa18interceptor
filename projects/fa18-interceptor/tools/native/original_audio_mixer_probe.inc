/* Reference-only logical update_audio boundaries and actual Paula integrals.
 * No clock, audio channel or accumulator is changed by this observer. */
#pragma pack(push, 1)
struct fa18_audio_mixer_record {
    uae_u64 service_cycle, logical_cycle;
    uae_u32 duration;
    uae_s32 values[4];
    uae_u32 times[4];
    uae_u8 kind;
};
#pragma pack(pop)
static struct fa18_audio_mixer_record fa18_audio_mixer_records[65536];
static unsigned fa18_audio_mixer_count, fa18_audio_mixer_depth;
static bool fa18_audio_mixer_enabled, fa18_audio_mixer_overflow;
static uae_u64 fa18_audio_mixer_logical;
static void fa18_audio_mixer_observe(unsigned kind,unsigned duration,int nr,int sample);
__declspec(dllexport) unsigned e9k_debug_audio_mixer_record_size(void) {
    return sizeof(struct fa18_audio_mixer_record);
}
__declspec(dllexport) void e9k_debug_audio_mixer_enable(int enabled) {
    fa18_audio_mixer_enabled=enabled!=0;
    fa18_audio_mixer_count=0;fa18_audio_mixer_overflow=false;
    if(enabled) fa18_audio_mixer_observe(0,0,0,0);
}
__declspec(dllexport) unsigned e9k_debug_audio_mixer_take(const struct fa18_audio_mixer_record **records) {
    *records=fa18_audio_mixer_records;
    unsigned count=fa18_audio_mixer_count;fa18_audio_mixer_count=0;
    return fa18_audio_mixer_overflow?0xffffffff:count;
}
static void fa18_audio_mixer_observe(unsigned kind,unsigned duration,int nr,int sample) {
    if(!fa18_audio_mixer_enabled) return;
    if(fa18_audio_mixer_count==65536) {fa18_audio_mixer_overflow=true;return;}
    struct fa18_audio_mixer_record *r=fa18_audio_mixer_records+fa18_audio_mixer_count++;
    r->service_cycle=get_cycles();
    r->logical_cycle=kind==0?last_cycles:fa18_audio_mixer_depth?fa18_audio_mixer_logical:r->service_cycle;
    r->kind=kind;r->duration=duration;
    for(unsigned i=0;i<4;++i) {
        struct audio_channel_data2 *c=audio_data[i];
        r->times[i]=c->sample_accum_time;
        r->values[i]=kind==0?c->sample_accum:kind==1?((c->current_sample*c->mixvol)&c->adk_mask):
            kind==2?0:
            (i==(unsigned)nr?sample:0);
    }
}
static void fa18_audio_mixer_average(unsigned index,const int *datas) {
    if(!fa18_audio_mixer_enabled || index>=fa18_audio_mixer_count) return;
    for(unsigned i=0;i<4;++i) fa18_audio_mixer_records[index].values[i]=datas[i];
}
