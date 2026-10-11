# Kickstart 1.3 graphics.library OwnBlitter and DisownBlitter

The observed RAM vectors `$C0272E` and `$C02728` jump to `$FC64BC` and
`$FC64D4`. With graphics.library base at `$C028F6`, these are the
`-$1C8(A6)` and `-$1CE(A6)` vectors, named `OwnBlitter()` and
`DisownBlitter()` by the local Kickstart 1.3 `LVO.OFFS`. Their observed
return sites are game code at `$C53FBC` and `$C53FCC`.

The pinned ROM bodies are 24 and 104 bytes. Both adjust the word-sized
ownership counter at graphics.library base `+$AA`, with the original flags
and wraparound. `OwnBlitter` can return at once or save D0-D1/A0-A1 around
its `$FCF324` helper call. `DisownBlitter` has an early return when the
counter becomes negative; otherwise it calls `$FD3BC4`, tests owner state,
reads DMACONR's blitter busy bit, and takes the source's interrupt/DMA and
helper-call branches through `$FC653A`. The helper calls at `$FD3BC4`,
`$FCF384`, and `$FD3BD4` remain on the ordinary ROM path.

`port/os/graphics.c` contains the counter operations. The bridge in
`port/os/graphics_blitter_ownership.c` performs the surrounding source
instructions at their original boundaries, including MOVEM stack order,
bus accesses, flags, branch cycles, and custom-register writes. The runner
checks all 128 ROM bytes before enabling this pair. It is on by default in
translated mode; `--no-os-blitter-owner` selects ROM, and `--no-recomp`
defaults to ROM.

The original transition inventory counted 16,012 `OwnBlitter` and 16,010
`DisownBlitter` entries across the three sealed recordings. In a 300-frame
demo C-versus-ROM comparison, RAM and runner statistics matched. The ROM
path entered the two leaves 1,736 and 1,735 times; the C path entered
neither. Complete C-path runs of demo01, qual_carrier_success, and
qual_fail_crashes retained their sealed final RAM hashes and had zero
entries to either original leaf. GNU and MSVC Release produced identical
RAM and transition inventories in the focused 300-frame C-path run. With
both C leaves default-on, the full gate passed: 386 routines, 726,979
matching shadow calls and 925,873 matching sandbox calls across three
recordings, zero mismatches, and identical poison frames. Run075 parity
frames 393-402 remained 10/10 exact.
