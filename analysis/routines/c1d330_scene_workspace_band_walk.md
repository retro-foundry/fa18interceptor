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

At `$C1D32C`, the active trace adjusts an `A0` stream cursor to `$C412EC`.
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
