# Run041 `$C1EE14` selector-input provenance

Classification: **same-invocation dataflow**. Regenerate the JSON with:

```powershell
python scripts/analyze_run041_selector_input_provenance.py
```

This consumes the four bounded run041 traces and the verified stage-call
records. It requires the immediate stores to equal the `D1` limit and `D3`
shift later read by `$C1EE14`; a merely earlier write is rejected.

The prior fixed-point/alternate-loop writer hypothesis was too broad. For
every selected `$C35568` and `$C355A0` invocation, the immediate input stores
are in the caller's record loop:

| Stage caller | Shift store | Limit store | Observed source expression |
| --- | --- | --- | --- |
| `$C1CC86` | `$C1CBA8`: `D0 -> $C45AB8` | `$C1CC60`: `D1 -> $C45B40` | primary record loop masks the low nibble of its selector word into `D0`; it later loads `D1` from `+$04` of its active record |
| `$C1CFA6` | `$C1CE66`: `D0 -> $C45AB8` | `$C1CF6A`: `D1 -> $C45B40` | alternate record loop follows the same low-nibble selector and active-record `+$04` pattern |

The source for the primary route is byte-exact in
`source_amiga/observed/walk_flight_placement_records.asm`; the alternate
route is in `prepare_alternate_flight_record.asm` and
`finish_alternate_flight_record_loop.asm`.

## Snapshot-backed descriptor/control join

Each trace begins with a Slow-RAM snapshot.  The analyzer decodes the active
record's selector, descriptor, coordinate words, `+$10` limit field, and the
descriptor's `+8` control field, then compares them with the later observed
stores and `$C1EE14` control pointer.  The control join succeeds for every
sampled call.  Where the trace-start limit still matches the call-time `D1`,
the complete record-field join succeeds too.

| Checkpoint / route | Active record(s) | Snapshot descriptor -> `+8` control | Call-time result |
| --- | --- | --- | --- |
| 5,000 primary | `$C4E9C2`, `$C4E9DA` | `$C22408 -> $C35568`; `$C2241C -> $C355A0` | all checked fields match |
| 5,750 primary and alternate | `$C4E9C2/$C4E9DA`; `$C4F712/$C4F72A` | the same `$C22408/$C2241C` pair | all checked fields match |
| 6,000 primary and alternate | `$C4E9C2/$C4E9DA`; `$C4F6E2/$C4F6FA` | the same pair | later limit fields change within the trace, but selector and descriptor-control fields still match |
| 6,250 primary and alternate | `$C4E9DA/$C4E9F2`; `$C4F6E2/$C4F6FA` | the same pair | some limits differ from the trace-start snapshot; selector and descriptor-control fields still match |

Thus the frame-6,250 caller-pass difference is not explained by these sampled
routes selecting different descriptor `+8` control pointers: both select the
same `$C35568/$C355A0` pair, while only the alternate route reaches the
observed transforms and line output.  This is a bounded control-flow fact,
not a claim that the records represent one object, a physical range, or an LOD
state.  The limit mismatch is also positive evidence that a trace-start
snapshot cannot substitute for call-time record state; the builder/writer
trace remains required for that field.

## Same-record limit updates

The record-limit mismatch is now explained on the sampled paths.  Before the
caller copies `+$10` to `$C45B40`, both routes can invoke the byte-exact
`$C1D91A` fixed-point stage and store its returned `D1` back to that same
record field.  The analyzer records this only when its `A0` matches the later
limit load.  Observed examples are:

| Frame / route | Record and control | `$C1D91A` call | Same-record write | Published limit |
| --- | --- | ---: | ---: | ---: |
| 6,009 primary | `$C4E9DA`, `$C355A0` | `$C1CC2E` | `$C1CC34`: `D1=398 -> +$10` | 398 |
| 6,012 alternate | `$C4F6FA`, `$C355A0` | `$C1CF2C` | `$C1CF32`: `D1=397 -> +$10` | 397 |
| 6,253 primary | `$C4E9DA`, `$C35568` | `$C1CC2E` | `$C1CC34`: `D1=524 -> +$10` | 524 |
| 6,255 alternate | `$C4F6E2`, `$C35568` | `$C1CF2C` | `$C1CF32`: `D1=524 -> +$10` | 524 |

For example, at frame 6,009 the trace enters `$C1D91A` from `$C1CC2E`, returns
`D1=398`, stores it through `A0=$C4E9E6` at `$C1CC34` (therefore record
`$C4E9DA + $10`), then `$C1CC60` publishes the same 398 to `$C45B40`.
This proves a fixed-point-derived mutable detail threshold on these invocations.
It does not establish that the value is physical distance, camera depth, or
an LOD metric: that requires the unresolved live inputs to `$C1D91A` and a
controlled physical-range scenario.

The recorded `A0` values also locate the active mutable record without an
identity claim.  The limit load runs after the loop has advanced over the
two-byte selector, four-byte descriptor, and three coordinate words, so its
`A0 - $0C` is the record start.  For the two selected control streams, the
primary route reads records `$C4E9C2`/`$C4E9DA` at frames 5,000--6,000 and
`$C4E9DA`/`$C4E9F2` at frame 6,250; the alternate route reads the corresponding
`$C4F712`/`$C4F72A`, `$C4F6E2`/`$C4F6FA`, and `$C4F6E2`/`$C4F6FA` records in
the sampled 5,750, 6,000, and 6,250 windows.  These are mutable placement
cache addresses, not static template addresses or proven world objects.

No available run041 selector window includes a `$C1D488` template copy followed
by `$C1DD36` builder read.  It therefore cannot prove how any of these active
records was constructed.  The next trace must start before the placement
builder, retain `$C1DD54/$C1DD88/$C1E04A/$C1E054/$C1E05E`, and continue through
the matching `$C1CC60` or `$C1CF6A` read and returned `$C1EE14` call.

The completed `build/run041_frame05736_builder_to_selector_trace` window
(frames 5,737--5,760) reaches 23 descriptor-stage calls but has no `$C1D488`
copy or `$C1DD36` builder-header read.  It is evidence that this render-loop
interval follows a prior cache refresh, not evidence that the active records
are unconstructed.  A future trace should be triggered from a state-changing
scene/placement update rather than extended blindly around this stable window.

For the two control streams, the immediate writer values change together with
the record source family: primary frame 5,000 writes shifts 2 and limits
545/523; frame 5,750 writes shift 1 and limits 656/611; frame 6,000 writes
shift 1 and limits 512/419; frame 6,250 writes shift 0 and limits 524/457.
The alternate loop supplies the same shift and nearly the same limit in its
calls (one-word differences appear for `$C355A0` in the sampled windows).

Thus the proven selector rule is a decision over fields of the active placement
record. It is not a global fixed-point result. The fixed-point helper can
modify records on other paths, but it is not the immediate producer of the
inputs in these selected calls. Physical coordinate meaning, placement-object
identity, and the reason the alternate route becomes the rendering route at
frame 6,250 remain open.
