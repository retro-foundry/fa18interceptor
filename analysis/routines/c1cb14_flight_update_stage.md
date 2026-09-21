# Flight update stage at `$C1CB14` (Hunk 8 +`$84C`)

Classification: **structural, capped**. This is a human-flight invocation of a
direct child of the long update sequence. It is not a completed function
contract and is not assigned a subsystem name beyond its observed placement.

## Proven edge and runtime packet

- The live stack at the frame-9 breakpoint proves
  `$C0F0A6  JSR $C1CB14 → $C0F0AC`.
- State: `run001` frame 2,696 plus the single rebased frame-2,697 joystick
  press; no later input exists in the trace recording.
- Breakpoint `$C1CB14`, hit frame 9; requested return `$C0F0AC`.
- The packet reaches its 20,000-instruction cap without returning. Its
  `termination` is `max_instructions`, so it must never be cited as a complete
  routine execution.
- P-code: `pcode/raw/run001_c1cb14_update_stage/`, 1,991 observed RAM starts
  and 13,883 operations. 1,895 starts map to resolved Hunks; 96 remain
  unmapped and are retained rather than attributed to game code.

The capped trace repeatedly enters the verified Hunk-36 range around
`$C2FCxx-$C304xx`, which includes known renderer code. That execution overlap
does not establish that `$C1CB14` owns rendering or its data.

## Observed scene-table handoff

The executed `$C1CB74-$C1CCB6` loop walks 24-byte records selected through the
word offset at `$C459AA`; `$C4E9AA` is used as its base when `$C45865` is zero.
Each selected record supplies a descriptor pointer in `A1`. After the loop
copies the selected record's three initial words into `$C45B2A/$C45B30`, it
consumes longwords from that descriptor in order:

| Descriptor field order | Destination / use |
| --- | --- |
| first | loaded into `A2`, then called indirectly |
| second | loaded into `A0` |
| third | stored at `$C45A36` |
| fourth | stored at `$C45A3A` |

The third of these is the exact producer of the control-stream pointer used
by `$C1F6F8` and then by the projected-edge path. This proves that scene table
records, rather than the projection tail, select the stream. The names,
ownership, and visual identity of individual records remain unassigned until
the run024 landmark packets are captured.

A normal replay breakpoint at `$C1CC70`, armed immediately before run024 frame
23,000 (the user-identified San Francisco / distant Golden Gate view), proves
that the loop is active in that visible scene. Its first observed descriptor is
`$C22A8C`, whose first five longwords are
`$00C454F4, $00C454F4, $00000000, $00C096CA, $00C4558C`. In particular, its
third longword would write zero to `$C45A36`; it is therefore a non-geometry
descriptor. The sample proves traversal of the selection system in the
landmark frame, not the descriptor or edge list that draws the bridge.
