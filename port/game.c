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
    for (size_t i = 0; i + 1 < game->menu_text.selector_count; ++i) {
        if (fa18_menu_select_message_record(&game->exe, game->menu_text.selectors[i],
                                            &game->menu_records[i]) != 0) {
            fputs("Cannot resolve a top-level menu text record\n", stderr);
            fa18_hunks_free(&game->exe);
            fa18_disk_close(&game->disk);
            return 0;
        }
    }
    if (fa18_menu_select_inline_followup(
            &game->exe, &game->menu_records[game->menu_text.selector_count - 2],
            game->menu_text.text_layout_increment,
            &game->menu_records[game->menu_text.selector_count - 1]) != 0) {
        fputs("Cannot resolve the top-level menu inline text record\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (fa18_render_top_level_menu(&game->video, &game->exe, game->menu_records,
                                   game->menu_text.selector_count) != 0) {
        fputs("Cannot render the top-level menu text\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    fa18_menu_flow_init(&game->menu_flow);
    game->frame = START_FRAME;
    return 1;
}

void fa18_game_free(FA18Game *game) {
    fa18_hunks_free(&game->exe);
    fa18_disk_close(&game->disk);
}

int fa18_game_apply_controls(FA18Game *game, const FA18ReplayControlState *controls) {
    if (!game) return -1;
    return fa18_menu_flow_apply_controls(&game->menu_flow, controls, &game->video);
}

int fa18_game_frame(FA18Game *game, const FA18ReplayControlState *controls) {
    if (!game) return -1;
    (void)controls;
    uint16_t selector;
    if (fa18_menu_flow_take_selector(&game->menu_flow, &selector) == 0) {
        FA18MenuRecord record;
        if (fa18_menu_select_message_record(&game->exe, selector, &record) != 0 ||
            fa18_render_top_level_menu(&game->video, &game->exe, &record, 1) != 0)
            return -1;
    }
    fa18_menu_flow_finish_presented_frame(&game->menu_flow, &game->video);
    ++game->frame;
    return 0;
}
