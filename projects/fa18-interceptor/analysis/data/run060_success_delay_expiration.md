# Run060 success queue: delay expiration transition

Classification: **scenario-backed scheduler transition**.

From the sealed native GUI-frame-9,284 checkpoint, an instruction walk begins
at the ordinary parent update `$C0EFD4` and watches `$C45AD6`. At instruction
171, `$C0F7FA` executes the byte-exact post-input tick store, changing the
word from `$0000` to `$FFFF`.

The source sequence is `MOVE.W $C45AD6,D0; SUBQ.W #1,D0; MOVE.W D0,$C45AD6`.
The observed writer PC and values are retained in
`build/run060_success_delay_zero_to_negative.json`. The same parent-update
walk reaches `$C11186` 53 instructions later, where selector `$004A` is
written to `$C4574A`; see
[the queue-writer fixture](run060_success_queue_writer_transition.md).

This proves that the successful result queue is launched by an expired
scheduler word. `$C458A6=9` was already stable before this interval and is
the established menu/mission context mode, not evidence of the success
predicate. The earlier writer that selected this callback/timing state and
the success-versus-failure decision remain open.
