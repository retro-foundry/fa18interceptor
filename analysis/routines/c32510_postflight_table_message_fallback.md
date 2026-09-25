# `$C32510-$C325A5`: postflight table-message fallback

Classification: **static dataflow in a runtime-observed postflight parent path**.

`source_amiga/observed/select_postflight_table_message.asm` is byte exact for
`$C32510-$C325A5`. For zero `$C459C4`, it detects a changed `$C45ADE` selector,
uses its low byte to select a 28-byte record from `$C3D0A0`, skips the first
byte, and copies the following 27 bytes to `$C4580A`. It remembers the full
selector at `$C45AE4` and sets `$C4583C` to two before joining `$C325A6`.

Nonzero slot values set `$C459C4` to either `-1` or zero, then copy the first
table payload instead. An unchanged selector only returns when its delay byte
is exhausted; otherwise it decrements the byte and falls through to the shared
submission setup.

This proves fixed-size table selection and delay semantics, not the meaning of
the selector, table-record header byte, or message/result category.
