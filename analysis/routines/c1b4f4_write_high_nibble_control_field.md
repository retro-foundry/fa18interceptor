# `$C1B4F4` high-nibble control-field routes

Classification: **structural with byte-exact source**. Three queue entries
select `$20`, `$10`, or `$00`. The common helper latches the selection at
`$C4582E`; if either observed gate `$C457AD` or `$C457B4` is nonzero, it
clears bits 5–4 of `$C461E9` and merges the selection.

The route is a precise byte-level state update. Its control-name binding is
deferred until isolated raw-key traces are available.
