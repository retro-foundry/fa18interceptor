# Complete corner projection and view-record owners

The original twelve owners are implemented in `port/game/corner_view.c`,
with CPU/stack adapters and source-boundary continuations in
`port/game/glue/glue_corner_view*`. `C2E758` and `C2CE82` replace older
adapters; ten seeded owners are newly registered. Registration is now
537 translated plus 73 source-only entries, 610 total, 531 source-timed.
These counts do not establish whole-game completion.

| Original owner | Complete game behavior |
| --- | --- |
| C2E758 | Neighbor/corner selection, four rounded clipping planes, accepted/rejected publication and projected screen points |
| C2CE82 | Three matrix rows with original product/add/shift order and view-depth publication |
| C2CCA0 | View-record position/grid bias, rotation and test |
| C2CD28 | Layered view-record construction and shared C2D082 tail |
| C2CD94 | Three original template pairs, projected point writes and frame loop |
| C2D082 | Depth/distance selection, unsigned scale arithmetic and three layer draws |
| C2D3A4 | Signed coordinate distance and original scale/shift choices |
| C200F6 | Restore saved A1/A5, load frame index, return |
| C203CC / C2058E | Original minus-one shared rejection returns |
| C20826 | Original zero shared rejection return |
| C22C70 | Original return-only render hook |

The source audit seals all 515 unique /106 shared instructions, 25 distinct
child contracts /31 per-owner sites, four oracle ownership arrays, actual
original incoming transfers and the 36-byte pair template at C2CE5E.
Calls and shared branch entries remain distinguished. No selector bounds,
loop caps or substitute child results are added to the game.

All 393,216 whole-call comparisons match every CPU register, PC, full SR,
all Chip/Slow RAM, ordered Custom writes and terminal device state:
196,608 controlled-child calls and 196,608 original-child calls. Controlled
coverage is 515/515 in union. C2CD28 covers 102/139; its missing 37 shared
layer instructions are independently covered by C2D082 (73/73). Every
other controlled owner has full coverage. Original-child union is 433/515;
the checkpoint retains every missing PC instead of claiming full original-
child branch coverage.

The original nonpositive-depth branch at C2EA02 (`6ffe`) remains non-returning.
16,384 independent observations of 64 branches match full CPU/PC/SR/RAM.
The test ends observation with its harness; the production loop receives
no fabricated return. Original-child returning fixtures and this proof
remain separate.

Actual dispatch passes 34,816 calls: eleven owners each pass 1,024 calls in
ON/shadow/sandbox, and C200F6 passes 1,024 synchronous ON calls. Classifications
are hardware-free in every mode; actual observed Custom packets are
110,086 ON /107,526 shadow /107,526 sandbox, including initial fixture blits.
Packet observations and source admission classifications measure different
things.

C200F6 restores two saved pointers above its RTS return. A standalone pending
native frame would mistake saved A1 for that return. Its complete whole-C tail
is proven independently and registered with 40 base cycles plus 16 observed
MOVEM cycles; C20100 already owns all three source continuations. No new
standalone step registration or shadow/sandbox comparison is claimed.
The initial dispatch failure is retained.

The unchanged normal-C checker produces zero mismatches and positive completed
comparisons for C2E758, C2CE82 and C2D082: 7,083 shadow /7,962 sandbox.
The nine other owners have zero recording calls. Its expected rejection
`C2CCA0: no completed comparisons` remains in `corner_view_normal_all.log`;
the cold owners' independent fixtures are their separate evidence.

The full 610-row gate passes 568,446 shadow /443,870 sandbox comparisons,
all three RAM seals and identical poison frames. All 36,236 isolated live
RGB444 frames and final seals match. Local DMA passes 515 instructions /
16,480 cases; fresh combined DMA passes 29,300 /937,600, independently
reconciled as previous 29,042 plus 515 minus 257 overlapping instructions.
GNU headless and MSVC Release builds pass; build artifacts occupy 1.974 GiB.

All 38 older generator recipes produce byte-identical outputs before/after
this batch. Shared runtime, machine, arithmetic, observers and protected
native-check files remain unchanged. The historical committed command-
dispatch output is retained exactly; generator compatibility does not assert
that stale committed outputs equal fresh generation. The older
`corner_edges_registers` replay remains unchanged for its enclosing display
owner.

Family timing is exact through frame 600. ALL still first differs at frame
424 /34,144 pixels. This remains deferred timing evidence.

The next game work includes complete C12950/C131BE control-record readout
and magnitude dispatch, other larger parents and older adapters, C2C392's
unresolved computed table extent, C1612C graphics-wait integration, and full
original cold/indirect/callback reconciliation. Kickstart services, Copper
fade and timing remain deferred. The user's game-function stopping point
has not been reached.

Exact scopes, per-owner branch coverage, hardware packets, raw log hashes,
recording hashes and implementation hashes are in
`analysis/figures/native_corner_view_checkpoint.json`.

```text
python tools/recomp/audit_corner_view_source.py
python tools/recomp/check_corner_view.py --cases 16384
python tools/recomp/check_corner_view_loop.py --cases 16384
python tools/recomp/check_corner_view_dispatch.py --cases 1024
python tools/recomp/check_active_planes_step.py --group corner_view --cases 32 --bus
python tools/recomp/check_active_planes_step.py --group all --cases 32 --bus
python tools/recomp/verify_corner_view_recordings.py
```
