# `$C2F558` outer-loop pointer selector

Classification: **dataflow**: complete per-iteration child of the outer loop.

- Restore: `captures/baseline_menu/state.bin`; playback `local/start_demo.e9k`.
- Breakpoint `$C2F558`, armed at frame 600, hit at frame 607.
- Returns to `$C15D9C` in nine instructions; no later input occurs while
  stepping.
- P-code: `pcode/raw/no_key_c2f558_outer_child/` (9 starts / 26 operations).

The selector chooses base pointers at `$C4566E/$C4568E`, adding `$10/$14`
when `$C4566C` is non-zero, then publishes them at `$C456B6/$C456BA`.
`source_amiga/observed/select_outer_loop_pointer_pair.asm` is byte-exact for
the complete `$C2F558-$C2F581` routine (42 bytes).

The runtime-backed renderer entry at `$C2F5F4` loads the first published
pointer from `$C456B6` before deriving its four adjusted output pointers. This
proves the selector is a producer for that renderer pointer block. It does not
by itself assign a graphics or object meaning to either pointer pair.

### Pointer publisher trace from cockpit state

A return-bounded no-input trace from `build/attract_focus_1800/state.bin` hits `$C2F558` at replay frame 3 and returns to `$C15D9C` after nine instructions. This independently confirms the active attract-mode path invokes the selector; its register rows are preserved in `build/attract_cockpit_c2f558_trace/trace.jsonl`.

### Normal run029 frame-993 selector trace

`build/run029_normal_c2f558_993/` reaches the same routine during ordinary
replay in frame 993, called from `$C15D96` and returning to `$C15D9C` after
seven executed instructions. `$C4566C` is zero, so the `BEQ` at `$C2F56A`
selects the unadjusted pair. The trace records the exact publications:

- `$C2F574`: `MOVE.L A0,$C456B6`, with `A0=$C4566E`;
- `$C2F57A`: `MOVE.L A1,$C456BA`, with `A1=$C4568E`.

At the corresponding normal snapshots, `$C4566E` contains `$0538F0`,
`$0519B0`, `$04FA70`, and `$04DB30`, respectively: the four visible planes
from plane 4 through plane 1 in the run029 Copper section. The prior frame
still has `$C456B6=$C4567E`, whose entries are the older `$018980` through
`$012BC0` family. This identifies an observed table-selector transition, not
the point at which any individual numeric glyph is drawn.
