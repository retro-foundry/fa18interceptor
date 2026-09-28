# `$C1D10C-$C1D32E`: template cursor resolution

The scene-selector setup resolves the cursor consumed by the existing
`$C1D330-$C1D51D` terrain-template workspace pass. It does not directly emit
geometry or submit a page.

## Source-owned packs

The `$C45864` append flag selects the second selector pack when nonzero:

| condition | control root | group directory | gate base | terms | selector byte |
| --- | --- | --- | --- | --- | --- |
| append and `$C45786 != 0` | `$C4124E` | `$C42490` | `$C1A29C` | `$C4594C/$C4594E` | `$C45851` |
| append and `$C45786 == 0` | `$C414B0` | `$C42490` | `$C1A29C` | `$C4594C/$C4594E` | `$C45851` |
| no append, `$C45865 == 0`, `$C45A66 > $FFFF6000` | `$C42092` | `$C42290` | `$C1929C` | `$C45948/$C4594A` | `$C45850` |
| no append, otherwise `$C45786 != 0` | `$C4124E` | `$C42290` or `$C42390` | `$C1929C` or `$C19A9C` | `$C45948/$C4594A` | `$C45850` |
| no append, otherwise | `$C414B0` | `$C42290` or `$C42390` | `$C1929C` or `$C19A9C` | `$C45948/$C4594A` | `$C45850` |

The `$C45865` flag chooses `$C42390/$C19A9C`; clear chooses
`$C42290/$C1929C`. The `$C42092` path sets `$C45856`, which changes the
following offset arithmetic.

## Shared cursor calculation

With selector byte `s`, source first maps `s` through `$C41170`. The mapped
value selects an 8-byte row helper at `$C41180`, a 16-byte group helper at
`$C411B0`, and a 24-byte control-translate block at `$C411F0`. The source
then forms an indexed word offset using `$C411A0`, the relevant helper byte,
and the live selector bytes. It reads a positive word at that offset from the
chosen control root and advances the cursor by that word. In the traced route
`A0=$C4124E`, word `$C41250=$009E` produces `$C412EC`.

The three roots, their helper tables, the selected directory, and gate table
are all required to reproduce `$C1D10C`; treating `$C412EC` as an always-live
terrain stream is incorrect.
