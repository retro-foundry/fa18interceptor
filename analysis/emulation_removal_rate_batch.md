# Direct update-stage rate classification

Before the native frontend direction, C1C63E was connected directly to
`classify_record_rate` in `port/game/control_records.c`. The 36 C1C7F6
instruction cases and CPU entry adapter are deleted. The connected update
chain has removed 928 instruction cases. Guest data and outer timing remain.

GNU/MSVC runners build; twelve CTests and GNU profiling invariance pass.
4,096 original-child update-stage calls match all registers, PC, SR and RAM.
4,096 composed calls match registers, PC, SR and RAM outside the previously
documented CPU stack scratch. Events are held in these component checks.

Three 800-frame probes reach 43/15/75 direct rate calls. Schema-2 raw CPU
minimum increases from 0.4469635% to 0.4545609% (+0.0075973 percentage points).
Demo/carrier RGB and indices match the prior build; crash pixels and all final
RAM hashes differ. Source parity remains failing; accepted removal is
unavailable, and zero of four subsystems can be deleted. No full replay ran.
See `emulation_removal_rate_meter_probe.json/.md` for scoped measurements.

The user's later direction replaces this incremental strategy with a native
intro-to-menu runner reusing game source. These changes are preserved separately.
