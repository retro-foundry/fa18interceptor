# `$C2FB7A` selected-plane line submission

Classification: **byte-exact hardware-facing source**. This is the suffix of
the `$C2FA7E` line emitter. For each set bit 0--3 in `$C456E7`, it selects the
corresponding pointer from `$C456B6`, waits for `DMACONR` bit 6 to clear, and
programs the prepared control/error/size values into the OCS blitter before
writing `BLTSIZE`.

The complete `$C2FB7A-$C2FD21` range (424 bytes) is
`source_amiga/observed/submit_blitter_line_to_active_planes.asm`. The pointer
table selected through `$C456B6` is not necessarily the Copper-visible table;
the per-plane hardware contract is directly established by the trace.

## Normal run029 buffer evidence

`build/run029_normal_c2fb7a_990/` traces one ordinary-replay invocation at
chipset frame 990.  It enters from `$C312D4` and returns to `$C312DA` after
100 instructions, with no later input events in the derived replay prefix.
All four plane-mask bits are set.  The four `BLTSIZE` submissions use the
following same-C/same-D pointers:

| Trigger | Pointer |
| --- | --- |
| `$C2FBE6` | `$0142C9` |
| `$C2FC4E` | `$016209` |
| `$C2FCB6` | `$018149` |
| `$C2FD1C` | `$01A089` |

These belong to the older four-plane working family, not the run029
Copper-visible five-plane ranges beginning `$04DB30`.  Therefore a normal
renderer line submission can prepare buffered content ahead of the visible
display; it cannot alone attribute a changed visible cockpit glyph.
