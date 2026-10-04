# Complete older rendering parents

2026-10-04. Ten partial adapters now own complete readable C with actual
original child contracts. Registration remains 526 translated plus 66
source-only entries, 592 rows. Source-timed entries rise from 456 to 465
(399 translated plus 66 source-only); C1FB82 already had a timing adapter.
Game-function porting remains incomplete.

| Original entry | Complete behavior | Controlled PCs | Original-child PCs |
| --- | --- | ---: | ---: |
| C2D16C | All shape components, screen bounds and line/polygon/circle children | 174/174 | 116/174 |
| C21500 | Four-point block face and rejection | 55/55 | 52/55 |
| C2122A | Every offset segment, frame count and status | 35/35 | 35/35 |
| C20592 | Split-square diagonal and clip input order | 64/64 | 62/64 |
| C2168A | Both oriented side-triangle clip sites | 121/121 | 112/121 |
| C203D0 | Both square placements and second-face children | 132/132 | 130/132 |
| C201A6 | Record-shadow admission, scale, winding and stream traversal | 175/175 | 175/175 |
| C3019C | Every marker vertex and classify/fill/outline calls | 27/27 | 22/27 |
| C1FB82 | Vector, cross-product and three axis-plane side tests | 123/123 | 123/123 |
| C1F99A | Oriented, ordinary and compressed vertex paths | 195/195 | 195/195 |

The source seal contains 1,101 unique / zero shared boundaries, sixteen
distinct / per-owner child sites, direct callers and six original indirect
table entries at C1F942, returning to C1F944. Exact slots, caller bytes and
observed edges are retained. No whole selector domain or table extent is
claimed, and no selector guard is added. Only ten wrappers in
glue_postflight_hud.c are replaced. Unreachable private replay helpers are
removed; both remaining exported function bodies are unchanged.

Shape processing retains all twelve/nine original component iterations,
unsigned table products, byte-kind branches, signed matrix operations,
overflow-sensitive view tests, packed signed division, source screen clamps
and actual returned drawing state. Face processing retains complete geometry
construction, original mixed/signed branches, source-colour writes and every
second-face call. Stack MOVEM transfers retain flags, original addresses and
sign extension. Offset runs accumulate each returned result in the original
frame word and consume each segment once.

Record shadow retains all admission thresholds, original scale adjustment,
record coordinates, winding rejection, face/point loops and source stream
terminators. Face-side tests preserve both original dot-product branches
and the returned BTST flags on axis tests. Vertex transforms retain full
word/long register state, oriented versus ordinary/compressed input,
original transform-child sites, frame count and every saved cursor. Marker
processing scales every point and consumes classify, fill and outline in
source order. No last-call register replay, convenience loop limit, new
clipping policy or service return is introduced. Domain and whole-call glue
invoke no opcode handlers; generated timing bridges are separate.

All 327,680 whole calls match every register, PC, full SR and all Chip/Slow
RAM bytes: 16,384 controlled and 16,384 original-child calls per owner.
Controlled contracts compare complete child-entry CPU/SR/RAM snapshots,
change all full data registers, flags, valid returned cursors and RAM, and
cover every one of the 1,101 owned boundaries. Source frame and stack remain
owned by the parent. Face-side, shadow and all three vertex paths also have
complete original-child coverage.

Other original-child fixtures use a zero shape view matrix, degenerate
clip/segment geometry and marker vertices beyond the original last-row
bound, including the pre-existing third point that the original scans for
short counts. These frozen-clock fixtures make no active shape/mask/fill/
outline/DMA claim. Exact missing PCs are retained in the checkpoint.
Ordered Custom writes and terminal registers, effective flags, counters
and data latches match; bounded fixtures produce zero Custom writes.

Actual ON/shadow/sandbox dispatch passes 30,720 complete bounded calls,
exact hardware-free classification and all OFF/selection/non-call/source
byte/stack guards. Six stream owners use the original indirect caller.
Independent normal C disables only timing steps and retains busy/range/owns
contracts: all ten owners complete comparisons, totaling 57,634 shadow /
62,893 sandbox. The first six owners are cold in both qualification runs;
all ten are active in demo01. Raw per-recording comparison/hardware/incomplete
counts remain explicit; strict generic zero-call rejection is unchanged.

All 36,236 isolated live RGB444 frames and final RAM seals match. Full
592-row replay passes 568,446 shadow / 443,868 sandbox, zero mismatches,
exact seals and identical poison frames. Local DMA passes 1,101 instructions /
35,232 cases. Independently verified union and fresh combined DMA pass
26,324 instructions / 842,368 cases: previous 25,346 plus 1,101, overlap
123 from the already-timed C1FB82 owner. All 31 older generator outputs are
byte-identical. Signed divide, SUBA.W and data-register EXG recipes are local
to this family. Shared runtime/bus/CPU/arithmetic/observer/classification,
earlier complete HUD domains and user-owned native files are unchanged.
GNU headless and MSVC Release pass. Family timing is exact through frame
600; ALL remains frame 416 / 361 pixels. Build/ occupies 1.599 GiB.

Initial probes caught reversed comparison operands in flag observation,
an original marker drawing path beyond frozen-clock limits, and an incorrect
CMake source directory. Compare ordering, original marker-row fixture bounds
and port/recomp configuration were corrected. Failed logs remain; no source
service or production timing workaround was added.

Next seventeen segment/face/grid/block stream parents have 773 unique / 99
shared boundaries and original direct/indirect callers sealed. Other older
adapters, C2C392's computed transfer, C1612C graphics-wait integration and
the full original cold/indirect/callback graph remain game-function work.
Kickstart and timing remain deferred under the user's stopping condition.

Evidence: analysis/data/render_parents_source_scope.json,
analysis/figures/native_render_parents_checkpoint.json and
analysis/data/face_stream_parents_scope_inventory.json. Reproduce with
audit_render_parents_source.py, check_render_parents.py --kind contract/real
--cases 16384, check_render_parents_dispatch.py --cases 1024,
check_whole_call_glue.py with the ten owners and standard replay/live/DMA/
MSVC gates.
