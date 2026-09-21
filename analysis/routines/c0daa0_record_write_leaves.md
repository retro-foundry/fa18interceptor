# `$C0DAA0`, `$C0DAD0`, `$C0DAD4`, `$C0DADC`, `$C0DAE6`: record-write leaves

Authority: the return-bounded `$C2FEDE -> $C0D752 -> $C2FEF2` cockpit trace.
The trace reaches and returns from each of these five direct leaves while
`A1` is a caller-selected cursor in the `$C4B390` area.

`$C0DAA0` reads two indexed word-pairs from `$C4B990`, subtracts them from
the literal pair `$013F/$00B3`, and appends four words. The other leaves append
respectively: a zero longword; `$013F,0`; `$013F,$00B3`; and `$0000,$00B3`.
They are record-writing contracts only: neither the record type nor the visual
meaning of the literals is assigned.

Each source file in `source_amiga/observed/` is byte-exact and has one of the
five entry addresses above.
