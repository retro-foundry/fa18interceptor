# Outer update-loop slice at `$C15D80`

Classification: **structural caller loop**. It is the first static loop
boundary above the byte-exact `$C0EFD4-$C0F3C3` parent update routine.

`$C15DA2` calls `$C0EFD4`; after `$C53FC0` and `$C1612C`, `$C15DB2` branches
back to `$C15D96`. The initial `$C0E78A` call at `$C15D8E` therefore occurs
before that back-edge and is not repeated by this local loop slice.

`source_amiga/observed/run_outer_update_loop.asm` is byte-exact for
`$C15D80-$C15DB3` (52 bytes). Helper and timing roles remain unassigned; the
back-edge is structural evidence, not proof of a complete main-loop function.

The final pre-back-edge child is dynamically bounded at `$C1612C -> $C15DB2`;
see [`c1612c_outer_loop_child.md`](c1612c_outer_loop_child.md).
