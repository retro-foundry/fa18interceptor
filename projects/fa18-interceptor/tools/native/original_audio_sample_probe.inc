
#pragma pack(push, 1)
struct fa18_audio_sample_record {
    uae_u64 cycle;
    uae_u32 address;
    uae_u16 word;
    uae_s8 sample;
    uae_u8 channel_state;
    uae_u16 hpos, vpos;
};
#pragma pack(pop)
static struct fa18_audio_sample_record fa18_audio_samples[65536];
static unsigned fa18_audio_sample_count;
static bool fa18_audio_probe_enabled, fa18_audio_probe_overflow;
static uae_u32 fa18_audio_dat_addr[4], fa18_audio_dat2_addr[4];
__declspec(dllexport) unsigned e9k_debug_audio_probe_record_size(void) {
    return sizeof(struct fa18_audio_sample_record);
}
__declspec(dllexport) void e9k_debug_audio_probe_enable(int enabled) {
    fa18_audio_probe_enabled = enabled != 0;
    fa18_audio_sample_count = 0;
    fa18_audio_probe_overflow = false;
    for (int i=0;i<4;i++) fa18_audio_dat_addr[i]=fa18_audio_dat2_addr[i]=0xffffffff;
}
__declspec(dllexport) unsigned e9k_debug_audio_probe_take(const struct fa18_audio_sample_record **records) {
    *records = fa18_audio_samples;
    unsigned count = fa18_audio_sample_count;
    fa18_audio_sample_count = 0;
    return fa18_audio_probe_overflow ? 0xffffffff : count;
}
static void fa18_audio_probe_sample(int nr, sample8_t sample) {
    if (!fa18_audio_probe_enabled) return;
    if (fa18_audio_sample_count == 65536) {
        fa18_audio_probe_overflow = true;
        return;
    }
    struct audio_channel_data *c = audio_channel + nr;
    struct fa18_audio_sample_record *row=fa18_audio_samples + fa18_audio_sample_count++;
    row->cycle=get_cycles();
    row->address=fa18_audio_dat2_addr[nr];
    row->word=c->dat2;
    row->sample=sample;
    row->channel_state=nr | (c->state << 2);
    row->hpos=current_hpos();
    row->vpos=vpos;
}
