#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include <SDL.h>
#include <stdint.h>
#include <string.h>
#include "sdl_memory.h"
#include "pcm_output.h"
#include "runtime_memory.h"

int main(void) {
    assert(amiga_sdl_memory_install());
    const size_t initial=amiga_sdl_memory_stats().used;
    unsigned char *first=SDL_malloc(257),*second=SDL_calloc(39,7);
    assert(first && second && (uintptr_t)first%16==0 && (uintptr_t)second%16==0);
    for(unsigned i=0;i<273;++i) assert(!second[i]);
    memset(first,0xa7,257);
    unsigned char *grown=SDL_realloc(first,4096);assert(grown);
    for(unsigned i=0;i<257;++i) assert(grown[i]==0xa7);
    assert(!SDL_realloc(grown,AMIGA_SDL_MEMORY_BYTES));
    for(unsigned i=0;i<257;++i) assert(grown[i]==0xa7);
    SDL_free(second);SDL_free(grown);
    assert(amiga_sdl_memory_stats().used==initial);
    /* Released adjacent blocks must be reusable as one large allocation. */
    void *large=SDL_malloc(AMIGA_SDL_MEMORY_BYTES-1024);assert(large);SDL_free(large);
    assert(!SDL_calloc(SIZE_MAX,2));

    char error[256];AmigaPcmOutput output={0};
    assert(amiga_pcm_open(&output,48000,1,NULL,error,sizeof error));
    SDL_PauseAudioDevice(output.device,1);
    int16_t *input=calloc(AMIGA_PCM_RING_FRAMES,4);
    int16_t *played=calloc(AMIGA_PCM_RING_FRAMES+8,4);assert(input && played);
    for(unsigned i=0;i<AMIGA_PCM_RING_FRAMES*2;++i) input[i]=(int16_t)(i*17);
    const size_t ready=amiga_sdl_memory_stats().requests;
    FILE *stream=tmpfile();assert(stream);
    amiga_runtime_memory_lock(1);
    assert(amiga_pcm_write(&output,input,AMIGA_PCM_RING_FRAMES-3));
    amiga_pcm_consume(&output,played,AMIGA_PCM_RING_FRAMES-5);
    assert(!memcmp(input,played,4*(AMIGA_PCM_RING_FRAMES-5)));
    assert(amiga_pcm_write(&output,input,AMIGA_PCM_RING_FRAMES-2));
    assert(output.queued_frames==AMIGA_PCM_RING_FRAMES);
    const unsigned position=output.read_frame;
    assert(!amiga_pcm_write(&output,input,1));
    assert(output.read_frame==position && output.queued_frames==AMIGA_PCM_RING_FRAMES);
    amiga_pcm_consume(&output,played,AMIGA_PCM_RING_FRAMES+8);
    assert(!memcmp(played,input+2*(AMIGA_PCM_RING_FRAMES-5),8));
    assert(!memcmp(played+4,input,4*(AMIGA_PCM_RING_FRAMES-2)));
    for(unsigned i=AMIGA_PCM_RING_FRAMES*2;i<(AMIGA_PCM_RING_FRAMES+8)*2;++i) assert(!played[i]);
    assert(!output.queued_frames && !amiga_runtime_memory_violations());
    /* None of these publications/consumptions requested even SDL pool space. */
    assert(amiga_sdl_memory_stats().requests==ready);
    assert(!malloc(8));assert(!calloc(2,4));assert(!realloc(input,8));free(input);
    assert(!fopen("unused-runtime-file","wb"));assert(fclose(stream)==EOF);
    assert(amiga_runtime_memory_violations()==6);
    amiga_runtime_memory_lock(0);
    assert(!fclose(stream));free(input);free(played);assert(amiga_pcm_close(&output));SDL_Quit();
    puts("Fixed SDL arena reuse/failure, PCM wrap/overflow/silence and heap guard pass");
    return 0;
}
