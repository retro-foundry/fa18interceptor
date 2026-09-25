# Reverse caller chains from visible renderer outcomes

**Run041 update:** The priority-1 selector's exact comparison and source
choice are now joined in `analysis/data/run041_descriptor_detail_selector.md`.
The immediate `$C45B40` and `$C45AB8` writers are now proved:
`$C1CBA8/$C1CC60` in the primary loop and `$C1CE66/$C1CF6A` in the alternate
loop. See `analysis/data/run041_selector_input_provenance.md`. The next
high-value unknowns are construction and coordinate meaning of those active
records, and the reason `$C1CFA6` takes over rendering at frame 6,250. These
must be traced backward before assigning physical-range meaning.

Classification: **bounded dynamic caller ancestry and target ranking**. The
chains below are observed execution paths, not a complete call graph or proof
of object identity. Regenerate the machine-readable result with:

```powershell
python scripts/reverse_trace_call_stacks.py `
  --trace build/run037_m_map_stable_20f_trace/trace.jsonl `
  --trace build/run041_frame06250_12f_trace_retry/trace.jsonl `
  --trace build/run042_m_map_transition_trace/trace.jsonl `
  --anchor 0xC2FF48 --anchor 0xC2FA7E --anchor 0xC2AA9C `
  --anchor 0xC2AFE2 --anchor 0xC1EE14 --anchor 0xC1F6F8 `
  --anchor 0xC1F4AC --focus-target 0xC1EE14 `
  --focus-target 0xC1ED3C --focus-target 0xC1ED4C `
  --focus-target 0xC2AA9C `
  --output analysis/data/reverse_call_stack_world_renderer.json
```

## Method and boundary

The analyzer accepts a `JSR` or `BSR` frame only when the next instruction
starts at its target and `A7` falls by four bytes. It closes the frame only
when `RTS` reaches that call's exact return PC and restores the caller's `A7`.
At an anchor it lists active Slow-RAM calls in the anchor's 64 KiB stack region;
this removes separate interrupt-stack frames. A branch into a routine shares
its caller's frame and is reported as the preceding instruction, not a new
call. A trace starting mid-invocation can show only its in-window ancestry.
The JSON retains all observed anchor hits, multiple chains, unmatched returns,
and any capped focus calls. An unmatched return is not silently made into a
new caller edge.

Inputs: sealed run037 stable M-map 20-frame trace, run041 frame-6,250 12-frame
trace, and run042 post-M transition trace. Together they contain 545,582
instruction rows and 504 hits on the selected stage/display anchors. The
byte-exact source, existing scene notes, and these dynamic chains are distinct
evidence layers.

## Chains that explain the endpoints

### M-map polygons

In run037, 63 of 67 `$C2FF48` polygon-wrapper entries have this chain:

```text
$C15DA2 -> $C0EFD4 parent update
  $C0F07C -> $C2AA9C depth/detail preparation
    $C2AB2C -> $C2AB34 wide directory (51 polygon entries)
      or $C2AB20 -> $C2AB5A normal directory (12)
      ... branch/loop into $C2AFE2 packet batch
        $C2AFE2 -> $C246A0 display stage
          $C24D60 -> $C2FF48 polygon submission
```

The remaining four run037 polygon entries are on a separate placement/control
route through `$C1CB26 -> $C1EE14 -> $C2005C -> $C2469E`. Run042 similarly
shows 53 wide and 10 normal entries of its 65 polygons; two entries have only
a shorter in-window chain. Run041 shows both the map path and a separate
placement/control path. The current `$C2AA9C` source already proves the metric
arithmetic and directory setup. `$C1C636` is the proved writer of `$C45A78`:
it publishes the shifted `D1` component of a transformed tuple, as documented
in `analysis/routines/c1c5e0_projection_component_publish.md`. The important
unknown is which live record/camera context supplies that tuple and whether
its transformed component corresponds to physical range. The existing
depth-driven control-stream/packet variants are documented in
`analysis/routines/c2ab34_wide_map_packet_directory_setup.md`.

### Flight/placed-component path

The run037 descriptor route has this observed ancestry:

```text
$C15DA2 -> $C0EFD4 parent update
  $C0F0A6/$C0F0B4 -> $C1CB14/$C1CB26 placement update
    $C1CC86 JSR (A2) -> $C1EE14 or $C1ED3C
      [$C1ED3C can branch through $C1ED48 into $C1EE14]
      branch into $C1F4AC immutable-triple transform
      branch into $C1F6F8 control-stream walker
        indirect display records -> $C2FA7E line or $C2FF48 polygon
