# run060 root motion delta terms

Classification: **byte-exact flight-update dataflow**. This identifies one
root-record term as the signed vertical displacement used by the active pose
integrator; it does not call the terms velocities.

`$C14B16-$C14BB0` prepares three scaled local longwords and stores their
negations in the selected record:

```text
local -$10(A6) -> negated -> root +$3E
local -$14(A6) -> negated -> root +$42
local -$18(A6) -> negated -> root +$46
```

The same `$C13D84` continuation later performs the committed vertical pose
update:

```text
root +$18 := root +$18 - local -$14(A6)
```

Therefore, on that update route:

```text
root +$18 := root +$18 + long[root + $42]
```

The sealed frame-925 descent trace proves the `+$18` subtraction is committed
in active run060 qualification flight. Thus root `+$42` is the signed
per-update vertical displacement term for that route. `+$3E` and `+$46` are
the corresponding pair of signed scaled displacement terms, but their exact
mapping to lateral/forward root coordinates is not yet proven.

Authority: `source_amiga/observed/prepare_c13d84_scaled_record_deltas.asm`,
`source_amiga/observed/store_negated_scaled_record_deltas.asm`,
`source_amiga/observed/update_c13d84_record_offset18.asm`, and
`build/run060_frame00925_root_descent_writer/memory_writes.json`.
