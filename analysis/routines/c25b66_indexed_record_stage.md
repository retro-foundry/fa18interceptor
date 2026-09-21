# Indexed record stage at `$C25B66`

Classification: **bounded structural stage**. In the sealed run003 frame-6,000
normal update, `$C22D88` calls `$C25B66` and execution returns to `$C22D8E`
after 2,603 instructions. Future playback is empty. Canonical P-code is
`pcode/raw/run003_6000_c25b66/` (1,831 RAM instruction starts, 12,079
operations, 29 observed call targets).

The packet enters from the capped `$C22C80` indexed-record stage and reaches
the established current-record selector `$C13D84`, matrix helper `$C2D408`,
and multiple nested record/update helpers. Its record ownership and aggregate
semantics are not yet assigned. This is a complete caller-return packet, not a
claim that `$C25B66` is a standalone source-level function.

The frame-6000 trace's entry fragments are byte-exact source:
`initialize_indexed_update_gate.asm`, `gate_indexed_update_event.asm`,
`route_zero_indexed_update.asm`, `gate_indexed_update_record_flag.asm`, and
`gate_indexed_update_control_path.asm`. They establish the selected-index,
event-byte, record-flag, and control-bit gates without claiming untraced
alternatives.

The later zero-index path now also has byte-exact gates and the direct
`$C25E24 -> $C149BE` transform call in `gate_zero_indexed_transform.asm`,
`gate_indexed_transform_control.asm`, and `run_indexed_update_transform.asm`.
The next traced slices set up, gate, and publish the two observed bounded
coordinate deltas at `+$14` and `+$1C`; no physical coordinate meaning is
assigned to those fields.
The subsequent `$C25E86-$C25F01` slices preserve the observed fixed-point
normalization and writes at `+$0C/$0E/$10`, without assigning coordinate axes.
