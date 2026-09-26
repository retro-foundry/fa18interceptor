# `$C110A4`: postflight message-sequence prefix

Classification: **scenario-backed selector production**.

The byte-exact source
`source_amiga/observed/prepare_postflight_message_sequence.asm` covers
`$C110A4-$C111A9`.  On a negative `$C45AD6`, it clears `$C45795`, installs
`$C10DAE` in callback slot `$C1820C`, initializes the message sequence, then
branches by the signed byte at `$C458A6` and selector-state byte `$C45798`.

The run060 native success-activation boundary changes `$C1820C` from this
entry to `$C10DAE`, and changes `$C4574A-$C4574F` to
`$004A,$8053,$0000`.  The static mode-9 branch is exact:

```text
$C11172  write $0001 through pointer $C1AB74
$C1117C  call $C1643A
$C11182  write $004A at the sequence cursor
$C1118C  write $8053 at the next word
$C11192  write $01 to $C4582B
$C1119A  write $EF to $C45798
```

Thus, with the observed frame-9284 mode `$C458A6=9`, this is the producer of
the first success selector 74 and its immediately following `$8053` word.
The direct checkpoint-to-instruction transition at `$C11186` is retained in
[the run060 queue-writer fixture](../data/run060_success_queue_writer_transition.md):
it observes `A0=$C4574A` and the `$004A` store after 224 instructions from
the ordinary parent-update entry.

It proves the postflight mode-9 message transition, not the condition that
made mode 9 reachable, the semantic meaning of the external helper, or pilot
qualification persistence.

## `$C1AB74` offset-zero lifetime in run060

The pointer stored at `$C1AB74` resolves to `$00C06A98` in the adjacent
run060 checkpoints.  Its first word is `$0000` at frame 9,284 and `$0001` at
frame 9,285, exactly across the traced mode-9 branch.  This independently
matches the `MOVE.W #1,(A0)` at `$C11172` in the success-selector writer
trace.  By frame 9,400 that word is again `$0000`.

Consequently this is an observed transient state-block write associated with
the mode-9 message sequence.  The available checkpoints do **not** establish
which later instruction clears it, nor do they support treating it as a
persistent qualification/unlock record.  Its known later static consumer
(`$C1BD78`) only tests whether the word is zero while selecting a command
mode; it has not been observed consuming this success transition.