```

All seven run037 `$C1CC86` calls in this bounded window return to `$C1CC88`:
five call `$C1EE14` directly and two call the `$C1ED3C` sibling that enters
it by branch. Six of those seven frames reach both `$C1F4AC` and `$C1F6F8`;
the seventh is a 39-instruction exit with neither. The six active frames
collectively reach nine `$C2FA7E` line-emitter entries and four `$C2FF48`
polygon wrappers. This narrows `$C1EE14` to a conditional descriptor/control
rendering stage. It does **not** prove that every primitive in a frame belongs
to one placement record or identify an aircraft, bridge, or terrain feature.

The short run037 return is itself informative, rather than an absent
descriptor. At trace rows 115895–115944, `$C1CC70` publishes descriptor
`$C225FC`'s `+8` field `$C37EA6` to `$C45A36`, and `$C1CC86` calls
`$C1EE14`. The stage scans `$C37EA6`'s first two control words `$4880` and
`$4D00`; after masking `$3FFF` and shifting by the live count `7`, their
values are `17` and `26`, both within the live limit `96`. Bit `$4000` skips
the following four bytes after each. The next word at `$C37EB2` is `$FFFF`,
which branches to `$C1EEA0`; that exit returns `D0=0` to `$C1CC88` without
entering either the triple transform or record walker. This is a tested
stream-sentinel path, not evidence that `$C37EA6` contains no control data.
`source_amiga/observed/return_zero_from_c1ee14_stream_scan.asm` reconstructs
the observed six-byte return.

In run041's close Golden Gate window, `$C1EE14` also receives calls from
`$C1CFA6`; its directly called frames include six `$C1F4AC` and six
`$C1F6F8` entries, four polygon wrappers and 37 line-emitter entries.
Eleven of 12 direct calls return within the trace; one reaches the trace end.
The `$C1ED4C` sibling returns separately after a branch into `$C1F6F8`.
This corroborates the stage across distinct visible scenes, but the first
source-family selection still needs a same-invocation state trace.

## Priority order

| Priority | Boundary | Why it unlocks more meaning | Next exact evidence |
| ---: | --- | --- | --- |
| 1 | Success/failure state writer before the shared postflight scheduler | Run060 now proves `$C0F7FA` expires `$C45AD6` and `$C11186` queues selector 74. The stable `$C458A6=9` is a selected mission context, not the result. The remaining causal boundary is whichever earlier state/callback transition reaches this mode-9 scheduler route in success but reaches the run062 failure route instead. | Compare run060 and run062 at their result-entry checkpoints: retain callback slot, `$C45AD6`, queue/control bytes, and the immediately preceding writer PCs. Keep countdown expiry, crash logic, persistence, and the pass/fail predicate as separate claims until a controlling branch is traced. |
| 2 | `$C1C2C8` origin/active-record triple -> `$C45A62/$C45A66/$C45A6A` prepared-component publisher | This bounded helper joins the active-record triple, shared origin triple, and the proven `$C1D974` magnitude primitive before publishing values read by flight, descriptor, and renderer routes. It has broad downstream fan-out, but does not execute during run060's qualifying path. | Capture one scenario that reaches `$C1C2C8`, retain pre/post triples and its return consumers, and distinguish the mathematical transform from any physical-coordinate or LOD interpretation. |
| 3 | `$C50212` delayed record-update command interpreter in the `$C50158` four-record iterator | This resolves shared record fields `+$24/+$2C/+$30/+$34` and separates delayed updates from the iterator's position/delta fields. Its zero-countdown path is directly seen in 15 P-code packets across menu, attract, flight, and crash-result states. | Capture a nonzero `A3+$2C` expiry with its command pairs and `$C4FFB4` call context. Until then, preserve the static dataflow contract and do not assign record or callback identity. |
| 4 | `$C1CC86/$C1CFA6 -> $C1EE14`, including `$C1ED3C/$C1ED4C` | Common conditional stage between placement descriptors, immutable triples, control streams, and both renderer outputs. | The run033 `$C4E9C2` fixture and frame-5,750/6,000 fixtures now join an active-record construction, selected fields, stage target, cursor, immutable source, walker, and return. Extend only to a distinct builder branch or unrepresented stage branch; do not repeat a static call census. |
| Deferred | `$C1C5E0 -> $C45A78 -> $C2AA9C` | The publisher and map metric formula are byte-exact, but interpreting its tuple as physical range would be LOD-related work. | Retain existing dataflow evidence; do not prioritize new physical-range/detail captures while LOD remains user-out-of-scope. |

`$C0EFD4` is a shared parent and therefore has high raw fan-out, but its
1,008-byte static source and long update cycle make a whole-routine semantic
guess low value. Use its stage markers to isolate the boundaries above. Keep
the run041 Golden Gate source-family change as a separate question: these
chains locate where to inspect; they do not yet prove a physical-distance LOD
predicate.
