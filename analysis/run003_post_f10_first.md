# Run003 state after first F10

`build/run003_post_f10_first/state.bin` is derived from the sealed run003
state at frame 3,887 by replaying only its first native F10 tuple:

```text
F 3888 K 291 0 16 1
F 3893 K 291 0 16 0
```

The resulting serialized state is at frame 3,899. The bounded trace proves
the tuple enters `$C1BD04` as raw `$59` and computes the level-ten byte `$79`
at `$C45870`. The active-gate route also leaves `$0050` at `$C45778` and
`$C4577C`.

This is the base state for subsequent scripted function-key experiments. Its
configuration is `captures/run003/config.uae`; substituting a different core
configuration would invalidate the state.
