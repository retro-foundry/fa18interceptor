#ifndef AMIGA_PCM_OUTPUT_H
#define AMIGA_PCM_OUTPUT_H
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
enum { AMIGA_PCM_RING_FRAMES=131072 };
typedef struct {
    uint32_t device, bytes;
    unsigned rate;
    FILE *wave;
    char wave_buffer[4096];
    int16_t *ring;
    unsigned read_frame,queued_frames;
    unsigned startup_callbacks;
    const char *error;
} AmigaPcmOutput;
/* Stereo signed 16-bit PCM. Headless callers may capture without a device. */
int amiga_pcm_open(AmigaPcmOutput *output,unsigned rate,int audible,const char *wave,
                   char *error,size_t capacity);
int amiga_pcm_write(AmigaPcmOutput *output,const int16_t *samples,unsigned frames);
int amiga_pcm_close(AmigaPcmOutput *output);
/* Device callback consumer; caller owns the audio-device lock. Empty output
 * is silence, as with SDL's former queue; full writes fail without dropping PCM. */
void amiga_pcm_consume(AmigaPcmOutput *output,int16_t *samples,unsigned frames);
#endif
