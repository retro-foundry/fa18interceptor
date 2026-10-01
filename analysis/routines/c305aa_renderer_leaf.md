# Polygon-edge renderer at `$C305AA`

Classification: **registered, source-timed C bridge**. The complete original
range `$C305AA-$C306B2` is represented by `glue_C305AA_step` in
`port/game/glue/glue_render_polygon_step.c`; the exclusive dispatcher boundary
is `$C306B4`. The earlier run001 observation covered only the four-instruction
equal-edge return. The bridge now covers both unequal-edge paths, signed slope
construction, clip-mask updates, blitter setup, busy polling, and both returns.

The bridge advances at original instruction boundaries. It preserves word
arithmetic and high register halves, source branch order, ordered custom-chip
writes, source-dependent 68000 `MULS`/`DIVS` timing, and divide-by-zero exception
behavior. The readable domain implementation remains in
`port/game/render_polygon.c`; the step bridge supplies timing and resumable
execution across hardware events.

## Proof

`python tools/recomp/check_active_planes_step.py --group polygon` compares all
144 polygon-bridge instructions over 4,608 fixtures. The combined bridge oracle
matches 1,163 instructions over 37,216 fixtures. Every fixture compares all
registers, full SR, PC, instruction cycles and RAM against the original
instruction.

Fresh isolated `PORTS_ONLY=C305AA` source-OFF and C-ON streams match every
RGB444 frame in all three sealed recordings. GNU and MSVC Release builds pass.
The complete 414-entry gate still matches 721,752 shadow calls and 1,169,653
sandbox calls with zero mismatches, sealed RAM, and identical poison frames.

The complete registered set still first differs in the 500-frame demo at frame
419 by 29,453 pixels, so remaining registered timing debt is distributed among
other fixed-charge entries. `scripts/probe_recomp_timing.py` currently ranks
C0FA04 as the earliest isolated difference, at frame 401 by 361 pixels.

Historical evidence remains in `pcode/raw/run001_c305aa_renderer_child/` and
`source_amiga/observed/prepare_unequal_pair_range.asm`.
