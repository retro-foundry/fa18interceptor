/* F/A-18 Interceptor 320x200 SDL display and exact run075 oracle playback.
 * The stream is a visual oracle. Game behavior is ported separately as its
 * original routine contracts are proved (see PORT.md).
 */
#include <SDL.h>
#include "menu.h"
#include "replay.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

enum { WIDTH = 320, HEIGHT = 200, PIXELS = WIDTH * HEIGHT };

typedef struct {
    FILE *file;
    uint16_t chunky[PIXELS];
    uint32_t first;
    uint32_t last;
    uint32_t next;
} FrameStream;

typedef struct {
    FA18ReplayEvent events[128];
    size_t count;
    size_t next;
    FA18ReplayControlState controls;
} NativeReplay;

static int collect_replay_event(const FA18ReplayEvent *event, void *user) {
    NativeReplay *replay = user;
    if (!event || replay->count >= sizeof replay->events / sizeof replay->events[0]) {
        return -1;
    }
    replay->events[replay->count++] = *event;
    return 0;
}

static int native_replay_open(NativeReplay *replay, const char *path) {
    memset(replay, 0, sizeof *replay);
    if (fa18_replay_read_events(path, collect_replay_event, replay,
                                &replay->count) != 0 || replay->count == 0) {
        fprintf(stderr, "Cannot read native replay: %s\n", path);
        return 0;
    }
    return 1;
}

static int native_replay_apply_frame(NativeReplay *replay, uint32_t frame,
                                     FA18MenuState *menu_state) {
    while (replay->next < replay->count &&
           replay->events[replay->next].frame <= frame) {
        const FA18ReplayEvent *event = &replay->events[replay->next++];
        if (fa18_replay_apply_event(&replay->controls, event) != 0) return -1;
        if (event->frame == 230u && event->kind == FA18_REPLAY_KEY_EVENT &&
            event->value[0] == 49 && event->value[3] != 0) {
            if (fa18_select_run075_demo_mode(menu_state) != 0) return -1;
        }
        if (event->frame == 234u && event->kind == FA18_REPLAY_KEY_EVENT &&
            event->value[0] == 49 && event->value[3] == 0) {
            if (fa18_schedule_demo_selection(menu_state) != 0) return -1;
        }
    }
    return 0;
}

static int read_bytes(FILE *file, void *data, size_t size) {
    return fread(data, 1, size, file) == size;
}

static int read_u32(FILE *file, uint32_t *value) {
    uint8_t bytes[4];
    if (!read_bytes(file, bytes, sizeof bytes)) return 0;
    *value = (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) |
             ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
    return 1;
}

static uint32_t chunky_adler32(const uint16_t *pixels) {
    uint32_t a = 1, b = 0;
    for (size_t i = 0; i < PIXELS; ++i) {
        a += pixels[i] & 0xffu;
        b += a;
        a += pixels[i] >> 8;
        b += a;
        /* Adler modulo every 5552 bytes keeps uint32_t intermediates in range. */
        if (i % 2776 == 2775) {
            a %= 65521u;
            b %= 65521u;
        }
    }
    a %= 65521u;
    b %= 65521u;
    return (b << 16) | a;
}

static int stream_open(FrameStream *stream, const char *path) {
    uint8_t magic[8];
    uint32_t version, width, height;
    memset(stream, 0, sizeof *stream);
    stream->file = fopen(path, "rb");
    if (!stream->file) {
        fprintf(stderr, "Cannot open oracle bundle: %s\n", path);
        return 0;
    }
    if (!read_bytes(stream->file, magic, sizeof magic) ||
        memcmp(magic, "FA18RGB4", sizeof magic) != 0 ||
        !read_u32(stream->file, &version) ||
        !read_u32(stream->file, &width) ||
        !read_u32(stream->file, &height) ||
        !read_u32(stream->file, &stream->first) ||
        !read_u32(stream->file, &stream->last) ||
        version != 1 || width != WIDTH || height != HEIGHT ||
        stream->first > stream->last) {
        fprintf(stderr, "Invalid FA18RGB4 v1 header: %s\n", path);
        fclose(stream->file);
        stream->file = NULL;
        return 0;
    }
    stream->next = stream->first;
    return 1;
}

static int stream_next(FrameStream *stream) {
    uint32_t frame, expected, span_count;
    uint32_t previous_end = 0;
    if (stream->next > stream->last) return 0;
    if (!read_u32(stream->file, &frame) || !read_u32(stream->file, &expected) ||
        !read_u32(stream->file, &span_count) || frame != stream->next ||
        span_count > PIXELS) {
        fprintf(stderr, "Invalid frame record at %u\n", stream->next);
        return -1;
    }
    for (uint32_t n = 0; n < span_count; ++n) {
        uint8_t bounds[4];
        uint8_t raw[PIXELS * 2];
        if (!read_bytes(stream->file, bounds, sizeof bounds)) {
            fprintf(stderr, "Truncated span header at frame %u\n", frame);
            return -1;
        }
        uint32_t start = (uint32_t)bounds[0] | ((uint32_t)bounds[1] << 8);
        uint32_t length = (uint32_t)bounds[2] | ((uint32_t)bounds[3] << 8);
        if (!length || start < previous_end || start + length > PIXELS ||
            !read_bytes(stream->file, raw, length * 2)) {
            fprintf(stderr, "Invalid or truncated span at frame %u, span %u\n", frame, n);
            return -1;
        }
        for (uint32_t i = 0; i < length; ++i) {
            stream->chunky[start + i] = (uint16_t)(raw[i * 2] |
                                                 ((uint16_t)raw[i * 2 + 1] << 8));
        }
        previous_end = start + length;
    }
    uint32_t actual = chunky_adler32(stream->chunky);
    if (actual != expected) {
        fprintf(stderr, "Frame %u pixel checksum mismatch: expected %08x, got %08x\n",
                frame, expected, actual);
        return -1;
    }
    ++stream->next;
    return 1;
}

