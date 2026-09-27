#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { START_FRAME = 200 };

int fa18_game_init(FA18Game *game, const char *adf_path) {
    memset(game, 0, sizeof *game);
    if (!fa18_disk_open(&game->disk, adf_path)) {
        fprintf(stderr, "Cannot read AmigaDOS disk image: %s\n", adf_path);
        return 0;
    }
    size_t size = 0;
    uint8_t *file = fa18_disk_read(&game->disk, "F-18 Interceptor", &size);
    int loaded = file && fa18_hunks_load(&game->exe, file, size);
    free(file);
    if (!loaded) {
        fprintf(stderr, "Cannot load the game executable from %s\n", adf_path);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (fa18_menu_queue_top_level_text(&game->menu_text) != 0) {
        fputs("Cannot initialize the top-level menu text queue\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    game->frame = START_FRAME;
    return 1;
}

void fa18_game_free(FA18Game *game) {
    fa18_hunks_free(&game->exe);
    fa18_disk_close(&game->disk);
}

void fa18_game_frame(FA18Game *game, const FA18ReplayControlState *controls) {
    (void)controls;
    ++game->frame;
}
