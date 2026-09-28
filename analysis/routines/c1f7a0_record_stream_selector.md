# `$C1F7A0-$C1F837`: negative control stream selector

Classification: **bounded control-stream selector**.

`record_stream_selector.{c,h}` ports the complete observed
`select_record_streams` slice. It keeps source addresses as addresses: the
caller maps its byte span at `stream_base`, while longword operands are read as
the source's direct pointers and returned without reinterpreting ownership.

For `$FFFF`, the adapter returns the source return-zero route. For bit `$2000`
controls, it publishes A2 directly, either from the following longword when
bit `$1000` is set or from the `$0FFF`-masked base offset. Other controls
publish A1, set the source local count for bit `$4000`, and otherwise consume
the first A1 word to select A2. Bit `$8000` of that word sets the A1 flag.
The bit-`$4000` case reports the separate `$C1F844` post-stream boundary;
record dispatch at `$C1F906` remains caller-owned.

The synthetic contract covers terminal, direct and base-relative A1/A2 forms,
the `$4000` post-stream route, and the `$8000` A1 flag. This adapter is not
scheduled by `game.c`.
