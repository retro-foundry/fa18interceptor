# `$C34066` postflight four-step sweep prefix

Classification: **static structural/dataflow**. This range is not covered by
the current P-code exports.

It stores incoming `d1` in a stack frame, makes an initial submission from
`$C4598C`, then repeatedly advances `d0` by `$A` while it remains below both
`$13F` and `$B9 + $C45988`. Iterations 1 and 3 use an extra `$C2F5D4` call
and submit `d1 - 1`; all other iterations call `$C2F5F4`.

`$C340DC` starts the mirrored second sweep. Helper effects remain unproven.
