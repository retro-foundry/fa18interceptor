# `$C16EAE` external event/code stage

Classification: **structural update stage**.

The complete `$C16EAE-$C16F1B` range is called first by the bounded
`$C0F3C4` pending-key phase. It passes `$C1ABEC` through the known `$C53C08`
external-vector wrapper, tests the returned longword, and clears `$C4582D`
when it is zero. On a nonzero return it reads a word at `+$06` through the
pointer held at `$C1ABCA`, dispatches exact values `$0068` and `$00E8` to
`$C0833E` and `$C08394`, then clears that word.

Both paths pass `$C1ABCE` through `$C53C8C` and call `$C16F1C` before return.
The trace establishes the control/data flow and consumed-word clear; it does
not identify the external object, event protocol, the two code meanings, or
the semantics of the called dispatchers.
