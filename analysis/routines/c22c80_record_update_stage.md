# `$C22C80` record-update stage

Classification: **behavioural record-update evidence**. The record class is
unknown, so this is not named as player, camera, or AI state.

## Runtime packet

- Restored state: `run001` frame 2,696 plus its sole rebased joystick press;
  there is no later input while stepping.
- The live call edge is `$C1C6B6 -> $C22C80 -> $C1C6BC` at replay frame 17.
- It returns cleanly in 3,155 instructions.
- P-code: `pcode/raw/run001_c22c80_record_update_stage/`, 2,093 observed RAM
  starts, 12,940 P-code operations, and 41 dynamically reached call-target
  entries.

## Observed sequence

The stage clears `$C459B4/$C459B6`, prepares `$C46184` and `$C46984` record
bases, then calls `$C230B0`, `$C23228`, `$C23CA6`, `$C244E2`, and the complete
`$C25B66` stage. Its subsequent observed route revisits `$C25B66` through
`$C23070` and performs additional `$C230E8/$C23116/$C231A2/$C23A7E` calls.

This packet contains the whole measured `$C25B66 -> $C13D84` control path and
therefore is the current highest bounded caller on that path. It remains a
per-record update classification until separate inputs and record-state
experiments establish ownership.
