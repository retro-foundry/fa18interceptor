# `$C0F56A`: fixed-width hexadecimal ASCII formatter

## Evidence

Static 68000 disassembly of `$C0F56A-$C0F5F7`, reconstructed byte-for-byte in
`source_amiga/observed/format_fixed_width_hex_ascii.asm`. No execution claim
or caller ABI has yet been captured for this routine.

## Observed contract

The routine treats stack locations `8(a6)`, `12(a6)`, and `19(a6)` as an output
pointer, a 32-bit input value, and a digit count. It begins at the byte after
the requested output field and walks backward. Each pass converts the low
nibble to ASCII `0` through `9` or `A` through `F`, stores it, and shifts the
input right four bits.

It then scans the field from its first byte through all but the final digit,
replacing consecutive ASCII `0` bytes with spaces. The exact caller-side
argument layout and whether the routine is used for a display field remain
open.
