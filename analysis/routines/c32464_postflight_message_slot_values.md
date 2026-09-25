# `$C32464-$C3250F`: postflight slot value preparation

Classification: **static dataflow in a runtime-observed postflight parent path**.

`source_amiga/observed/format_postflight_message_slot_values.asm` is byte exact
for `$C32464-$C3250F`. It selects one of three slots from `D0`, copies the
literal labels `ALT:`, `HDG:`, or `SPD:` to `$C45817`, prepares a value from
the `A4` record, and calls the shared packed-decimal formatter at `$C3267A`.

The three proved value paths are:

- `ALT:`: `((long +$18) >> 10) * 5`, five digits into `$C45821`.
- `HDG:`: signed word `+$68`, arithmetic right shift by three, unsigned divide
  by 10, three digits into `$C45820` with the nonblank formatting flag set.
- `SPD:`: magnitude of word `+$6E` unless record bit 7 is set, unsigned divide
  by 12, four digits into `$C45820`.

The code proves these literal-to-arithmetic associations. It does not yet
prove the record's object identity, the unit/scaling convention beyond the
literal labels, or a qualification outcome.
