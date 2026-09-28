#include "game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum { START_FRAME = 200 };

static int initialize_viewport_callback_state(FA18Game *game) {
    if (!game) return -1;
    for (unsigned colour = 0; colour < FA18_VIEWPORT_PALETTE_WORDS; ++colour) {
        const uint16_t register_word = (uint16_t)(UINT16_C(0x0180) + colour * 2u);
        game->viewport_copper_bytes[colour * 4u] = (uint8_t)(register_word >> 8);
        game->viewport_copper_bytes[colour * 4u + 1u] = (uint8_t)register_word;
    }
    game->viewport_copper_bytes[FA18_VIEWPORT_PALETTE_WORDS * 4u] = 0xff;
    game->viewport_copper_bytes[FA18_VIEWPORT_PALETTE_WORDS * 4u + 1u] = 0xff;
    game->viewport_copper_bytes[FA18_VIEWPORT_PALETTE_WORDS * 4u + 2u] = 0xff;
    game->viewport_copper_bytes[FA18_VIEWPORT_PALETTE_WORDS * 4u + 3u] = 0xfe;
    game->viewport_copper_stream = (FA18CopperMutableInstructionStream){
        game->viewport_copper_bytes, sizeof game->viewport_copper_bytes
    };
    return fa18_initialize_viewport_palette_buffer(&game->exe,
                                                   &game->viewport_palette_buffer);
}

static int advance_viewport_callback(FA18Game *game) {
    FA18ViewportModeBindings bindings;
    FA18ViewportModeStep step;

    if (!game) return -1;
    bindings = (FA18ViewportModeBindings){
        0, game->viewport_left_pointer_table, game->viewport_right_pointer_table,
        2, &game->viewport_palette_buffer
    };
    /* The registered `$C1718E` callback has one counted entry per run075
     * replay frame. Its pointer publication is retained as opaque state here;
     * the future two-page owner supplies the actual table payloads. */
    if (fa18_advance_viewport_mode(&game->viewport_mode, &bindings, &game->exe,
                                   &game->viewport_copper_stream, 1, &step) != 0)
        return -1;
    if (step.palette_loaded && fa18_apply_viewport_copper_palette_to_video(
                                   &game->viewport_copper_stream, 1,
                                   &game->video) != 0)
        return -1;
    return 0;
}

static int initialize_scene_entry(void *context) {
    FA18Game *game = context;
    if (!game) return -1;
    return fa18_run_scene_entry_runtime(&game->scene_entry_runtime,
                                        game->menu_flow.selected_mode,
                                        game->menu_flow.root_type,
                                        &game->scene_initialization,
                                        &game->post_input_followup.countdown);
}

