# `$C15BF8`: allocate the `$C1AB74` pointer-selected state block

Classification: **static allocation/dataflow plus snapshot-backed pointer
state**. This entry has not been directly executed by an available P-code
export, so it is not a scenario or gameplay-transition claim.

## Static contract

The runtime Slow-RAM authority image decodes `$C15BF8-$C15C54` as a bounded
entry with a local frame. At `$C15C1E-$C15C4E`, it pushes these two longword
arguments and calls `$C53B30`:

```text
$0000004E
$00010001
```

`$C53B30` preserves `A6`, loads the library base from `$C07F64`, forwards its
two stack longwords in `D0/D1` to vector `-$C6(A6)`, restores `A6`, and
returns. The caller then executes:

```text
$C15C4E  MOVE.L D0,$C1AB74
```

The vector shape and argument order match an Exec-style allocation wrapper:
`D0` is a 78-byte size and `$00010001` is passed as the allocation requirement
word. The routine's broader initialization role is deliberately unassigned.

## Snapshot join

At the controlled run029 key-6 `$C1017E` entry, the live longword at
`$C1AB74` is `$00C06A98`. The 48 bytes `$C06A88-$C06AB7`, including every
byte used by `$C1017E` at pointer offset `$12 + 3..8`, are zero. This joins
the static allocation/store chain to the observed conditional-state block
without implying that these bytes encode qualification, mission completion,
or any persistent pilot record.

The later `$C1017E` conditional-byte probe demonstrates the consumer side;
see `analysis/routines/c1017e_build_selectable_missions_queue.md`.

## Static offset inventory

The allocation size `$4E` is independently consistent with the largest
observed pointer-relative access (`+$46`, a word). The following is an offset
inventory, not a field-name claim:

| Offset | Width | Static access evidence |
|---|---:|---|
| `+$02` | word | `$C08EB8` reads it into `$C458A7/$C458A8`; `$C08ED0` writes the sign-extended `$C458A7` byte back. |
| `+$04` | word | `$C0FE68`, `$C115F8`, and `$C116DC` compare it; `$C1172C` increments and stores it. |
| `+$06` | byte | `$C11374` stores `$C458A6`. |
| `+$07` | byte | `$C11386` stores a byte selected from `+$12 + D0`. |
| `+$10` | word | `$C10DEE` increments and stores it. |
| `+$12..+$1A` | bytes | `$C101B4` tests `+$12 + index` for indices 3 through 8. |
| `+$36` | word | `$C107F6` increments and stores it. |
| `+$38` | word | `$C11390` increments and stores it. |
| `+$3A` | word | `$C15294` stores a computed `D1`. |
| `+$46` | word | `$C14E54` increments and stores it. |

Calls around `$C16342`, `$C163BA`, and `$C164B6` also pass the pointer and
size `$4E` to external wrappers, but their copy/transform semantics are still
unassigned. This table only establishes a bounded state-block layout and
the links needed to target future writer/consumer traces.
