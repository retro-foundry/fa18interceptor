# Runtime ILBM object slots

Static evidence links each immutable ILBM path to a distinct BSS result slot.
These are decoded runtime-object pointers, not source file bytes and not code.

| Asset | Loader result slot | Direct consumer evidence | Derived BSS pointer cache |
| --- | --- | --- | --- |
| `pix/splsh` | `$C1AADC` | `$C1693A-$C1697E` copies object offsets `$08-$18` | `$C1AAF4-$C1AB04` (five pointers) |
| `pix/inst5` | `$C1AB08` | `$C1691C` passes object offset `$18` to `$C53FD0`; `$C1698A-$C169BA` copies offsets `$08-$14` | `$C1AB20-$C1AB2C` (four pointers) |
| `pix/frnt5` | `$C1AB38` | `$C168F4` passes object offset `$18` to `$C53FD0`; `$C16A68-$C16AA2` copies offsets `$08-$14` | `$C1AB50-$C1AB5C` (four pointers) |

The `inst5` and `frnt5` call sites also pass the BSS word pairs
`$C1AB0C/$C1AB0E` and `$C1AB3C/$C1AB3E`, respectively, before calling
`$C53FD0`. The external vector's semantic identity remains unassigned.

This establishes separate runtime ownership paths for the three loaded assets.
It does not establish their visible placement, timing, or the roles of every
pointer field.
