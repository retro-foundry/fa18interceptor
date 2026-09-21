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
