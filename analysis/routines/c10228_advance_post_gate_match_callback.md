# `$C10228`: post-gate input-match callback

Classification: **scenario-backed callback transition**.  Byte-exact source:
`source_amiga/observed/advance_post_gate_match_callback.asm`.

It compares `$C458A0` and `$C458A1`; unequal values return without writes.
On equality it sets `$C45795=1`, `$C45858=$FF`, `$C458AE=4`, `$C458AD=1`,
leaves delay `$C45AD6=5`, and installs `$C10678` at `$C1820C`.

The run024 qualification prefix reaches this equality body at frame 713 with
`D1=15`, after the prior `$C101FC` stage initialized `$C458A1` to `$0F`.
This proves the callback handoff in that scenario.  The producer advancing
`$C458A0` and gameplay role of `$C10678` remain unassigned.
