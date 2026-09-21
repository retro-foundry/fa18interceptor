# Control-record input stage at `$C1B27E`

Classification: **behavioural packet**. In the sealed run003 frame-6,000
indexed-record path, `$C25C70` calls `$C1B27E` and it returns to `$C25C76`
after 65 instructions. Future playback is empty. Canonical P-code is
`pcode/raw/run003_6000_c1b27e/` (65 instruction starts, 289 operations, two
observed call targets).

The route takes the `$C4584B <= 0` branch, reads byte fields from the caller's
current control record in `A1`, and reaches local helper `$C1B4D0`. That helper
replaces the low two bits of `$C461E9` with one. The packet also calls
`$C25A6A`, which immediately returns in this state. The broader control-record
field meanings remain unassigned.
