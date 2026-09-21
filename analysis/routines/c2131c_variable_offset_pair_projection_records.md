# `$C2131C`: variable offset-pair projection records

Classification: **runtime-backed generic projection-record submitter**.

`source_amiga/observed/submit_variable_offset_pair_projection_records.asm` is
byte exact for `$C2131C-$C2139D`.

## Contract

The first two `A2` words select a six-bit table selector and a base triple.
The remaining stream comprises endpoint-offset pairs. The helper reads pairs
until the high bit of the next pair's first word announces that the current
pair is final; its second offset still has bit 15 cleared and is submitted.
Each endpoint is resolved through `$C48390`, adjusted by the base triple
delta, and stored as two triples at `$C4C592` before `$C2EE4A` is called.

It accumulates all projection results and preserves `A1` and `A5`.

## Runtime anchor

The run031 frame-12,000 Golden Gate control block invokes this entry from
selector `$8038` at payload `$C35788` (trace index 1681). The data proves an
active endpoint-pair packet but does not identify individual visible bridge
edges.
