# Indirect record dispatch at `$C1F942`

Classification: **structural**. This is an observed table dispatch within a
larger record-walking routine; it has no inferred game-object or subsystem
ownership.

## Dispatch contract

The code at `$C1F910-$C1F944` takes a signed word from `(A2)+`. Negative words
are control values; non-negative words have bit `$4000` counted in local
`-$6A(A6)`, then are masked to `$3FFF` and used as a byte offset into the
longword table at `$C1FCE8`:

```asm
        andi.w  #$3fff,d0
        lea     $c1fce8,a0
        movea.l (a0,d0.w),a0
        jsr     (a0)
```

The callee returns to `$C1F944`. A negative `D0` from that call branches back
to the record-walker control point `$C1F7FA`; otherwise it ORs `D0` into the
local word at `-$7C(A6)` and continues.

## Attract evidence

The frame-601/602 no-input trace observes these selectors and targets:

| Selector | Target | Observed calls |
| --- | --- | ---: |
| `$000C` | `$C2005C` | 16 |
| `$0084` | `$C207FE` | 1 |
| `$0034` | `$C212B0` | 2 |

`$C212B0` is the separately bounded line-submission target. The table has
further static entries but this document makes no claim about unobserved
selectors or their semantics.

The only observed `$0084` target path is four instructions: it tests byte
`$C45785`, follows its nonzero branch, and returns zero in `D0`. Its raw P-code
packet is `pcode/raw/no_key_c207fe_dispatch_target/` (four starts, 14
operations), fully mapped to verified Hunk 13. The byte's broader meaning is
unknown.

## Verified source entry

`source_amiga/observed/record_table_dispatch_entry.asm` is a byte-exact
62-byte reconstruction of `$C1F910-$C1F94D`. It includes the negative control
path, `$FFFF` error report, bit-`$4000` local count, `$3FFF` table mask,
indirect call, and status accumulation. Branches to the enclosing record
walker remain named external edges.
