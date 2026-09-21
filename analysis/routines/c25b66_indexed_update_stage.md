# `$C25B66` indexed update stage

Classification: **behavioural indexed-update evidence**. The record type and
subsystem are not yet named.

## Runtime packet

- Restored state: `run001` frame 2,696 plus its sole rebased joystick press;
  no later input occurs while the interval is instruction-stepped.
- The live call edge is `$C22D88 -> $C25B66 -> $C22D8E` at replay frame 17.
- It completes at that real return boundary in 2,688 instructions.
- P-code: `pcode/raw/run001_c25b66_update_stage/`, 1,803 observed RAM starts,
  11,570 P-code operations, and 31 dynamically reached call-target entries.

## Observed sequence

The stage calls `$C13D84` at `$C25D7E`, giving the previously bounded
joystick-state consumer its direct enclosing invocation. Later it calls
`$C2D408`, whose observed route enters the existing matrix helpers
`$C2DD4E`, `$C2DEE0`, `$C2DF02`, `$C2D968`, and `$C2D996`. It also reaches
`$C149BE` and the downstream `$C2600E -> $C26EBE` path.

The packet therefore joins the verified joystick-state, indexed-record, and
matrix work inside one complete per-record update interval. It does not yet
prove that this is the player, camera, or a particular aircraft record.
