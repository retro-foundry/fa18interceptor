# `$C1B0F2` nonzero-context dispatch tail

Classification: **structural with byte-exact source**. The fragment gates raw
`$31` on `$C457E0 != 2`, routes raw `$46` to `$C1C2B8`, and raw `$45` to the
signed-input-state bridge. Raw Return (`$44`) clears `$C457D3` before joining
the common command queue; all other keys join it without that clear.

The source is `finish_nonzero_context_key_dispatch.asm`. The state-field
labels describe observed storage roles, not an asserted gameplay meaning.
