# `$C2F490`: renderer-pass bound initialization

Classification: **runtime-backed one-word initialization**. The entire helper
writes long `$000FFFFF` to `$C456E6` and returns.

`$C1518C` calls it once before its record sweep. The continuous forced-slot
run060 trace observes the write at the start of the true branch, before the
first selected `$C45C72`-stride record. This identifies `$C456E6` as a bound
or sentinel initialized for that renderer-associated pass, without assigning a
screen-space or gameplay meaning.

Evidence: byte-exact source `source_amiga/observed/initialize_renderer_state_long.asm`,
static bytes `$C2F490-$C2F49B`, and
`build/run060_c1518c_continuous_forced_route/trace.jsonl` rows 131-149.