static int advance_scene_entry_callback(FA18Game *game) {
    if (!game || !game->scene_entry_armed || game->scene_entry_complete ||
        fa18_menu_flow_advance_post_input_countdown(
            &game->menu_flow, &game->post_input_followup.countdown) != 0)
        return -1;
    int result;
    switch (game->post_input_followup.callback) {
    case FA18_POST_INPUT_CALLBACK_FINISH_FOLLOWUP:
        result = fa18_finish_post_input_followup(&game->post_input_followup,
                                                  &game->viewport_mode,
                                                  initialize_scene_entry, game);
        break;
    case FA18_POST_INPUT_CALLBACK_AFTER_FINISH_FOLLOWUP:
        result = fa18_advance_post_input_followup_match(&game->post_input_followup,
                                                         &game->viewport_mode);
        break;
    case FA18_POST_INPUT_CALLBACK_COMPLETE_FOLLOWUP:
        result = fa18_complete_post_input_followup(
            &game->post_input_followup, &game->scene_initialization.scene_flag);
        break;
    default:
        return -1;
    }
    if (result < 0) return -1;
    if (result > 0 &&
        game->post_input_followup.callback ==
            FA18_POST_INPUT_CALLBACK_AFTER_FINISH_FOLLOWUP)
        game->scene_entry_initialized = 1;
    if (result > 0 &&
        game->post_input_followup.callback ==
            FA18_POST_INPUT_CALLBACK_CONTINUE_AFTER_COMPLETE_FOLLOWUP)
        game->scene_entry_complete = 1;
    return 0;
}

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
    file = fa18_disk_read(&game->disk, "pix/splsh", &size);
    loaded = file && fa18_load_splsh_ilbm_page(&game->splash_page, file, size) == 0;
    free(file);
    if (!loaded) {
        fputs("Cannot load source pix/splsh ILBM page\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (fa18_initialize_scene_renderer_defaults(&game->scene_renderer_defaults) != 0) {
        fputs("Cannot initialize source scene renderer defaults\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (fa18_initialize_renderer_page_setup(&game->renderer_page_setup) != 0) {
        fputs("Cannot initialize native renderer page setup\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (initialize_viewport_callback_state(game) != 0) {
        fputs("Cannot initialize source viewport callback state\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (fa18_load_projection_grid(&game->exe, &game->projection_grid) != 0) {
        fputs("Cannot load the C279 projection grid\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (fa18_load_scene_record_table(&game->exe, &game->scene_record_table) != 0) {
        fputs("Cannot load the C42A02 scene record table\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (fa18_load_scene_magnitude_table(&game->exe, &game->scene_magnitude_table) != 0) {
        fputs("Cannot load the C1D9D8 scene magnitude table\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (fa18_load_scene_dispatch_table(&game->exe, &game->scene_dispatch_table) != 0) {
        fputs("Cannot load the C297D2 scene dispatch table\n", stderr);
        fa18_hunks_free(&game->exe);
        fa18_disk_close(&game->disk);
        return 0;
    }
    if (fa18_scene_entry_runtime_init(&game->scene_entry_runtime, &game->exe,
                                      &game->scene_dispatch_table,
                                      &game->scene_record_table) != 0) {
        fputs("Cannot initialize the scene-entry runtime\n", stderr);
        fa18_hunks_free(&game->exe);
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
    int result;
    if (!game) return -1;
    result = fa18_menu_flow_apply_controls(&game->menu_flow, controls, &game->video);
    if (result != 0) return result;
    if (game->c279_render_fixture.enabled)
        return fa18_present_c279_render_fixture(&game->c279_render_fixture, &game->video);
    if (game->render_fixture.enabled)
        return fa18_present_scene_render_fixture(&game->render_fixture, &game->video);
    return 0;
}

int fa18_game_enable_render_fixture(FA18Game *game, const char *chip_capture_path) {
    if (!game) return -1;
    return fa18_initialize_scene_render_fixture(&game->render_fixture, &game->exe,
                                                chip_capture_path, &game->video);
}

int fa18_game_enable_c279_render_fixture(FA18Game *game, const char *slow_capture_path,
                                         const char *chip_capture_path) {
    if (!game) return -1;
    return fa18_initialize_c279_render_fixture(&game->c279_render_fixture, &game->exe,
                                               &game->projection_grid, slow_capture_path,
                                               chip_capture_path, &game->video);
}

int fa18_game_present_active_scene_render_diagnostic(FA18Game *game) {
    FA18FivePlanePage page;
    FA18FlightRendererPage renderer;
    FA18PlanarPixelState pixels = {0, 0, 0, 0};
    FA18LineStyle lines = {0, 0, 0, 0};
    FA18DefaultSceneRenderPassInput input;
    FA18DefaultSceneRenderPassResult result;
    FA18SceneActiveRecordState active_record;
    uint16_t palette[FA18_VIEWPORT_PALETTE_WORDS];

    if (!game || !game->scene_entry_initialized) return -1;
    fa18_five_plane_page_init(&page);
    if (fa18_flight_renderer_page_init(
            &renderer, &page, &pixels, &lines,
            game->scene_renderer_defaults.display_bound_y,
            game->scene_renderer_defaults.display_vertical,
            game->scene_renderer_defaults.display_horizontal, 0, 0, 0) != 0)
        return -1;
    active_record = (FA18SceneActiveRecordState){
        game->scene_entry_runtime.dispatch_runtime.record[0].bytes,
        sizeof game->scene_entry_runtime.dispatch_runtime.record[0].bytes, 0, 0
    };
    input = (FA18DefaultSceneRenderPassInput){
        active_record,
        {0, 0, 0, {
            game->scene_renderer_defaults.matrix_row_scale[0],
            game->scene_renderer_defaults.matrix_row_scale[1],
            game->scene_renderer_defaults.matrix_row_scale[2]
        }},
        0, game->scene_renderer_defaults.display_bound_y
    };
    if (fa18_render_default_active_scene_pass(
            &game->scene_entry_runtime.trig_table, &game->projection_grid, &input,
            &renderer, &result) != 0 ||
        fa18_load_viewport_mode_palette(&game->exe, game->viewport_mode.current,
                                        palette) != 0 ||
        fa18_five_plane_page_load_rgb4(&page, palette, sizeof palette / sizeof *palette) != 0)
        return -1;
    return fa18_five_plane_page_present(&page, &game->video);
}

int fa18_game_frame(FA18Game *game, const FA18ReplayControlState *controls,
                    uint16_t post_input_ticks) {
    if (!game) return -1;
    (void)controls;
    uint16_t selector;
    if (fa18_menu_flow_take_selector(&game->menu_flow, &selector) == 0) {
        FA18MenuRecord record;
        if (fa18_menu_select_message_record(&game->exe, selector, &record) != 0 ||
            fa18_render_top_level_menu(&game->video, &game->exe, &record, 1) != 0)
            return -1;
    }
    for (uint16_t tick = 0; tick < post_input_ticks; ++tick) {
        if (game->scene_entry_armed) {
            if (advance_scene_entry_callback(game) != 0) return -1;
            continue;
        }
        int result = fa18_menu_flow_post_input_tick(&game->menu_flow);
        if (result < 0) return -1;
        if (result > 0) {
            /* `$C1000A` has selected `$C0FA04`; its delay of four belongs to
             * the same externally supplied `$C0F5F8` scheduler stream. */
            game->post_input_followup = (FA18PostInputFollowupState){
                0, 0, 0, game->menu_flow.display_delay,
                FA18_POST_INPUT_CALLBACK_FINISH_FOLLOWUP
            };
            game->scene_entry_armed = 1;
        }
    }
    if (advance_viewport_callback(game) != 0) return -1;
    fa18_menu_flow_finish_presented_frame(&game->menu_flow, &game->video);
    ++game->frame;
    return 0;
}
