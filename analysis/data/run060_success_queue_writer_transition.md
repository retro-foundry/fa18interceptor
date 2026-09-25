# Run060 success queue: exact selector-store transition

Classification: **scenario-backed message-queue writer**.

Authority is the sealed native GUI-frame-9,284 checkpoint
`build/run060_frame09284_checkpoint/frame_09284_state.bin`, restored with its
no-input playback. The bounded instruction walk starts at the ordinary parent
update `$C0EFD4` and checks the first four bytes of `$C4574A` after every
instruction.

At instruction 224, `$C11186` executes `MOVE.W #$004A,(A0)` with
`A0=$C4574A`, changing the checked queue prefix from `$00000000` to
`$004A0000`. The immediate successor is `$C1118A`; the captured opcode bytes
also show its following `$C1118C` `MOVE.W #$8053,(A0)` store.

The retained machine-readable result is
`build/run060_success_queue_writer_transition_v2.json`. It joins the native
checkpoint delta to the byte-exact mode-9 branch in
[`prepare_postflight_message_sequence`](../../source_amiga/observed/prepare_postflight_message_sequence.asm)
and the selector-74 payload proof in
[`run060_success_text_payload_trace.md`](../run060_success_text_payload_trace.md).

This proves the queue writer and its first selector store in the successful
qualification scenario. It does not identify the earlier logic that made
`$C45AD6` negative or supplied postflight mode `$C458A6=9`.
