# `$C31392` postflight renderer prefix

Classification: **partially runtime-observed structural/dataflow**. The
run024 frame-23000 continuation executes the common prefix through its
observed table path; unselected branches remain unobserved.

`source_amiga/observed/run_postflight_renderer_prefix.asm` is byte-exact for
`$C31392-$C3141D` (140 bytes). It gates on `$C45838`, selects one of two
40-byte table regions at `$C4E71C`, processes up to 11 word pairs through the
shared renderer entries, resets/increments three state bytes, and loads three
longs from `$C4E2BC` for a zero guard into `$C31722`.

The full contiguous routine through its return at `$C318F5` is now covered by
the following byte-exact static-only slices:

- `$C3141E-$C3149B`: fixed-point normalization and record rejection.
- `$C3149C-$C31517`: record-attribute guards.
- `$C31518-$C315BF`: record-status classification.
- `$C315C0-$C31611`: second normalization pass.
- `$C31612-$C316BF`: renderer selector classification.
- `$C316C0-$C31721`: submission and loop-back.
- `$C31722-$C3180B`: status resolution.
- `$C3180C-$C318F5`: terminal record scan and return.

The table's content and the routine's gameplay role remain unproven. The
individual slices retain structural/dataflow naming until a capture executes
this postflight path.
