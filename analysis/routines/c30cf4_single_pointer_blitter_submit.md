# `$C30CF4-$C30D21`: single-pointer blitter submission

Classification: **runtime-backed dataflow/control flow**.

The sealed run029 trace for chipset frame 993 reaches this leaf three times.
Each entry follows an external blitter-idle wait and writes `BLTSIZE` at
`$C30D1C`; the observed trigger sizes are `$0202`, `$01C3`, and `$01C2`.

`source_amiga/observed/submit_single_pointer_blitter_job.asm` is byte-exact
for 46 bytes.  It programs `BLTCON0` from `D2`, clears `BLTCON1`, sets
`BLTADAT` to `$FFFF`, takes the first/last A masks from `D0`/`D3`, applies
`D5` as both C and D modulo, and writes the same prepared `D4` longword to
both `BLTCPT` and `BLTDPT`.  `D6` triggers the job through `BLTSIZE`.

The caller in this run prepares destinations in a non-Copper-visible working
plane family.  This leaf is renderer plumbing only: neither the input record
nor its pixels are assigned a cockpit-number meaning.
