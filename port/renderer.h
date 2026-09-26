#ifndef FA18_RENDERER_H
#define FA18_RENDERER_H

#include <stdint.h>

enum { FA18_WIDTH = 320, FA18_HEIGHT = 200 };

/* The port's four-bit render target. Each byte stores one native palette
 * index, replacing four separate Amiga bitplanes. */
typedef struct {
    uint8_t pixels[FA18_WIDTH * FA18_HEIGHT];
} FA18IndexedFrameBuffer;

/* $C2FD22 clears all active four-plane words. In the native target this is a
 * complete zero-index clear of the supplied chunky work buffer. */
void fa18_clear_renderer_work_buffer(FA18IndexedFrameBuffer *framebuffer);

typedef enum {
    FA18_PIXEL_PRIMARY,   /* $C2F766 / $C2F786 -> $C2F826-$C2F8CF */
    FA18_PIXEL_TWO_ROWS   /* $C2F7C6 / $C2F7E6 -> $C2F8D0-$C2FA6F */
} FA18PixelTable;

/* Proved renderer configuration from $C45954, $C456E7, $C456E8 and $C456EB.
 * These fields describe behavior, not an emulated Slow-RAM layout.
 */
typedef struct {
    uint8_t draw_mode;
    uint8_t active_plane_mask;
    int16_t output_xor_enable;
    uint8_t output_xor_plane_mask;
} FA18RendererState;

/* Complete visual-buffer contract of $C2F688-$C2FA6F. `indices` is a
 * deplanarized four-bit buffer. A nonnegative output_xor_enable and nonzero
 * output_xor_plane_mask XORs selected lanes on the first row and returns
 * before dispatch. Returns 0 after a write, 1 for original D1.w <= 0 no-op,
 * or -1 when the original address falls outside this visual-buffer model.
 */
int fa18_apply_pixel_mask(FA18IndexedFrameBuffer *framebuffer,
                          const FA18RendererState *state,
                          FA18PixelTable table, int x, int y);

#endif
