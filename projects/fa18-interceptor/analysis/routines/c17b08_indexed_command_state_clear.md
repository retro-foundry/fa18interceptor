# `$C17B08`: indexed command-state clear wrapper

Static, byte-exact reconstruction: `source_amiga/observed/clear_indexed_command_state.asm`.

It clears `C4FE38[index * 4]`, then invokes `$C4FFB0` with the same index. It
is reached from the static `$C17B2C` indexed command-state path; the ownership
of the table and callee remain unassigned.
