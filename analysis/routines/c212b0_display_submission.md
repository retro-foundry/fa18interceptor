# `submit_display_line_records` at `$C212B0` (Hunk 14 +`$250`)

Classification: **behavioural**, with a bounded display-only contract. This
verified indirect-dispatch target prepares line-record inputs and invokes the
known blitter line path. It is not assigned to an aircraft, terrain, HUD, or
other game object.

## Runtime packet

- No-input `start_demo` replay from `captures/baseline_menu/state.bin`.
- Breakpoint `$C212B0`, hit in frame 602; observed return `$C1F944`.
- 2,552 instructions, no input after the breakpoint.
- P-code: `pcode/raw/no_key_c212b0_display/`, 236 observed RAM starts / 1,434
  operations, all mapped to verified Hunks.

The function repeats this exact observed call chain ten times:

```text
$C21304  jsr $C2EE4A
  $C2EEAC  bsr $C2F0F4
    $C2F088  jsr $C2FA7E
      blitter_draw_line_to_enabled_planes
```

Immediately before `$C21304`, it assembles words through `A0` from two
source positions relative to `A3`, tests a combined word for a negative result,
and pushes `A2`. The source-record layout and line semantics remain unknown.

The complete target ends at `$C2131A` with `RTS`; the indirect dispatcher at
`$C1F942` calls it and receives control at `$C1F944`. The dispatcher table
selector is still unknown.
