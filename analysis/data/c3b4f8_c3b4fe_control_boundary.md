# `$C3B4F8 -> $C3B4FE`: descriptor-target control boundary

Classification: **static boundary observation**. `$C3B4F8` occurs as a
descriptor `+8` target in the decoded flat-placement template catalog, while
the adjacent `$C3B4FE` family has separate renderer-observed polygon evidence.
This note prevents treating the target value as an independent coordinate or
model pointer.

The byte-stable baseline payload at `$C3B4F8-$C3B4FD` is:

```text
$C3B4F8  42 C0 40 86 C0 14
$C3B4FE  44 80 00 86 C0 14 ...
```

Thus `$C3B4F8` is a six-byte prefix immediately preceding `$C3B4FE`; it is
not the start of the `$C3B4FE` byte sequence. Run037 now proves that it is a
live descriptor `+8` value: `$C1CC70` reads it from `$C22708`. Before the next
`$C1F6F8` entry, however, eleven later descriptor writes replace the shared
`$C45A36` control value; the walker enters with `$C3B73E`, not `$C3B4F8`.
The bounded continuation executes neither `$C3B4F8` nor `$C3B4FE`. Its exact
parser role and any individual 3D component association therefore remain
unresolved; the store alone cannot be promoted to a mountain-instance link.
See the [store/overwrite audit](run037_c3b4f8_descriptor_store_overwrite.md).

The adjacent `$C3B4FE` family is independently associated with the five-triple
`$C3B588` green-mountain component, including four triangular side records.
That existing relationship does **not** promote `$C3B4F8` itself to a mountain
instance pointer: the missing evidence is the live transition from this
descriptor target into the `$C3B4FE` control stream.

Authority: `captures/baseline_menu/slow.bin` at slow-RAM offset `$3B4F8`,
`analysis/data/terrain_lattice_target_catalog.json`, and the run003/run037
live control-stream inventories plus
`build/run037_c3b4f8_descriptor_target_trace/trace.jsonl`; the separate face-family authority is
`analysis/data/c3b588_later_bridge_component.md`.
