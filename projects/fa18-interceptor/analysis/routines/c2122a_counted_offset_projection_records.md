# `$C2122A`: counted offset-projection records

Classification: **runtime-backed generic projection-record submitter**.

`source_amiga/observed/submit_counted_offset_projection_records.asm` is byte
exact for `$C2122A-$C2129B`.

## Contract

The first `A2` word supplies a six-bit selector and a count in its upper byte.
The second selects a `$C48390` base triple; a further word advances to the
record list. Each six-word list record is adjusted by the selected base delta,
stored in `$C4C592`, and sent to `$C2EE4A`. The helper ORs all per-record
results and returns the accumulated word after restoring `A1`, `A2`, and `A5`.

## Runtime anchor

The run031 frame-12,000 Golden Gate control block invokes this helper from
selector `$8030` with payload `$C35780` (trace index 1256). It is a second
counted scene-record format alongside `$C211DC`; no geometry identity is
inferred from the control data alone.
