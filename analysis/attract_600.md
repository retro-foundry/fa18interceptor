# Attract-mode display packet: hardware frames 601-602

Authority: `build/attract_focus_600/`, replayed from the sealed
`captures/attract_run001` state and recording. The preceding frame-600 image is
an in-flight demonstration screen.

The two-frame bounded trace contains 12,902 executed instructions, 1,527 unique
RAM instruction starts (4,546 bytes), 26 structural RAM call targets and 9,877
raw Ghidra P-code operations. Its P-code is in
`build/attract_focus_600/pcode/`; the Ghidra project is `Fa18Attract600`.

There are 739 custom-register writes: 651 CPU-originated and 88 Copper-originated.
The most frequent CPU offsets are `$040`, `$042`, `$052`, `$054`, `$056`, `$058`
and `$074` (54 each), followed by `$062`, `$048`, `$04A` and `$074`-adjacent
blitter data/pointer registers. These offsets are the documented blitter control,
pointer, modulo, data and start registers. This is direct evidence of active
blitter setup during these two display frames; it does not yet identify a game
routine or prove which visual element each blit produces.

The recurring CPU write-source addresses include `$C2FB72`, `$C2FBD4`,
`$C2FBD8`, `$C2FC3C`, `$C2FC40`, `$C2FCA4`, `$C2FCA8`, `$C2FD0A` and
`$C2FD0E`. In Engine9000 these source PCs are recorded at the custom write, so
they are breakpoint candidates, not function names. Next packet: break around
one source, correlate one blit transaction with its observed RAM instructions,
destination pointer and frame pixels.
