# `$C2FB7A` active-bitplane line submission

Classification: **byte-exact hardware-facing source**. This is the suffix of
the `$C2FA7E` line emitter. For each set bit 0--3 in `$C456E7`, it selects the
corresponding pointer from `$C456B6`, waits for `DMACONR` bit 6 to clear, and
programs the prepared control/error/size values into the OCS blitter before
writing `BLTSIZE`.

The complete `$C2FB7A-$C2FD21` range (424 bytes) is
`source_amiga/observed/submit_blitter_line_to_active_planes.asm`. The pointer
owner and visible primitive remain unassigned; the per-plane hardware contract
is directly established by the custom-register trace.
