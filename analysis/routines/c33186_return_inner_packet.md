# `$C33186` inner packet reached by raw `$44`

Classification: **complete inner execution packet** for the sealed `run002`
Return route. This is not a claim that `$C33186` is exclusively a
weapon-selection function.

## Runtime evidence

- State: `run002` replay through frame 5,171, then isolated `K 13 0 16 1`.
- Raw `$44` reaches `$C1BB7A`; the measured branch calls `$C33186` at
  `$C1BBBA` and resumes at `$C1BBC0`.
- Breakpoint trace `$C33186 -> $C1BBC0` completes in 1,924 instructions.
- No future input occurs while stepping. P-code:
  `pcode/raw/run002_return_c33186/` (147 observed RAM starts / 1,025
  operations).

The packet calls `$C17EF2`, repeatedly reaches `$C17B08` and `$C4FFB0`, and
later calls `$C501E0`. It is retained as a bounded, complete path for
frame-by-frame decomposition; its subsystem role and full static function
extent remain unassigned.
