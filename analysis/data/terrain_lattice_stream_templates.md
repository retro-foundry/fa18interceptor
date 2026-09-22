# Immutable templates referenced by the selector lattice

Classification: **static template-payload export for a bounded selector lattice**.
These are exact header/two-word records consumed after `$C1D442` advances past
each stream's leading control byte, and before
their mutable workspace expansion. They are not global placement coordinates,
terrain vertices, elevation values, or a complete world mesh.

The 32×32 lattice reaches 21 distinct static streams and 116 non-terminator
static records. The JSON carries every record plus the selector-bin cells that
reference its stream; a zero-record stream is an observed immediate `$FF` terminator.

| Static stream | records | selector-bin cells |
| --- | ---: | ---: |
| $C42644 | 0 | 61 |
| $C42646 | 22 | 14 |
| $C42706 | 4 | 14 |
| $C42720 | 2 | 14 |
| $C4272E | 3 | 14 |
| $C42742 | 1 | 14 |
| $C4274A | 10 | 14 |
| $C42788 | 7 | 14 |
| $C427B4 | 3 | 14 |
| $C427C8 | 7 | 14 |
| $C42950 | 4 | 79 |
| $C42956 | 3 | 72 |
| $C4298A | 2 | 1024 |
| $C429D0 | 2 | 1024 |
| $C429DE | 2 | 1024 |
| $C42ADA | 8 | 14 |
| $C42B3E | 1 | 14 |
| $C42B46 | 1 | 14 |
| $C42B5C | 0 | 1024 |
| $C42B66 | 18 | 14 |
| $C42BD4 | 16 | 14 |

The exact record format and one-byte stream preamble are scenario-backed by the
direct copy trace. A stream's
presence in a selector bin is page-content evidence only: downstream code combines
these reusable records with mutable placement context, so its words must not be
drawn as absolute map points.
