# `$C3267A-$C326B7`: postflight packed-value text formatter

Classification: **behavioral formatter contract**.

`source_amiga/observed/format_postflight_packed_value.asm` is byte exact for
`$C3267A-$C326B7`. It submits `D0` to the established `$C25A08` packed-BCD
converter, then emits `D2 + 1` low-nibble-first ASCII digit bytes backwards
from `A2`. Each nibble is converted by adding ASCII `'0'`.

When `D4` is zero, the resulting leading `'0'` bytes are changed to spaces;
nonzero `D4` retains zero digits. The postflight slot-value paths at `$C32464`
provide the three documented callers and their buffer/digit-count parameters.
