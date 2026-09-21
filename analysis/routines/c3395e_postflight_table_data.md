# `$C3395E` postflight table data

Classification: **static structural/dataflow**. This is data, not code.

`$C3395E–$C33AD5` contains 94 big-endian long values (376 bytes). The initial
values form an ordered stepped series beginning `$00005000`, `$00009000`,
`$00006000`, `$0000A000`; the series continues through the values addressed
from `$C33A16` by the nearby postflight code.

The reconstruction at `$C337FC`, `$C338AA`, and `$C3391A` proves address
formation of `$C33A16 + d1` before calls to `$C33F54`. This establishes the
region as helper input/table data, but neither element semantics nor the
consumer contract is yet known. `$C33AD6` is the next code entry.
