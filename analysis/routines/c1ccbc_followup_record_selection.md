# `$C1CCBC`: follow-up reset and record selection prefix

Classification: **static/dataflow prefix, shared flight follow-up**. Both
outcomes of the `$C265E8` parent decision call this helper.

Its entry writes `$01` to `$C45864`, `$04` to `$C4585B`, clears `$C458BD`,
`$C45ABA`, `$C45932`, and `$C459AA`, then reads a word from `$C4E98A +
$C459AA`. A positive word is published as `$C459B4`; the helper doubles its
`<<8` form into `$C459B6` and forms the corresponding `$C46184` 512-byte
record pointer on its positive-selector path. The zero-selector route instead
branches to `$C1CE38`; it is not valid to treat `$C1CD0E` as unconditional.

This identifies a common reset/select-record handoff following either flagged
slot outcome, without assigning the selected record a gameplay identity.

In the normal deterministic run060 call at frame 304, the initial word is
nonpositive and `$C1CCF2` takes `$C1CE38`. That route loads a descriptor from
`$C4F6CA + $C459AA`, publishes descriptor fields including `$C45932`, and
fans into `$C1D0B6` plus repeated renderer/projection helpers. The complete
call returns to `$C0F116` after 17,672 instructions.

Evidence: static bytes `$C1CCBC-$C1CD0D`, parent calls at `$C0F100/$C0F110`,
the existing byte-exact `$C1CD0E` follow-up source, and
`build/run060_c1ccbc_normal_full/trace.jsonl`.

`fa18_prepare_flight_followup_record` now represents this prefix directly.
It retains the caller-owned big-endian selector table and reports the positive
component-calculation versus nonpositive `$C1CE38` continuation separately.

The positive branch's next bounded `$C1CD0E-$C1CDB1` magnitude preparation is
now represented by `fa18_calculate_flight_followup_magnitudes`. Its `$29`
error-helper call remains a required callback when the source maximum caps at
`$EF`; the following lookup-table shift phase remains separate.

That `$C1CDB2-$C1CDFB` lookup/shift phase is now represented by
`fa18_shift_flight_followup_components`. Its signed-byte table lookup remains
caller-owned; the following descriptor dispatch is still a separate boundary.
