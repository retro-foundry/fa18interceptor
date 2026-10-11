# `$C24FE8` `$C457D7` nonzero guard

Classification: **structural**.

The run024 crash-result trace executes the direct edge
`$C17446 -> $C24FE8`. The byte-exact entry checks `$C457D7` and takes its
observed zero path to the adjacent `$C24FE6` `RTS`:

```asm
$C24FE6  RTS
$C24FE8  TST.B   $C457D7
$C24FEE  BEQ.S   $C24FE6
```

`source_amiga/observed/guard_nonzero_c457d7.asm` reconstructs these ten bytes,
including the shared return target. The trace proves execution of the entry and
the return path in this presentation-phase sample; it does not establish the
meaning, producer, consumer ownership, or lifetime of `$C457D7`.
