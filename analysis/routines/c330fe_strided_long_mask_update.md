# `$C330FE` byte-mask update across strided longwords

Classification: **dataflow-backed structural update packet**.

The exact `$C330FE-$C33168` packet is now reconstructed in
`source_amiga/observed/apply_byte_masks_to_strided_longs.asm`.  The sealed
run029 normal full-frame profiler samples every instruction in both of its loop
forms; in particular `$C3313E-$C3315A` is the hottest previously unrepresented
region in the profile.

## Established data flow

- Saves/restores `D0` and `D6`.
- Forms a packed shift/mode value from `D2` and the incoming `D3` word.
- Uses `D4` as a byte-stream address and `D1` as a longword-stream address.
- Derives the `DBRA` count from `D7 >> 6`, then advances the byte stream by
  one and the longword stream by `$28` for each iteration.
- In the zero high-nibble route, it clears a shifted byte-derived mask from
  each longword. In the nonzero route it applies the complementary set form.
- Restores the saved registers and returns at `$C33168`.

The two forms are algebraic mask updates only.  No record type, field meaning,
display interpretation, or gameplay role is assigned from this evidence.

## Native port contract

`port/glyph.c:fa18_apply_glyph_mask_lane` ports the complete proved mask
algebra to a named `FA18PlanarPage` plane. `FA18GlyphMaskLane` names the
selected plane, bounded byte offset, glyph-byte stream, source mode/shift word,
and row count. It does not carry a CPU register file, Chip-RAM address, or
unbounded pointer.

The native routine retains the source's big-endian longword update, 40-byte
stride, high-nibble set/clear selection, and one rotate-and-narrow shift
derivation before the row loop.
`fa18_glyph_contract_test` checks both `$0B0A` clear and `$0BFA` set forms, a
nonzero shift derived from bits 12--15, and bounds rejection. The caller's
mapping of glyph records and destination lanes remains separate.

## Authority and limitation

Execution is established by
`build/run029_profile_full/profile.json`, a normal full-frame replay of the
sealed run029 input oracle.  This is profiler evidence, not an instruction-step
call/return packet for that run.

A bounded run060 no-input packet now supplies one concrete invocation:
`build/port_run060_c330fe_120/` starts at success-text checkpoint frame 9,285
and reaches `$C330FE` in its continuation frame 2. Entry has `D1=$018E3A`,
`D2=$FBFA`, `D3=$F000`, `D4=$C3DA8D`, and `D7=$01C2`. The source bytes are
`80 80 80 C0 C0 C0 F8`; the destination is plane 4's `$04BA` offset. The
seven final big-endian longwords at 40-byte intervals are:

```text
00010000 00010000 00010000 00018000 00018000 00018000 0001F000
```

`fa18_glyph_contract_test` reproduces this exact bounded output using native
plane index 3 and byte offset `$04BA`. This establishes one live glyph lane's
source, destination plane, stride/count, and set form. It does not establish
the screen identity of the text record or all caller modes.
