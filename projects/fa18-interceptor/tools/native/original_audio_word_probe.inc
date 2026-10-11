/* Reference-only word lifecycle telemetry. Never used by native playback. */
#pragma pack(push, 1)
struct fa18_audio_word_record {
    uae_u64 cycle;
    uae_u32 pt, lc, address;
    uae_u16 len, wlen, dat, dat2, hpos, vpos;
    uae_u8 channel, state, kind, flags;
    uae_u16 value;
};
#pragma pack(pop)
static struct fa18_audio_word_record fa18_audio_words[65536];
static unsigned fa18_audio_word_count;
static bool fa18_audio_word_enabled, fa18_audio_word_overflow;
__declspec(dllexport) unsigned e9k_debug_audio_word_record_size(void) {
    return sizeof(struct fa18_audio_word_record);
}
__declspec(dllexport) void e9k_debug_audio_word_enable(int enabled) {
    fa18_audio_word_enabled = enabled != 0;
    fa18_audio_word_count = 0;
    fa18_audio_word_overflow = false;
}
__declspec(dllexport) unsigned e9k_debug_audio_word_take(const struct fa18_audio_word_record **records) {
    *records = fa18_audio_words;
    unsigned count = fa18_audio_word_count;
    fa18_audio_word_count = 0;
    return fa18_audio_word_overflow ? 0xffffffff : count;
}
static void fa18_audio_word_observe(int nr, int kind, uaecptr address, uae_u16 value) {
    if (!fa18_audio_word_enabled) return;
    if (fa18_audio_word_count == 65536) { fa18_audio_word_overflow = true; return; }
    struct audio_channel_data *c = audio_channel + nr;
    struct fa18_audio_word_record *r = fa18_audio_words + fa18_audio_word_count++;
    r->cycle = get_cycles(); r->pt = c->pt; r->lc = c->lc; r->address = address;
    r->len = c->len; r->wlen = c->wlen; r->dat = c->dat; r->dat2 = c->dat2;
    r->hpos = current_hpos(); r->vpos = vpos;
    r->channel = nr; r->state = c->state; r->kind = kind;
    r->flags = (c->dr ? 1 : 0) | (c->dsr ? 2 : 0) | (c->dat_written ? 4 : 0) | (c->intreq2 ? 8 : 0);
    r->value = value;
}
