# Record-walk stage from `$C1F6F8`

Classification: **capped structural packet**. The stack at the live entry
contains return PC `$C30001`, proving this stage is called from the renderer
path. The trace did not complete, so this is not a recovered function contract.

## Runtime packet

- Breakpoint `$C1F6F8`, run001 replay frame 9, with no future input.
- Live entry stack top: `$C30001`.
- The constrained trace reached its 30,000-instruction cap without returning
  to `$C30001`; retain it as capped.
- P-code: `pcode/raw/run001_c1f6f8_record_walk_stage/`, 2,283 observed RAM
  starts / 16,083 operations, with 55 observed call-target candidates.

The observed prefix initializes `$C456E6` to `$000FFFFF`, clears `$C456EA`,
loads a list pointer from `$C45A36`, and enters the `$C1F7xx/$C1F9xx` control
and record-dispatch path. The shorter source slices at `$C1F7A0` and
`$C1F910` establish portions of that path, but the long stage requires more
sub-boundaries before it can receive a broader name or ownership claim.
