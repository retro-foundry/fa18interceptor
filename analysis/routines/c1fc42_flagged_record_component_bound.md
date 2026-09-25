# `$C1FC42`: flag-controlled record-component bound predicate

Classification: **runtime-backed, memory-read-only record-walker predicate**.
The routine compares one shifted record component with a workspace-adjusted
bound, then conditionally inverts the result with control bit 12. Its precise
game-space axis and record type are not established.

## Inputs and calculation

`D0.w` is the signed offset into the record base loaded from `$C45A32`;
`D7.w & $0C00` selects the component and `D7` bit 12 selects sense. The
record's byte at base `+6`, masked with `$0F`, is the signed word right-shift
count. The selected paths in byte-exact
`source_amiga/observed/compare_flagged_record_component_bound.asm` are:

| Selector `(D7.w >> 10) & 3` | Record word at `base + D0.w +` | Added workspace term | Compared bound | Width |
| --- | ---: | --- | --- | --- |
| 0 or 3 | `$0E` | `wrap16($C45B2E.w << $C45AB8.w)` | `wrap16(-$C45A76.w)` | word |
| 1 | `$0C` | none | `wrap32(-$C45A78.l)` | long |
| 2 | `$0A` | `wrap16($C45B2A.w << $C45AB8.w)` | `wrap16(-$C45A72.w)` | word |

First arithmetic-shift the signed record word, then add the workspace term
with word wrapping where applicable. Selector 1 sign-extends that shifted
word to long. `CMP` sets a signed `below` result when the negated bound is
less than the adjusted record component. The shift counts and negation retain
68000 width semantics; neither is an unbounded mathematical operation.

## Boolean result and caller

The helper communicates its decision in the **Z flag**, not a normalized
`D7` boolean:

```text
Z = (below == ((input_D7 >> 12) & 1))
```

When `below` is false, `BTST #12,D1` supplies Z and `D7` still holds the
negated bound. When `below` is true and bit 12 is clear, it returns `D7=1`,
`Z=0`; when bit 12 is set, `CLR.W D7` returns `Z=1` while preserving `D7`'s
upper word. The caller at `$C1F780` uses `BEQ` to choose its next control-
stream offset. Treating the returned `D7` as a boolean would be incorrect.

## Evidence boundary

`pcode/raw/no_key_c1fc42_record_constraint/` has 18 observed instructions
and 156 P-code operations, with no memory stores. The frame-601 capture
selects path 1: shifted component `112`, bound `132`, `below=false`, bit
12 clear, and returned `Z=1`, matching
`build/no_key_c1fc42_record_constraint/trace.jsonl`. A separate run031
frame-12,000 trace in
`build/run031_frame12000_golden_gate_c1f6f8_probe/trace.jsonl` exercises
selector 2 twice: the adjusted record words are `$2600` and `$2638`, versus
bound `$27CE`; both return `Z=1`. Selectors 0 and 3, and the `below` return
arms, are established by byte-exact source but not by these traces. No
particular map object is assigned to this helper.
