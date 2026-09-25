# `$C328DA-$C329D9`: activity record status formatter

Classification: **static literal/formatter dataflow**.

`source_amiga/observed/format_activity_record_status.asm` is byte exact for
`$C328DA-$C329D9`. It branches on the high-nibble selector prepared by the
`$C328A8` prefix. The nonzero paths write literal `SW`, `AM`, or `GUN` labels
into `$C457FA`, prepare the selected byte/word value, and call `$C32794`.
After that call returns, a positive input word selects literal `ARM`; otherwise
it selects `NO`, then performs a second `$C32794` submission.

This establishes label bytes, arithmetic, and renderer calls. It does not
prove whether the record represents a weapon, target, radio mode, or any other
gameplay subsystem; the literal `ARM` must not be promoted into such a claim
without a scenario trace.
