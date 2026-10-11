# `set_rudder_input_mask` at `$C1B58E` (Hunk 5 +`$5956`)

Classification: **behavioural**. The run003 comma press becomes raw `$38`,
reaches `$C1B58E`, and returns to the poll caller in 68 instructions. The
period press becomes raw `$39`, reaches `$C1B594`, and returns in 70
instructions. P-code is in `pcode/raw/run003_comma_key_dispatch/` and
`pcode/raw/run003_period_key_dispatch/`.

Comma selects `$80`; period selects `$40`. Both preserve the low six bits of
`$C461E9`, replace its high two input bits, and publish the selected mask at
`$C4582F`. The adjacent `$C1B59A` entry publishes zero, providing the static
release/reset form for the same fields.
