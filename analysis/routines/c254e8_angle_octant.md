# `classify_angle_octant` at `$C254E8` (Hunk 20 +`$814`)

Classification: **behavioural**. This routine divides one native angle word
into eight equal sectors and stores the sector index at `$C45854`.

When `$C45785` is non-zero it uses `$C45A96`; otherwise it uses the low word
of `$C45A8C`. It compares against `$0E10` increments through `$6270` and writes
one byte from `0` through `7` to `$C45854`. The full native turn represented by
these octants is `$7080`; no real-world angular unit is assumed.

The 102-byte reconstruction is
[`source_amiga/observed/classify_angle_octant.asm`](../../source_amiga/observed/classify_angle_octant.asm)
and is byte-exact. In the no-input attract packet, the selected angle takes
the `$0E10-$1C1F` range and the routine writes octant `1`, returning to
`$C0F03C` after 14 instructions. Its P-code at `pcode/raw/no_key_c254e8/`
contains 14 instructions / 54 operations, all mapped to Hunk 20.

`$C1C63E` begins by comparing `$C45854` with `$C45855`, which gives this field
an observed consumer inside the bounded update stage. The consumer's gameplay
meaning remains unknown.
