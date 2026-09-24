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
not the start of the `$C3B4FE` byte sequence. The field can therefore denote a
control-record entry whose later interpretation reaches the adjacent family,
but no live `$C1F6F8` entry for this target was reached in the sampled run003
or run037 map windows. Its exact parser role and any individual 3D component
association remain unresolved.

Authority: `captures/baseline_menu/slow.bin` at slow-RAM offset `$3B4F8`,
`analysis/data/terrain_lattice_target_catalog.json`, and the run003/run037
live control-stream inventories.
