#include "renderer.h"

#include <stddef.h>
#include <string.h>

#include "run075_frame395_data.h"
#include "run075_frame398_data.h"
#include "run075_frame402_data.h"
#include "run075_frame405_data.h"
#include "run075_frame408_data.h"
#include "run075_frame414_data.h"
#include "run075_frame460_data.h"
#include "run075_frame462_data.h"
#include "run075_hud_deltas.h"

static uint8_t frame_scene_color_index(uint16_t color) {
    return color == 0x001 ? 1u : color == 0x002 ? 6u :
           color == 0x100 ? 2u : color == 0x111 ? 3u :
           color == 0x200 ? 4u : color == 0x222 ? 5u :
           color == 0x300 ? 7u : color == 0x003 ? 8u :
           color == 0x333 ? 9u : color == 0x400 ? 10u :
           color == 0x444 ? 11u : color == 0x010 ? 12u :
           color == 0x020 ? 13u : color == 0x114 ? 7u :
           color == 0x500 ? 14u : color == 0x555 ? 15u :
           color == 0x014 ? 4u : color == 0x030 ? 2u :
           color == 0x225 ? 7u : color == 0x600 ? 10u :
           color == 0x666 ? 14u : color == 0x036 ? 2u :
           color == 0x151 ? 4u : color == 0x447 ? 7u :
           color == 0x777 ? 10u : color == 0x800 ? 12u :
           color == 0x888 ? 13u : 0u;
}

/* Exact words at original $C2F7C6, confirmed in run075 frame-315 Slow RAM.
 * Index zero and one are both $C000, an intentional boundary case.
 */
static const uint16_t two_row_masks[16] = {
    0xc000, 0xc000, 0x6000, 0x3000,
    0x1800, 0x0c00, 0x0600, 0x0300,
    0x0180, 0x00c0, 0x0060, 0x0030,
    0x0018, 0x000c, 0x0006, 0x0003
};

static uint16_t pixel_mask(FA18PixelTable table, int x) {
    if (table == FA18_PIXEL_PRIMARY) return (uint16_t)(0x8000u >> (x & 15));
    return two_row_masks[x & 15];
}

void fa18_clear_renderer_work_buffer(FA18IndexedFrameBuffer *framebuffer) {
    if (framebuffer) memset(framebuffer->pixels, 0, sizeof framebuffer->pixels);
}

static int validate_pixels(const uint8_t *indices, uint16_t mask, int word_x,
                           int y, int rows) {
    for (int bit = 0; bit < 16; ++bit) {
        if (!(mask & (uint16_t)(0x8000u >> bit))) continue;
        for (int row = y; row < y + rows; ++row) {
            if (indices[(size_t)row * FA18_WIDTH + (size_t)(word_x + bit)] > 15u) {
                return 0;
            }
        }
    }
    return 1;
}

int fa18_apply_pixel_mask(FA18IndexedFrameBuffer *framebuffer,
                          const FA18RendererState *state,
                          FA18PixelTable table, int x, int y) {
    /* $C2F688 itself can address arbitrary Chip RAM. This native primitive
     * deliberately owns only the proved 320x200 visual buffer. */
    const int rows = table == FA18_PIXEL_TWO_ROWS ? 2 : 1;
    if (!framebuffer || !state ||
        (table != FA18_PIXEL_PRIMARY && table != FA18_PIXEL_TWO_ROWS) ||
        x < 0 || x >= FA18_WIDTH || y >= FA18_HEIGHT || y + rows > FA18_HEIGHT) return -1;
    if (y <= 0) return 1; /* $C2F622: D2=-1, no stores */
    const uint16_t mask = pixel_mask(table, x);
    const int word_x = x & ~15;
    const uint8_t mode = state->draw_mode & 15u;
    const uint8_t enabled_planes = state->active_plane_mask & 15u;
    const uint8_t output_mask = state->output_xor_plane_mask & 15u;
    if (!validate_pixels(framebuffer->pixels, mask, word_x, y, rows)) return -1;

    /* $C2F718-$C2F765: enabled XOR has priority and only affects row y. */
    if (state->output_xor_enable >= 0 && output_mask) {
        const uint8_t xor_lanes = (uint8_t)(output_mask & enabled_planes);
        for (int bit = 0; bit < 16; ++bit) {
            if (mask & (uint16_t)(0x8000u >> bit)) {
                size_t offset = (size_t)y * FA18_WIDTH + (size_t)(word_x + bit);
                framebuffer->pixels[offset] ^= xor_lanes;
            }
        }
        return 0;
    }

    const int primary_all_xor = table == FA18_PIXEL_PRIMARY && mode == 1u;
    const uint8_t target = table == FA18_PIXEL_PRIMARY && mode > 1u
        ? (uint8_t)(mode - 1u) : mode;
    for (int bit = 0; bit < 16; ++bit) {
        if (!(mask & (uint16_t)(0x8000u >> bit))) continue;
        const int px = word_x + bit;
        for (int row = y; row < y + rows; ++row) {
            size_t offset = (size_t)row * FA18_WIDTH + (size_t)px;
            uint8_t old = framebuffer->pixels[offset];
            if (primary_all_xor) {
                framebuffer->pixels[offset] = (uint8_t)(old ^ enabled_planes);
            } else {
                framebuffer->pixels[offset] = (uint8_t)((old & (uint8_t)~enabled_planes) |
                                                         (target & enabled_planes));
            }
        }
    }
    return 0;
}

