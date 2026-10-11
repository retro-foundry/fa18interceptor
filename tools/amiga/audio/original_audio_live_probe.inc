/* Reference-only snapshots. These reads never advance audio or emulate time. */
#pragma pack(push, 1)
struct fa18_audio_live_record {
    uae_u64 cycle;
    uae_u32 pt, lc, per, evtime;
    uae_u16 len, wlen, dat, dat2;
    uae_u8 channel, state, volume, intreq2, flags, drhpos, phase;
};
#pragma pack(pop)
static struct fa18_audio_live_record fa18_audio_live[8];
static unsigned fa18_audio_restored_mask;
static void fa18_audio_live_copy(int nr, unsigned phase) {
    struct audio_channel_data *c = audio_channel + nr;
    struct fa18_audio_live_record *r = fa18_audio_live + 4 * phase + nr;
    r->cycle = get_cycles(); r->pt = c->pt; r->lc = c->lc;
    r->per = c->per; r->evtime = c->evtime;
    r->len = c->len; r->wlen = c->wlen; r->dat = c->dat; r->dat2 = c->dat2;
    r->channel = nr; r->state = c->state; r->volume = c->data.audvol;
    r->intreq2 = c->intreq2;
    r->flags = (c->dr ? 1 : 0) | (c->dsr ? 2 : 0) | (c->dat_written ? 4 : 0);
    r->drhpos = c->drhpos; r->phase = phase;
}
static void fa18_audio_live_restored(int nr) {
    fa18_audio_live_copy(nr, 0);
    fa18_audio_restored_mask |= 1u << nr;
}
__declspec(dllexport) unsigned e9k_debug_audio_live_record_size(void) {
    return sizeof(struct fa18_audio_live_record);
}
__declspec(dllexport) unsigned e9k_debug_audio_live_read(const struct fa18_audio_live_record **records) {
    if (fa18_audio_restored_mask != 15) return 0;
    for (int nr = 0; nr < 4; nr++) fa18_audio_live_copy(nr, 1);
    *records = fa18_audio_live;
    return 8;
}
