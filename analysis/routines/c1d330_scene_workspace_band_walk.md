# `$C1D330-$C1D3E6`: scene-workspace band walk before placement build

Classification: **scenario-backed dataflow slice**.  This is a traced
workspace traversal that precedes the runtime placement-cache builder; it is
not evidence that the workspace is an immutable terrain source or that this
slice writes every band on the observed branch.

## Authority

- Sealed `run033` replay restored before replay frame 404 and traced for the
  next three chipset frames:
  `build/run033_placement_bulk_404_trace/trace.jsonl`.
- The following cache builder and its 24-byte output contract are documented
  in [`$C1DC1C-$C1E0B0`](c1dc1c_scene_placement_record_builder.md).

## Observed band contract

At `$C1D322`, the active trace has `A0=$C4124E` and uses the word at
`$C41250` (`$009E`) as an offset.  `$C1D32C` consequently adjusts the active
stream cursor to `$C412EC`.
`$C1D330` then establishes `A3=$C48390` and `$C1D336` initializes `D4=$0E`.
Each accepted stream item reaches `$C1D3B2`, copies the current band base to
`A1`, and calls `$C1D3F4`.  `$C1D3BE` advances `A3` by `$600` before returning
to `$C1D338` for the next stream item.

The first observed bases are `$C48390`, `$C48990`, `$C48F90`, `$C49590`,
`$C49B90`, `$C4A190`, `$C4A790`, `$C4AD90`, and later `$C4D190`.  This
matches the `$600` stride exactly.  After its stream terminator, `$C1D3E6`
jumps to `$C1DC08`, which immediately enters the placement-cache path at
`$C1DC1C`.

Thus, the placement builder's `$C48390`-relative 96-byte cells belong to a
larger banded workspace traversal in the same live update phase.  This is a
stronger lifecycle boundary for the cache, but it does **not** prove that
`$C1D3F4` populated the bands in this scenario: the sampled helper route calls
`$C1D5D8`, which returns without a traced write while `$C45864` is clear.
The first writer for the relevant band, and its immutable input range, remain
the required evidence for an original terrain/map-data claim.

## Observed cell-marker reset

The same traced update phase has a direct writer for the band cells before the
walk.  `$C1D266` selects `$C411F0`, `$C1D272` saves that table-derived pointer,
and `$C1D276-$C1D280` establishes `A1=$C48390`, `D0=$FFFF`, `D1=$60`, and
`D2=$0D`.  `$C1D282` calls `$C1D722`, whose unrolled stores at
`$C1D722-$C1D75E` write the word `$FFFF` at 16 consecutive `$60`-byte cell
starts.  The `DBRA` at `$C1D286` repeats this for 14 bands: 224 marked cells
from the `$C48390` workspace family.

This marker has a direct downstream meaning in the placement builder:
`$C1DD2C` compares the leading cell byte with `$FF` and branches out of the
record-producing path when it matches.  Therefore this is a proved workspace
cell rejection-marker reset, not coordinate population.  It explains why the
later selector stream only produces placement records for a subset of cells;
it does not reveal immutable terrain coordinates.

## Static selector-input boundary

`$C412EC` is payload offset `$1BC` of original CODE segment 65
(`$C41130-$C42287`).  That 4,440-byte segment has no relocations and is
byte-identical to its original payload in the resolved runtime mapping.  The
traced read therefore proves that the band walk is controlled by immutable
program data rather than by the `$C48390` workspace alone.

The observed stream is byte-oriented: `$C1D338` reads a signed control byte,
and the nearby `$C1D34A-$C1D370` path maps it through `$C411F0` and
`$C1D764` before selecting its per-band helper parameters.  No direct read of
a three-word terrain coordinate from segment 65 is observed in this slice.
Treat segment 65 as a static scene-workspace selector/configuration candidate,
not as extracted map geometry or a placement-coordinate table.
