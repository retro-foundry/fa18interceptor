# `$C223A8-$C227EB`: relocation-backed scene pointer triplets

Classification: **inline data inside original CODE segment 16**. The enclosing
Hunk remains CODE; only this bounded table range is separated.

## Evidence

Original segment 16 is placed at `$C22048` and is runtime-mutated elsewhere.
Its original payload offsets `$360-$7A3` form a regular sequence of records
with three consecutive 32-bit relocation sites. Within each triplet the three
pre-relocation values are identical, and the target/offset pairs name segments
41-50. Examples:

| Segment-16 offsets | Relocated target | Original target offset |
| --- | --- | --- |
| `$360,$364,$368` | segment 45 | `$94` |
| `$3C4,$3C8,$3CC` | segment 42 | `$0` |
| `$400,$404,$408` | segment 44 | `$0` |
| `$5A4,$5A8,$5AC` | segment 49 | `$4C6` |
| `$748,$74C,$750` | segment 48 | `$0` |

The repeated relocation layout is positive data evidence independent of the
unknown semantic role of the referenced records. It cannot be 68000 code at
these fields: each longword is a linker relocation operand and every triplet
contains the same resulting pointer.

## Bound and exclusions

The table begins at `$C22048+$360 = $C223A8` and ends at
`$C22048+$7A3 = $C227EB`, covering 1,092 bytes. The range includes the
record-local non-pointer fields between triplets. No claim is made about the
adjacent bytes, segment 16's remaining executable content, or the semantic
meaning of the pointer targets.
