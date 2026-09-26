# `$C2F688` two-row mask path in run075 frame 315

Meaning: **port-contract for the alternate table of a renderer primitive**.
Authority is the sealed `captures/run075` replay and the 52-instruction
entry-to-return trace `build/port_run075_c2f688_52_chip/`, including exact
Chip RAM before and after. Static authority for all 16 offset handlers is
`source_amiga/observed/apply_offset_renderer_lane_masks.asm`. This is a
renderer operation; the identity of the drawn object is not assigned.

The call enters `$C2F688` with `(D0.w,D1.w)=(172,99)` in direct-core frame
315. `A3=$C2F7C6` selects the 16-word alternate mask table, and
`A4=$C2F7E6` selects the alternate 16-target handler table. The low nibble of
`$C45954=$100B` selects handler 11 at `$C2F9EE`. `$C456E7=$0F` enables all
four planes; `$C456E8=$FFFF` takes the direct dispatch path, without the
optional pre-handler XOR. Index 12 of the source mask table is `$0018`.
For the 16-pixel word covering x=160..175, those two bits correspond to
x=171 and x=172. The address offset is `40*99 + 20 = 3980 ($0F8C)`.

The selected handler applies the mask at that offset and again at `+$28`
(40 bytes, the next row). It ORs the mask into planes at `A3`, `A2`, and `A0`
and ANDs its complement into `A1`. The four-plane bases from `$C4566E`
are, in plane-bit order, `$04DB30`, `$04FA70`, `$0519B0`, `$0538F0`.
Deplanarized, this writes color index `$B` to x=171..172 on y=99..100.

| Game pixel | Before | After |
| --- | ---: | ---: |
| (171,99) | 4 | 11 |
| (172,99) | 4 | 11 |
| (171,100) | 11 | 11 |
| (172,100) | 11 | 11 |

Only four Chip-RAM bytes change because the second row already has index 11.
The native `fa18_apply_pixel_mask` takes a `FA18RendererState` and
`FA18_PIXEL_TWO_ROWS` selector. It implements the exact 16-word table, the
two-row indexed writes, enabled-plane preservation, and the common
pre-dispatch output-XOR rule. The state fields represent proved renderer
behavior (`draw_mode`, active planes, XOR enable, XOR-plane mask); they do not
model Amiga memory addresses. `port/renderer_contract_test.c` checks this
captured before/after fixture, plus a primary-table and XOR fixture. Original
`y<=0` rejection is retained. The native visual buffer deliberately rejects
coordinates outside 320x200 instead of writing arbitrary Amiga memory. No
Copper palette or visible-surface ownership claim follows from this trace.
