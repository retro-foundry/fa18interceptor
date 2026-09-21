# `request_next_target` at `$C1B1A4` (Hunk 5 +`$654C`)

Classification: **behavioural**. The first run003 `T` press becomes raw Amiga
key `$14` and enters this slice from the key dispatcher. The `$C33186` child
was traced separately with no future input and returned to `$C1B1AA` in 1,942
instructions; its P-code is `pcode/raw/run003_t_command_child/`.

After that return, the handler sets `$C4599A` bit 7, writes one to `$C457B9`,
and queues the raw input at `$C1C23C`. The input name comes from GAME.md and
the direct command-state effects come from this trace.
