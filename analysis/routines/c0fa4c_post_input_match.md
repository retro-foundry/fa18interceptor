# `$C0FA4C`: post-input input-match callback

Static, byte-exact reconstruction: `source_amiga/observed/wait_for_post_input_match.asm`.

When `$C45AD6` is negative, this callback clears `$C45795`, compares
`$C458A0` and `$C458A1`, and on equality resets the countdown to two and
installs `$C0FA80` in `$C1820C`. Its runtime use remains unobserved.
