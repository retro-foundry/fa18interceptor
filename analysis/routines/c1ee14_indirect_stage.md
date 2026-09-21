# Indirect stage target at `$C1EE14` (Hunk 10 +`$DCC`)

Classification: **structural**. This is the real dynamic target of the
`JSR (A2)` at `$C1CC86` in the captured human-flight invocation. It has no
assigned simulation, display, or input ownership.

## Runtime packet

- State: `run001` frame 2,696 plus its sole rebased joystick press.
- The stage packet executes `$C1CC86  JSR (A2)` with target `$C1EE14`.
- Breakpoint `$C1EE14`, frame 9; complete return to `$C1CC88`.
- 7,866 instructions.
- P-code: `pcode/raw/run001_c1ee14_indirect_target/`, 1,523 observed RAM
  starts / 10,404 operations. 1,471 starts map to resolved Hunks; 52 remain
  unmapped.

This supersedes the earlier stack-only association between `$C1CC86` and the
JOY0DAT callback. `$C1718E` remains independently proven as a registered
hardware callback, but this particular dynamic indirect call target is
`$C1EE14`.
