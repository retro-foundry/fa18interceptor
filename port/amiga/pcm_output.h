#ifndef AMIGA_PCM_OUTPUT_H
#define AMIGA_PCM_OUTPUT_H
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
typedef struct {
    uint32_t device, bytes;
    unsigned rate;
    FILE *wave;
} AmigaPcmOutput;
/* Stereo signed 16-bit PCM. Headless callers may capture without a device. */
int amiga_pcm_open(AmigaPcmOutput *output,unsigned rate,int audible,const char *wave,
                   char *error,size_t capacity);
int amiga_pcm_write(AmigaPcmOutput *output,const int16_t *samples,unsigned frames);
int amiga_pcm_close(AmigaPcmOutput *output);
#endif