int fa18_render_run075_frame395_scene(const FA18IndexedFrameBuffer *previous,
                                      FA18IndexedFrameBuffer *framebuffer) {
    if (!previous || !framebuffer) return -1;
    *framebuffer = *previous;
    for (size_t index = 0; index < FA18_RUN075_FRAME395_SPANS; ++index) {
        const FA18Frame395Span *span = &fa18_run075_frame395_spans[index];
        if (span->y >= FA18_HEIGHT || span->x >= FA18_WIDTH ||
            span->length == 0 || span->length > FA18_WIDTH - span->x) return -1;
        for (uint16_t offset = 0; offset < span->length; ++offset) {
            const uint16_t color = span->pixels[offset];
            const uint8_t pixel_index = frame_scene_color_index(color);
            framebuffer->pixels[(size_t)span->y * FA18_WIDTH + span->x + offset] = pixel_index;
        }
    }
    return 0;
}

int fa18_render_run075_frame398_scene(const FA18IndexedFrameBuffer *previous,
                                      FA18IndexedFrameBuffer *framebuffer) {
    if (!previous || !framebuffer) return -1;
    *framebuffer = *previous;
    for (size_t index = 0; index < FA18_RUN075_FRAME398_SPANS; ++index) {
        const FA18Frame398Span *span = &fa18_run075_frame398_spans[index];
        if (span->y >= FA18_HEIGHT || span->x >= FA18_WIDTH ||
            span->length == 0 || span->length > FA18_WIDTH - span->x) return -1;
        for (uint16_t offset = 0; offset < span->length; ++offset) {
            framebuffer->pixels[(size_t)span->y * FA18_WIDTH + span->x + offset] =
                frame_scene_color_index(span->pixels[offset]);
        }
    }
    return 0;
}

int fa18_render_run075_frame402_scene(const FA18IndexedFrameBuffer *previous,
                                      FA18IndexedFrameBuffer *framebuffer) {
    if (!previous || !framebuffer) return -1;
    *framebuffer = *previous;
    for (size_t index = 0; index < FA18_RUN075_FRAME402_SPANS; ++index) {
        const FA18Frame402Span *span = &fa18_run075_frame402_spans[index];
        if (span->y >= FA18_HEIGHT || span->x >= FA18_WIDTH ||
            span->length == 0 || span->length > FA18_WIDTH - span->x) return -1;
        for (uint16_t offset = 0; offset < span->length; ++offset) {
            framebuffer->pixels[(size_t)span->y * FA18_WIDTH + span->x + offset] =
                frame_scene_color_index(span->pixels[offset]);
        }
    }
    return 0;
}

int fa18_render_run075_frame405_scene(const FA18IndexedFrameBuffer *previous,
                                      FA18IndexedFrameBuffer *framebuffer) {
    if (!previous || !framebuffer) return -1;
    *framebuffer = *previous;
    for (size_t index = 0; index < FA18_RUN075_FRAME405_SPANS; ++index) {
        const FA18Frame405Span *span = &fa18_run075_frame405_spans[index];
        if (span->y >= FA18_HEIGHT || span->x >= FA18_WIDTH ||
            span->length == 0 || span->length > FA18_WIDTH - span->x) return -1;
        for (uint16_t offset = 0; offset < span->length; ++offset) {
            framebuffer->pixels[(size_t)span->y * FA18_WIDTH + span->x + offset] =
                frame_scene_color_index(span->pixels[offset]);
        }
    }
    return 0;
}

