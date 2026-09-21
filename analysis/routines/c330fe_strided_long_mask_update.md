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

## Authority and limitation

Execution is established by
`build/run029_profile_full/profile.json`, a normal full-frame replay of the
sealed run029 input oracle.  This is profiler evidence, not an instruction-step
call/return packet, so caller identity and concrete register values remain
unproven.  A future no-input breakpoint trace must capture one invocation's
entry registers, source bytes, and destination longwords before this routine
can be promoted beyond structural/dataflow meaning.
