# `$C30E06-$C30E43`: multi-pointer blitter submission prefix

Classification: **runtime-backed dataflow/control flow**.

Run029's chipset-frame-993 trace executes these 62 bytes once after an
external blitter-idle wait. The final `BLTSIZE` write at `$C30E40` uses
`$01C2`. `source_amiga/observed/submit_multi_pointer_blitter_job_prefix.asm`
is byte-exact for this straight-line prefix; the immediately following status
branch remains outside the slice.

The prefix sets `BLTCON0` from `D2`, clears `BLTCON1`, and sets both A masks to
`$FFFF`. It takes A and B pointers from `D0` and `D3`; C and D share `D4`.
It uses the pre-adjustment `D5` for A/B modulo, then decrements and offsets
`D5` by `A5` for C/D modulo, before using `D6` to trigger the job.

This is only a hardware preparation contract. Its alternate-plane target in
run029 is not treated as an identified cockpit glyph or formatter output.
