# `$C1DC1C-$C1E0B0`: runtime scene-placement record builder

Classification: **scenario-backed dataflow slice**.  This is a bounded write
contract inside a larger active update path, not a completed-function claim.

## Authority

- Sealed `run033` replay, restored at frame 403 and instruction-traced for the
  next three chipset frames:
  `build/run033_placement_bulk_404_trace/trace.jsonl`.
- The independently sampled prefix table shows the first bulk mutation at
  replay frame 404, changing record groups 0--37, 37--60, 70--96, and later
  groups over frames 404--408:
  `build/run033_placement_prefix_mutations/memory_region_mutations.json`.
- The records begin at `$C4E9AA` and have a 24-byte stride; their descriptor
  and coordinate layout is recorded in
  [runtime scene placements](../data/runtime_scene_placements.md).

## Observed write contract

The traced bulk pass clears `$C4E988` at `$C1DC1C`, then repeatedly advances
that counter at `$C1E0BC`.  During the same pass, `A2` walks the mutable table
at 24-byte intervals.  For example, the first captured in-window destination
is `$C4ED2E`; the following stores cover one record prefix and its mutable
tail before the next record starts at `$C4ED46`:

| PC | Observed store | Destination at the sampled iteration |
| --- | --- | --- |
| `$C1DD54` | `MOVE.W D6,(A2)+` | `$C4ED3A` |
| `$C1DD88` | `MOVE.L A1,(A2)+` | `$C4ED3C` |
| `$C1DF3C` | `OR.B D6,-5(A2)` | byte within the current record |
| `$C1E04A/$C1E054/$C1E05E` | three `MOVE.W D0,(A2)+` stores | `$C4ED40`, `$C4ED42`, `$C4ED44` |
| `$C1E098-$C1E0B0` | long/word/byte stores and clears | `$C4ED46-$C4ED4E` |

The next pass repeats the same store shape with `A2=$C4ED52`.  A second
observed iteration makes the record field order explicit: at `$C4F03A`,
`$C1DD54` writes the selector word; `$C1DD88` writes `A1=$C22700` as the
descriptor pointer; and `$C1E04A/$C1E054/$C1E05E` then write the three
coordinate-bearing words.  The 24-byte spacing and sampled bulk mutations
establish that this slice builds/refreshes the runtime placement records rather
than merely reading them.

## What this proves about terrain data

The flat coordinate-bearing record layer is generated or refreshed by game
code before the `$C1CB74` selector consumes it.  It is therefore a runtime
scene-placement cache, not the immutable terrain source.  The stored `A1`
value is an exact pointer into the `$C22000-$C22FFF` scene-descriptor family
in this observed iteration, joining the runtime record to the relocation-backed
static scene table.  The three coordinate words are not copied directly from
that descriptor: immediately before each store, the observed path loads and
arithmetically shifts mutable coordinate state:

| Store | Input | Operation |
| --- | --- | --- |
| `$C1E04A` | `$C456EE` | `MOVE.L` then `ASR.L D6,D0`, low word stored |
| `$C1E054` | `$C456F2` | `MOVE.W` then `ASR.W D6,D0`, stored |
| `$C1E05E` | `$C456F6` | `MOVE.L` then `ASR.L D6,D0`, low word stored |

Their immediate writers are now observed in the same bulk trace:
`$C1DDCA` stores `D6` to `$C456EE`, `$C1DE04` clears `$C456F2`, and
`$C1DE38` stores `D6` to `$C456F6`.  These are per-record scratch values—the
stores repeat before each following `$C1E04A/$C1E054/$C1E05E` triplet—rather
than a single global player position.  The `D6` producer and its relationship
to the static descriptor still need tracing before this can be called the
original world-placement table.

The next valid step is a focused call/return trace that records `A1`, the
three `D0` coordinate words, and the source reads immediately before this
builder's first `$C1DD54` entry.
