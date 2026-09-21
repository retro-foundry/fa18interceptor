# `$C31392` postflight renderer prefix

Classification: **static-only structural/dataflow**. This common target is
not reached by current P-code exports.

`source_amiga/observed/run_postflight_renderer_prefix.asm` is byte-exact for
`$C31392-$C3141D` (140 bytes). It gates on `$C45838`, selects one of two
40-byte table regions at `$C4E71C`, processes up to 11 word pairs through the
shared renderer entries, resets/increments three state bytes, and loads three
longs from `$C4E2BC` for a zero guard into `$C31722`.

The table's content and the later state-machine role remain unproven. This
prefix is deliberately separated before the larger branch-heavy body.
