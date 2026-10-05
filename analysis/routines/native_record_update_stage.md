# Native record update parent

`port/native_record_update_stage.c/.h` implements complete
`$C1C63E-$C1C7F4` and the `$C1C7F6` record-rate child against the shared native
scene bank. Its authority is the complete readable reconstruction in
`port/game/update_stage.c` and the sealed proof documented in
`native_c_record_update_stage.md`.

The native owner synchronizes the input latch, negates the position bias,
detects the signed `$A000` threshold crossing and reproduces the unusual
`CLR.W/SWAP/ASR.L #5` key as a zero-extended high-word value. After the
record producer returns its request byte, the selected-record route resolves
the actual `flight.viewed` object in the sixteen-record bank. The active-origin
route consumes the producer's ordinary three-long output, derives both cell
keys and publishes changed selectors in source order. The final request byte
is ORed into the shared view update mask.

The older detached `update_stage_prefix` helper treats the scaled high word as
signed and is not used by this path. Record-rate inputs are read through the
live record field view, preserving word negation and arithmetic-shift behavior
without creating another packed persistent record.

`$C22C80` record production and `$C29042` active-origin production remain
explicit complete callbacks. This moves the native boundary below `$C1C63E`;
it does not establish those children. The focused contract covers both parent
routes, child mutation order, signed/high-word edges, rate classes and selector
publication. The historical bootstrap oracle still contracts all three old
child boundaries and remains sequencing evidence rather than a differential
proof of this new direct owner.
