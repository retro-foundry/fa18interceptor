# Code/data boundary estimates

Generated from `analysis/coverage.json` by
`python scripts/verify_reconstructions.py` on 2026-09-25.

The project now reports two useful estimates. The strict figure is the only
one suitable for a byte-completeness claim. The working figure helps prioritize
remaining reverse engineering without pretending that unexecuted scene data is
already fully audited.

| Measure | Strict, confirmed | Working scene-data estimate |
| --- | ---: | ---: |
| Original CODE-hunk bytes | 285,976 | 285,976 |
| Confirmed data in CODE hunks | 66,192 | 66,192 |
| Additional candidate scene/control bytes | — | 10,148 |
| Plausible instruction denominator | 219,784 | 209,636 |
| Exact source | 51,256 | 51,256 |
| Exact-source coverage of denominator | 23.32% | 24.45% |
| Unclassified CODE-hunk bytes | 55,280 | 45,132 |

The 10,148-byte estimate consists of Hunks 41–44:
`$C34A50-$C3720F`. They remain unchanged after relocation in five snapshots,
have no observed instruction starts, and provide observed scene-control data.
Their evidence is recorded in `analysis/model_geometry_boundaries.md` and
`analysis/live_control_streams.md`.

They remain candidates because this is a hunk-level estimate. The strict
classifier continues to require positive reconstructed data references and no
flow reference for the whole hunk. Hunks 45–50 remain outside this estimate:
the present evidence includes valid 68000 instructions in Hunk 46 and has not
proved a full data-only boundary for the others.

The selector result strengthens the separation: `$C35568/$C355A0` are immutable
control-stream inputs, while `$C4E9AA/$C4F6CA` are mutable placement caches.
Neither fact by itself makes a whole hunk executable or data, so the two
estimates remain separate.
