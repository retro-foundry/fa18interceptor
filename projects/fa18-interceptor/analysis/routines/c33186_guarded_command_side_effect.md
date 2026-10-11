# `$C33186` guarded command-side-effect routes

Classification: **bounded structural command-side-effect wrapper**.

`source_amiga/observed/route_guarded_command_side_effect.asm` preserves the
complete `$C3316A-$C331CC` route cluster.  The named `$C33186` entry first
tests `$C45785` and `$C457D7`, chooses literal selector pairs, tests
`$C4588A`, then saves the complete caller register set and invokes `$C17EF2`.
It restores that set and returns at `$C331CC`.

The pre-existing raw-key packet observes `$C33186 -> $C1BBC0` returning in
1,924 instructions; see `analysis/routines/c33186_return_inner_packet.md`.
run029's normal full-frame profile additionally observes the guarded setup,
external-call, restore, and return instructions on its qualification-flight
path.

The selector literals, guard bytes, `$C17EF2` API identity, and resulting game
effect are not assigned.  In particular, this source does not claim that every
caller is a player command or that the route is specific to qualification.
