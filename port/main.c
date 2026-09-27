#include <SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

#include "game.h"

typedef struct {
    FA18ReplayEvent *items;
    size_t count;
    size_t capacity;
} EventList;

static int append_event(const FA18ReplayEvent *event, void *user) {
    EventList *list = user;
    if (list->count == list->capacity) {
        size_t capacity = list->capacity ? list->capacity * 2 : 64;
        FA18ReplayEvent *items = realloc(list->items, capacity * sizeof *items);
        if (!items) return -1;
        list->items = items;
        list->capacity = capacity;
    }
    list->items[list->count++] = *event;
    return 0;
}

static int write_rgb444(const FA18Video *video) {
    uint16_t pixels[FA18_PIXELS];
    fa18_video_to_rgb444(video, pixels);
    return fwrite(pixels, sizeof *pixels, FA18_PIXELS, stdout) == FA18_PIXELS ? 0 : -1;
}

static void copy_to_argb(const FA18Video *video, uint32_t *argb) {
    for (size_t i = 0; i < FA18_PIXELS; ++i) {
        uint16_t rgb = video->palette[video->pixels[i] & 31u] & 0x0fffu;
        uint32_t red = (rgb >> 8) & 15u;
        uint32_t green = (rgb >> 4) & 15u;
        uint32_t blue = rgb & 15u;
        argb[i] = 0xff000000u | (red * 17u << 16) | (green * 17u << 8) | blue * 17u;
    }
}

static int play_window(FA18Game *game, const EventList *events) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "SDL initialization failed: %s\n", SDL_GetError());
        return -1;
    }
    SDL_Window *window = SDL_CreateWindow("F/A-18 Interceptor",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        FA18_WIDTH * 2, FA18_HEIGHT * 2, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = window ? SDL_CreateRenderer(window, -1, 0) : NULL;
    SDL_Texture *texture = renderer ? SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, FA18_WIDTH, FA18_HEIGHT) : NULL;
    uint32_t *argb = texture ? malloc(FA18_PIXELS * sizeof *argb) : NULL;
    if (!window || !renderer || !texture || !argb) {
        fprintf(stderr, "SDL display creation failed: %s\n", SDL_GetError());
        free(argb);
        if (texture) SDL_DestroyTexture(texture);
        if (renderer) SDL_DestroyRenderer(renderer);
        if (window) SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    FA18ReplayControlState controls = {0};
    size_t next_event = 0;
    int running = 1;
    uint64_t deadline = SDL_GetTicks64();
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) if (event.type == SDL_QUIT) running = 0;
        if (!running) break;
        if (SDL_GetTicks64() < deadline) {
            SDL_Delay(1);
            continue;
        }
        if (fa18_replay_advance_frame(&controls, events->items, events->count,
                                      &next_event, game->frame) != 0) {
            fputs("Replay control update failed\n", stderr);
            running = 0;
            continue;
        }
        if (fa18_game_apply_controls(game, &controls) != 0) {
            fputs("Game control update failed\n", stderr);
            running = 0;
            continue;
        }
        copy_to_argb(&game->video, argb);
        if (SDL_UpdateTexture(texture, NULL, argb, FA18_WIDTH * (int)sizeof *argb) != 0 ||
            SDL_RenderClear(renderer) != 0 || SDL_RenderCopy(renderer, texture, NULL, NULL) != 0) {
            fprintf(stderr, "SDL display update failed: %s\n", SDL_GetError());
            running = 0;
            continue;
        }
        SDL_RenderPresent(renderer);
        if (fa18_game_frame(game, &controls) != 0) {
            fputs("Game frame update failed\n", stderr);
            running = 0;
            continue;
        }
        deadline += 20;
        if (SDL_GetTicks64() > deadline + 100) deadline = SDL_GetTicks64();
    }
    free(argb);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int main(int argc, char **argv) {
    const char *adf_path = NULL;
    const char *replay_path = NULL;
    uint32_t last = 0;
    int headless = 0;
    int dump_stdout = 0;
    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], "--adf") && i + 1 < argc) adf_path = argv[++i];
        else if (!strcmp(argv[i], "--replay") && i + 1 < argc) replay_path = argv[++i];
        else if (!strcmp(argv[i], "--headless")) headless = 1;
        else if (!strcmp(argv[i], "--to") && i + 1 < argc) {
            char *end = NULL;
            unsigned long value = strtoul(argv[++i], &end, 10);
            if (!end || *end || value > UINT32_MAX) {
                fputs("Invalid final frame\n", stderr);
                return 2;
            }
            last = (uint32_t)value;
        } else if (!strcmp(argv[i], "--dump-rgb444") && i + 1 < argc) {
            dump_stdout = !strcmp(argv[++i], "-");
            if (!dump_stdout) {
                fputs("Only standard output is supported for RGB444 output\n", stderr);
                return 2;
            }
        } else {
            fputs("Usage: fa18_port --adf FILE --replay FILE [--headless --to N --dump-rgb444 -]\n", stderr);
            return 2;
        }
    }
    if (!adf_path || !replay_path || (headless && (!last || !dump_stdout)) ||
        (!headless && (last || dump_stdout))) {
        fputs("Missing or incompatible playback options\n", stderr);
        return 2;
    }
#ifdef _WIN32
    if (dump_stdout && _setmode(_fileno(stdout), _O_BINARY) == -1) {
        fputs("Cannot set binary standard output\n", stderr);
        return 1;
    }
#endif

    EventList events = {0};
    if (fa18_replay_read_events(replay_path, append_event, &events, NULL) != 0) {
        fprintf(stderr, "Cannot read replay input: %s\n", replay_path);
        free(events.items);
        return 1;
    }
    FA18Game game;
    if (!fa18_game_init(&game, adf_path)) {
        free(events.items);
        return 1;
    }
    int result = 0;
    if (headless) {
        FA18ReplayControlState controls = {0};
        size_t next_event = 0;
        if (last < game.frame) {
            fputs("Final frame precedes native start\n", stderr);
            result = 2;
        }
        while (!result && game.frame <= last) {
            if (fa18_replay_advance_frame(&controls, events.items, events.count,
                                          &next_event, game.frame) != 0 ||
                fa18_game_apply_controls(&game, &controls) != 0 ||
                write_rgb444(&game.video) != 0) {
                fputs("Native frame output failed\n", stderr);
                result = 1;
                break;
            }
            if (fa18_game_frame(&game, &controls) != 0) {
                fputs("Game frame update failed\n", stderr);
                result = 1;
                break;
            }
        }
    } else {
        result = play_window(&game, &events) != 0;
    }
    fa18_game_free(&game);
    free(events.items);
    return result;
}
