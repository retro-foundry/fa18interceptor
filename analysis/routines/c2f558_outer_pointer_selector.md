# `$C2F558` outer-loop pointer selector

Classification: **complete per-iteration structural child** of the outer loop.

- Restore: `captures/baseline_menu/state.bin`; playback `local/start_demo.e9k`.
- Breakpoint `$C2F558`, armed at frame 600, hit at frame 607.
- Returns to `$C15D9C` in nine instructions; no later input occurs while
  stepping.
- P-code: `pcode/raw/no_key_c2f558_outer_child/` (9 starts / 26 operations).

The selector chooses base pointers at `$C4566E/$C4568E`, adding `$10/$14`
when `$C4566C` is non-zero, then publishes them at `$C456B6/$C456BA`.
`source_amiga/observed/select_outer_loop_pointer_pair.asm` is byte-exact for
the complete `$C2F558-$C2F581` routine (42 bytes).

### Pointer publisher trace from cockpit state

A return-bounded no-input trace from `build/attract_focus_1800/state.bin` hits `$C2F558` at replay frame 3 and returns to `$C15D9C` after nine instructions. This independently confirms the active attract-mode path invokes the selector; its register rows are preserved in `build/attract_cockpit_c2f558_trace/trace.jsonl`.