static void to_argb(const uint16_t *chunky, uint32_t *argb) {
    for (size_t i = 0; i < PIXELS; ++i) {
        uint16_t c = chunky[i];
        argb[i] = 0xff000000u | ((uint32_t)((c >> 8) & 15u) * 17u << 16) |
                  ((uint32_t)((c >> 4) & 15u) * 17u << 8) |
                  (uint32_t)(c & 15u) * 17u;
    }
}

static int dump_ppm(const char *path, const uint16_t *chunky) {
    FILE *out = fopen(path, "wb");
    if (!out) {
        fprintf(stderr, "Cannot write frame dump: %s\n", path);
        return 0;
    }
    if (fprintf(out, "P6\n%d %d\n255\n", WIDTH, HEIGHT) < 0) {
        fclose(out);
        return 0;
    }
    for (size_t i = 0; i < PIXELS; ++i) {
        uint16_t c = chunky[i];
        uint8_t rgb[3] = { (uint8_t)(((c >> 8) & 15u) * 17u),
                           (uint8_t)(((c >> 4) & 15u) * 17u),
                           (uint8_t)((c & 15u) * 17u) };
        if (fwrite(rgb, 1, 3, out) != 3) {
            fprintf(stderr, "Failed while writing frame dump: %s\n", path);
            fclose(out);
            return 0;
        }
    }
    if (fclose(out) != 0) {
        fprintf(stderr, "Failed to close frame dump: %s\n", path);
        return 0;
    }
    return 1;
}

static int stream_rgb444(const uint16_t *chunky) {
    uint8_t raw[PIXELS * 2];
    for (size_t i = 0; i < PIXELS; ++i) {
        raw[2 * i] = (uint8_t)chunky[i];
        raw[2 * i + 1] = (uint8_t)(chunky[i] >> 8);
    }
    return fwrite(raw, 1, sizeof raw, stdout) == sizeof raw;
}

static int apply_native_frame_gate(FrameStream *stream, uint32_t frame) {
    if (frame < 200u || frame > 284u) return 0;
    FA18IndexedFrameBuffer native_indexed;
    uint16_t native_rgb444[PIXELS];
    if (frame == 234u) fa18_render_run075_frame234_menu(&native_indexed, native_rgb444);
    else if (frame == 235u) fa18_render_run075_frame235_clear(&native_indexed, native_rgb444);
    else if (frame >= 236u && frame <= 272u) {
        fa18_render_run075_frame236_demo_label(&native_indexed, native_rgb444);
    }
    else if (frame >= 273u && frame <= 284u) {
        fa18_render_run075_frame235_clear(&native_indexed, native_rgb444);
    }
    else fa18_render_run075_frame200_menu(&native_indexed, native_rgb444);
    for (size_t i = 0; i < PIXELS; ++i) {
        if (native_rgb444[i] != stream->chunky[i]) {
        fprintf(stderr, "Native menu gate mismatch at frame %u, pixel %zu: "
                            "native %03x, oracle %03x\n",
                    frame, i, native_rgb444[i], stream->chunky[i]);
            return -1;
        }
        stream->chunky[i] = native_rgb444[i];
    }
    return 1;
}

