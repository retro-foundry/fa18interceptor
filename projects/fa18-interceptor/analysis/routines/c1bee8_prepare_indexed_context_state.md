# `$C1BEE8` indexed context-state preparation

Classification: **structural with byte-exact source**. The route disables two
request latches, converts `D1` to a word index by shifting left nine bits,
invokes `$C08324`, and branches on `$C45785`. Its zero branch reads record
fields at `$68/$62` from `$C46184`; a special type nibble `$30` updates the
same output words used by the shared context publisher.

The index and output labels describe observed data movement only. The caller
and gameplay role require bounded traces.
