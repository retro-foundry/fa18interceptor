# `$C212B0`: offset-pair segment submission

Classification: **scenario-backed edge-list to projection dataflow**.

`source_amiga/observed/submit_offset_pair_segments.asm` is byte exact for
`$C212B0-$C2131B`, including the complete frame-602 no-input invocation.

After its selector word, `A2` supplies pairs of word offsets. Each offset
indexes `$C48390`; the routine copies three words from each selected record
into the two-record workspace `$C4C592`, rejects the pair if the bitwise AND
of their third words is negative, then calls `$C2EE4A`. The first word's sign
terminates the list; its partner remains usable after masking with `$7FFF`.

The bounded packet performs ten accepted pair submissions and returns the OR
of their projection/line-submission statuses. Combined with the exact
`$C2F03A` projection tail, this proves `$C48390` is a source of per-endpoint
triples for projected line segments. It does not yet prove table ownership
(model versus transformed instance) or identify a particular aircraft/terrain
model.