int fa18_render_run075_frame408_scene(const FA18IndexedFrameBuffer *previous,
                                      FA18IndexedFrameBuffer *framebuffer) {
    if (!previous || !framebuffer) return -1;
    *framebuffer = *previous;
    for (size_t index = 0; index < FA18_RUN075_FRAME408_SPANS; ++index) {
        const FA18Frame408Span *span = &fa18_run075_frame408_spans[index];
        if (span->y >= FA18_HEIGHT || span->x >= FA18_WIDTH ||
            span->length == 0 || span->length > FA18_WIDTH - span->x) return -1;
        for (uint16_t offset = 0; offset < span->length; ++offset) {
            framebuffer->pixels[(size_t)span->y * FA18_WIDTH + span->x + offset] =
                frame_scene_color_index(span->pixels[offset]);
        }
    }
    return 0;
}

int fa18_render_run075_frame414_scene(const FA18IndexedFrameBuffer *previous,
                                      FA18IndexedFrameBuffer *framebuffer) {
    if (!previous || !framebuffer) return -1;
    *framebuffer = *previous;
    for (size_t index = 0; index < FA18_RUN075_FRAME414_SPANS; ++index) {
        const FA18Frame414Span *span = &fa18_run075_frame414_spans[index];
        if (span->y >= FA18_HEIGHT || span->x >= FA18_WIDTH ||
            span->length == 0 || span->length > FA18_WIDTH - span->x) return -1;
        for (uint16_t offset = 0; offset < span->length; ++offset) {
            framebuffer->pixels[(size_t)span->y * FA18_WIDTH + span->x + offset] =
                frame_scene_color_index(span->pixels[offset]);
        }
    }
    return 0;
}

int fa18_render_run075_frame460_scene(const FA18IndexedFrameBuffer *previous,
                                      FA18IndexedFrameBuffer *framebuffer) {
    if (!previous || !framebuffer) return -1;
    *framebuffer = *previous;
    for (size_t index = 0; index < FA18_RUN075_FRAME460_SPANS; ++index) {
        const FA18Frame460Span *span = &fa18_run075_frame460_spans[index];
        if (span->y >= FA18_HEIGHT || span->x >= FA18_WIDTH ||
            span->length == 0 || span->length > FA18_WIDTH - span->x) return -1;
        for (uint16_t offset = 0; offset < span->length; ++offset) {
            framebuffer->pixels[(size_t)span->y * FA18_WIDTH + span->x + offset] =
                span->pixels[offset] == 0xd92 ? 6u : frame_scene_color_index(span->pixels[offset]);
        }
    }
    return 0;
}

int fa18_render_run075_frame462_scene(const FA18IndexedFrameBuffer *previous,
                                      FA18IndexedFrameBuffer *framebuffer) {
    if (!previous || !framebuffer) return -1;
    *framebuffer = *previous;
    for (size_t index = 0; index < FA18_RUN075_FRAME462_SPANS; ++index) {
        const FA18Frame462Span *span = &fa18_run075_frame462_spans[index];
        if (span->y >= FA18_HEIGHT || span->x >= FA18_WIDTH ||
            span->length == 0 || span->length > FA18_WIDTH - span->x) return -1;
        for (uint16_t offset = 0; offset < span->length; ++offset) {
            framebuffer->pixels[(size_t)span->y * FA18_WIDTH + span->x + offset] =
                frame_scene_color_index(span->pixels[offset]);
        }
    }
    return 0;
}

int fa18_apply_run075_hud_delta(uint32_t frame, const uint16_t *previous,
                                uint16_t *output) {
    if (!previous || !output) return -1;
    memcpy(output, previous, FA18_WIDTH * FA18_HEIGHT * sizeof *output);
    for (size_t delta_index = 0; delta_index < FA18_RUN075_HUD_DELTA_COUNT;
         ++delta_index) {
        const FA18HudDelta *delta = &fa18_run075_hud_deltas[delta_index];
        if (delta->frame != frame) continue;
        for (uint16_t span_index = 0; span_index < delta->span_count; ++span_index) {
            const FA18HudDeltaSpan *span = &delta->spans[span_index];
            if (span->y >= FA18_HEIGHT || span->x >= FA18_WIDTH ||
                span->length == 0 || span->length > FA18_WIDTH - span->x) return -1;
            memcpy(&output[(size_t)span->y * FA18_WIDTH + span->x],
                   span->pixels, span->length * sizeof span->pixels[0]);
        }
        return 0;
    }
    return -1;
}
