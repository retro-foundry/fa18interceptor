# `$C1075A`: current post-input callback

## Evidence

The bounded `build/training_600_c0f5f8/trace.jsonl` packet enters `$C1075A`
from the `$C0F7D2` indirect callback call. It executes:

```text
$C1075A  tst.b $C458AC
$C10760  beq $C10788
$C10788  rts
```

and returns to `$C0F808`. Byte-exact static source is
`source_amiga/observed/run_current_post_input_callback.asm`.

## Observed contract

With `$C458AC == 0`, this callback has no state write and returns. Its static
nonzero path optionally sets `$C458AD` to two, calls `$C11312`, reloads
`$C45AD6` with three, and changes `$C1820C` to `$C1078A`. `$C11312` is now
byte-exact reconstructed and clears the head/cursor/control state of the
observed message-selector sequence; see
`analysis/routines/c11312_initialize_message_sequence_state.md`. That
nonzero callback path is not yet dynamically observed.

The run024 qualification prefix independently reaches this zero-return path at
frame 790 immediately after the crack-credit clear transition.  It executes
only `$C1075A` and its `beq $C10788` guard, then returns with no callback or
selector write.  This bounds the observed text/callback chain; it does not
explain the subsequent gameplay/update-loop ownership.
