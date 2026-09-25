# `$C118A0`: postflight failure-message callback

Classification: **scenario-backed result-message producer**.

The byte-exact callback
[`queue_postflight_failure_message`](../../source_amiga/observed/queue_postflight_failure_message.asm)
waits for signed `$C45AD6` to become negative. It calls `$C11ACC` with
`$C08490`, then tests `$C45849`:

```text
$10  -> queue selector $0062 at $C4574A
other -> queue selector $0063 at $C4574A
```

It clears `$C4582B` and installs `$C118E6` in callback slot `$C1820C`.

From the sealed run062 GUI-frame-2,200 checkpoint, a no-input sampler finds
the first queue mutation at core frame 25. An instruction walk from that
frame's `$C0EFD4` parent update reaches `$C118CC` after 342 instructions;
its `MOVE.W #$0063,$C4574A` changes the queue prefix from `$00000000` to
`$00630000`. The retained machine-readable result is
`build/run062_queue_writer_transition.json`.

This establishes the run062 failure-message producer and its selector branch.
It does not establish what `$C45849` represents, whether `$0062/$0063` are
distinct failure outcomes, or the earlier predicate that reaches this callback.
