# Terrain-template immutable source binding

`terrain_template_static_data.{c,h}` binds the original executable payloads
used by `$C1D330-$C1D51D`:

- Hunk 65 (`$C41130-$C42287`) at `+$1BC` is the observed `$C412EC` band
  control stream; and
- Hunk 66 (`$C42290-$C429C7`) at `+$100` is the `$C42390` relative group
  directory and its static template streams.

The loader returns byte spans and the directory offset only. `$C19A9C` bit
gates, `$C4F03A` placement cache, special-pair table, and workspace bands are
mutable parent state and deliberately remain outside this immutable binding.
