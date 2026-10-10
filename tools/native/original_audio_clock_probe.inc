/* Read-only reference output-clock state. No audio update, serialization,
 * event delivery or change to either sample countdown occurs here. */
extern unsigned int obtainedfreq;
extern float scaled_sample_evtime_orig, sound_sync_multiplier;
#pragma pack(push, 1)
struct fa18_audio_clock_record {
    uae_u64 service_cycle, last_cycle;
    float scaled_original, scaled_current, next_output, sync_multiplier;
    uae_u32 output_rate;
    uae_s32 nominal_lines, short_line_clocks, long_field;
    float fake_refresh, configured_refresh;
};
#pragma pack(pop)
static struct fa18_audio_clock_record fa18_audio_clock_snapshot;
__declspec(dllexport) unsigned e9k_debug_audio_clock_record_size(void) {
    return sizeof(struct fa18_audio_clock_record);
}
__declspec(dllexport) unsigned e9k_debug_audio_clock_read(const struct fa18_audio_clock_record **record) {
    struct fa18_audio_clock_record *r=&fa18_audio_clock_snapshot;
    r->service_cycle=get_cycles();r->last_cycle=last_cycles;
    r->scaled_original=scaled_sample_evtime_orig;r->scaled_current=scaled_sample_evtime;
    r->next_output=next_sample_evtime;r->sync_multiplier=sound_sync_multiplier;
    r->output_rate=obtainedfreq;r->nominal_lines=maxvpos_nom;
    r->short_line_clocks=maxhpos_short;r->long_field=lof_display;
    r->fake_refresh=fake_vblank_hz;r->configured_refresh=currprefs.chipset_refreshrate;
    *record=r;return 1;
}
