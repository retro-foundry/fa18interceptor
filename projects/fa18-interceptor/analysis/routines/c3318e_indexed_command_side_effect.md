# `$C3318E`: indexed-command side-effect wrapper

Classification: **bounded dataflow; one F1 scenario anchor**.

The byte-exact entry `$C3318E-$C331CD` first tests `$C457D7`.  When clear, it
constructs a fixed argument packet (`D0=2`, `D1=4`, then `D0=$12C`, `D2=1`,
`D3=$12C`, `D4=4`, `D5=1`) and calls `$C17EF2`; it preserves `D0-D7/A0-A4`
around that call.  A positive `$C4588A` skips the packet.  A nonzero
`$C457D7` branches to external predecessor `$C33180`.

Authority: the controlled run029 F1 variant, created by replacing only the
frame-266/269 key-5 rows with native F1 (`key=282`, character 0), then using a
prefix through frame 266.  `$C1BD78` receives `D4` low byte 10 and reaches
this wrapper without writing `$C458A6`.  The bounded trace enters `$C3318E`
at frame 266 and returns to `$C1BDF8` after 1,923 instructions.  Its nested
calls include `$C17EF2`, `$C17B08`, `$C17B2C`, and `$C4FFB0`.  Repeating F1
after the controlled digit-6 submenu entry takes the same pre-wrapper
zero-record route; see `analysis/run029_key6_selectable_missions_probe.md`.

This demonstrates that an F1 event takes a downstream indexed-state side
effect path, not an immediate `$C4574A` static-message selection.  The
meaning of the `$C17EF2` packet and whether a later update consumes its state
for mission selection remain unknown.  Its bounded stack-slot contract is
documented in `analysis/routines/c17ef2_guarded_command_packet.md`.  The
containing byte-exact source is
`source_amiga/observed/route_guarded_command_side_effect.asm`; no overlapping
entry-only slice is retained.

The run075 key-1 demo selection supplies a second scenario branch. After
`$C1BDEC` writes `$C458A6=$7F` in direct-core frame 230, the caller enters
`$C3318E` with `$C457D7!=0`. It jumps to `$C33180`, selects `D0=2,D1=2`,
passes the nonpositive `$C4588A` guard, and constructs the `$12C`/`1` packet
for `$C17EF2`. This distinguishes the demo path's `D1=2` from the F1
packet's `D1=4` without assigning a sound or gameplay purpose to the helper.
Trace: `build/port_run075_c1bd78_trace/trace.jsonl`, beginning at frame 230;
the wrapper's byte-exact source above supplies the full return path.