static int playback(FrameStream *stream, const char *replay_path) {
    if (stream->first != 200u) {
        fprintf(stderr, "Native playback must start at run075 frame 200\n");
        return 1;
    }
    NativeReplay replay;
    if (!native_replay_open(&replay, replay_path)) return 1;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_Window *window = SDL_CreateWindow("F/A-18 Interceptor run075 oracle",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * 2, HEIGHT * 2,
        SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = window ? SDL_CreateRenderer(window, -1, 0) : NULL;
    SDL_Texture *texture = renderer ? SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, WIDTH, HEIGHT) : NULL;
    uint32_t *argb = malloc(PIXELS * sizeof *argb);
    if (!window || !renderer || !texture || !argb ||
        SDL_RenderSetLogicalSize(renderer, WIDTH, HEIGHT) != 0) {
        fprintf(stderr, "SDL display creation failed: %s\n", SDL_GetError());
        free(argb);
        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    int running = 1, paused = 0, step = 0, result = 0;
    FA18MenuState native_menu = {0};
    uint64_t deadline = SDL_GetTicks64();
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_KEYDOWN && !event.key.repeat) {
                if (event.key.keysym.sym == SDLK_ESCAPE) running = 0;
                if (event.key.keysym.sym == SDLK_SPACE) paused = !paused;
                if (event.key.keysym.sym == SDLK_RIGHT && paused) step = 1;
            }
        }
        if (!running) break;
        if (paused && !step) {
            SDL_Delay(5);
            deadline = SDL_GetTicks64();
            continue;
        }
        if (!paused && SDL_GetTicks64() < deadline) {
            SDL_Delay(1);
            continue;
        }
        step = 0;
        int status = stream_next(stream);
        if (status <= 0) {
            result = status < 0;
            break;
        }
        if (native_replay_apply_frame(&replay, stream->next - 1,
                                      &native_menu) != 0) {
            fprintf(stderr, "Native replay state update failed at frame %u\n",
                    stream->next - 1);
            result = 1;
            break;
        }
        int native_gate = apply_native_frame_gate(stream, stream->next - 1);
        if (native_gate < 0) {
            result = 1;
            break;
        }
        if (native_gate == 0) {
            fprintf(stderr, "Native frame gate is not implemented for frame %u\n",
                    stream->next - 1);
            result = 1;
            break;
        }
        to_argb(stream->chunky, argb);
        if (SDL_UpdateTexture(texture, NULL, argb, WIDTH * (int)sizeof *argb) != 0 ||
            SDL_RenderClear(renderer) != 0 ||
            SDL_RenderCopy(renderer, texture, NULL, NULL) != 0) {
            fprintf(stderr, "SDL frame %u failed: %s\n", stream->next - 1, SDL_GetError());
            result = 1;
            break;
        }
        SDL_RenderPresent(renderer);
        char title[96];
        snprintf(title, sizeof title, "F/A-18 Interceptor run075 | frame %u", stream->next - 1);
        SDL_SetWindowTitle(window, title);
        deadline += 20; /* PAL 50 Hz */
        if (SDL_GetTicks64() > deadline + 100) deadline = SDL_GetTicks64();
    }
    free(argb);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return result;
}

int main(int argc, char **argv) {
    const char *path = NULL, *dump_path = NULL, *replay_path = NULL;
    uint32_t dump_frame = 0;
    int verify = 0, raw_stream = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--frames") && i + 1 < argc) path = argv[++i];
        else if (!strcmp(argv[i], "--replay") && i + 1 < argc) replay_path = argv[++i];
        else if (!strcmp(argv[i], "--verify")) verify = 1;
        else if (!strcmp(argv[i], "--stream-rgb444")) raw_stream = 1;
        else if (!strcmp(argv[i], "--dump-frame") && i + 2 < argc) {
            char *end;
            unsigned long parsed = strtoul(argv[++i], &end, 10);
            if (*end || parsed > UINT32_MAX) {
                fprintf(stderr, "Invalid dump frame number\n");
                return 2;
            }
            dump_frame = (uint32_t)parsed;
            dump_path = argv[++i];
        } else {
            fprintf(stderr, "Usage: fa18_port --frames FILE [--replay FILE] [--verify | --stream-rgb444] [--dump-frame N OUTPUT.ppm]\n");
            return 2;
        }
    }
    if (!path || (raw_stream && (verify || dump_path))) {
        fprintf(stderr, "Specify --frames FILE\n");
        return 2;
    }
#ifdef _WIN32
    if (raw_stream && _setmode(_fileno(stdout), _O_BINARY) == -1) {
        fprintf(stderr, "Cannot set binary stdout mode\n");
        return 1;
    }
#endif
    FrameStream *stream = malloc(sizeof *stream);
    if (!stream) {
        fprintf(stderr, "Cannot allocate chunky framebuffer\n");
        return 1;
    }
    if (!stream_open(stream, path)) {
        free(stream);
        return 1;
    }
    int result = 0;
    if (dump_path && (dump_frame < stream->first || dump_frame > stream->last)) {
        fprintf(stderr, "Dump frame %u outside bundle %u..%u\n",
                dump_frame, stream->first, stream->last);
        result = 2;
    } else if (verify || dump_path || raw_stream) {
        uint32_t count = 0;
        while (stream->next <= stream->last) {
            int status = stream_next(stream);
            if (status < 0) { result = 1; break; }
            ++count;
            if (raw_stream && !stream_rgb444(stream->chunky)) {
                fprintf(stderr, "Failed streaming frame %u\n", stream->next - 1);
                result = 1;
                break;
            }
            if (dump_path && stream->next - 1 == dump_frame &&
                !dump_ppm(dump_path, stream->chunky)) { result = 1; break; }
            if (!verify && dump_path && stream->next - 1 == dump_frame) break;
        }
        if (!result && !raw_stream) printf("Validated %u frame(s), %u..%u\n", count,
                                          stream->first, stream->next - 1);
    } else {
        if (!replay_path) {
            fprintf(stderr, "Live native playback requires --replay FILE\n");
            result = 2;
        } else {
            result = playback(stream, replay_path);
        }
    }
    fclose(stream->file);
    free(stream);
    return result;
}
