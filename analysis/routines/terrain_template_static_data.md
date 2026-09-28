# Terrain-template immutable source binding

`terrain_template_static_data.{c,h}` binds the original executable payloads
used by `$C1D330-$C1D51D`:

- Hunk 65 (`$C41130-$C42287`) at `+$1BC` is the observed `$C412EC` band
  control stream; and
- Hunk 66 (`$C42290-$C429C7`) at `+$100` is the `$C42390` relative group
  directory and its static template streams.
- Hunk 65 at `+$C0` is the 21-byte `$C411F0` control translate table.
- Hunk 8 (`$C1C2C8-$C1DBDB`) at `+$149C` is the 21-pair `$C1D764` delta
  table, and at `+$15EE` the 16-pair `$C1D8B6` special-value table.

The loader returns the source spans and directory offset. `$C19A9C` bit gates,
`$C4F03A` placement cache, and workspace bands remain mutable parent state.

`terrain_template_workspace_pass.{c,h}` is the explicit `$C1D330-$C1D51D`
composition boundary. It feeds those immutable spans to the existing band walk
and stream selector; its caller supplies the live selector terms, bit gates,
optional append records, and workspace. It is not scheduled until its real
frame owner is recovered.
