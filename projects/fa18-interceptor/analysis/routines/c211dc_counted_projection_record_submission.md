# `$C211DC`: counted consecutive projection-record submission

Classification: **runtime-backed projection-record submission**.

`source_amiga/observed/submit_counted_consecutive_projection_records.asm` is
byte exact for `$C211DC-$C21229`.

## Contract

The entry consumes a packed word from `A2`: its low six bits are published to
`$C45954`, while its upper byte is the number of following six-word records.
The next `A2` word is an offset from `$C48390`. For each contiguous six-word
record from that base, the routine copies the six words to `$C4C592`, calls
`$C2EE4A`, and OR-accumulates its result. It returns that accumulated word.

This is a direct record-sequence producer for the existing projection path;
it does not establish coordinate names, object ownership, or the meaning of
the selector bits.

## Golden Gate runtime anchor

The no-input invocation from the sealed run031 frame-12,000 Golden Gate
checkpoint enters `$C211DC` through `$C1F942`, then calls `$C2EE4A` thirteen
times before later dispatching other renderer work. The active stream is
`$C355A0`. This makes the helper a concrete multi-segment geometry path in the
Golden Gate frame, without asserting that every submitted record is a bridge
edge.
