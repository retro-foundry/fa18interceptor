# `$C1AD74` keyboard command-dispatch entry and gates

Classification: **structural with byte-exact source**. The entry receives its
raw input word through the stack, advances a signed byte counter at `$C457D5`,
and conditionally clears bit 7 of `D0`. It then loads context bytes into
`D1`, `D3`, `D5`, and `D6` and selects one of the contiguous dispatch tables.

The gates inspect observed state at `$C457D3`, `$C45787`, `$C45791`,
`$C458A6`, `$C458AE`, `$C4584B`, and the low nibble of `$C46200`. Their field
names describe dispatch mechanics; the higher-level game state represented by
each byte remains unassigned unless a bounded trace proves it.
