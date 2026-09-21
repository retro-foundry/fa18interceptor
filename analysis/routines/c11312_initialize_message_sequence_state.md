# `$C11312`: message-sequence state initializer

Classification: **static dataflow**, with an observed caller relationship.
The byte-exact source is
`source_amiga/observed/initialize_message_sequence_state.asm`.

## Contract

`$C11312` clears the first two words at `$C4574A` and `$C4574C`, sets
`$C4573E` to `$1B8`, and clears these bytes:

```text
$C457C6  selector-sequence byte cursor
$C457C3  selector/compositor active flag
$C457E0  selector/compositor effect counter
$C45871  selector/compositor inhibit flag
```

It deliberately does not clear the complete `$C4574A` sequence region.  The
routine therefore initializes its head and control fields but is not, by itself,
evidence for the later sequence writer.

## Caller evidence

The byte-exact `$C1075A` callback tests `$C458AC`; its nonzero static path
calls `$C11312`, reloads `$C45AD6` with three, and replaces callback slot
`$C1820C` with `$C1078A`.  That nonzero callback path remains unobserved, so
this is not promoted to a scenario transition.

The flight-return trace independently proves the consumer side: `$C32BD2`
uses `$C457C6` as a byte cursor into selector words rooted at `$C4574A`, and
`$C32CEE` dispatches the selected word to `$C32D24`.
