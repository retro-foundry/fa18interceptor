# `toggle_arrestor_hook` at `$C1B616` (Hunk 5 +`$69FE`)

Classification: **behavioural**. The run003 `A` press becomes raw `$20` and
enters this handler. Its `$C33186` child returns to `$C1B630` in 1,924
instructions; a second no-future-input packet from that return completes the
post-helper tail through `$C0F45C` in 33 instructions.

With `$C461E6 = $11` in the recorded state, the path sets `$C4599A` bit 1,
toggles `$C46186` bit 15 and `$C45847` bit 7, writes `$C45845 = 3`, and calls
the bounded `$C25704` command-word publisher. Source is
`toggle_arrestor_hook.asm`; the publisher is separately reconstructed in
`publish_command_word_flags.asm`.
