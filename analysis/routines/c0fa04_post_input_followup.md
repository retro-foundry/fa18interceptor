# `$C0FA04`: timer-gated post-input callback

## Evidence

Static disassembly of `$C0FA04-$C0FA4B`, reconstructed byte-for-byte in
`source_amiga/observed/finish_post_input_followup.asm`. `$C0F992` installs this
callback when its `$C4584B == 3` route is selected.

On a negative `$C45AD6`, it calls `$C0FAA4`, writes the recorded state values,
and installs `$C0FA4C` as the next callback. While the countdown is
non-negative it calls `$C2FD22`. The runtime route and helper purposes await
bounded packets.
