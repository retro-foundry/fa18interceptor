# `$C1AEE0` alternate-context key dispatch

Classification: **structural with byte-exact source**. This fragment reloads
the command-context byte `$C458AE`. A zero context selects a raw-key table;
context 6 selects the `$43` route; other nonzero contexts continue at
`$C1AF7C`. It ends at `$C1AF7B`.

The zero-context table routes Return and a set of movement/control raw keys.
It also maps raw `$9E`, `$AD`, `$AF`, `$BD`, and `$9D` to `$C1BB66`, the
observed fire-request queue tail. These are structural branch facts; the
individual source-key and gameplay bindings need isolated event traces.
