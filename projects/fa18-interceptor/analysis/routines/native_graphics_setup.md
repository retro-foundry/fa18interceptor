# Complete native graphics setup and palette imports

`port/graphics_setup.c/.h` implements the complete original `$C15DB4`
initializer and actual `$C2F4DE` table child using ordinary plane, bitmap,
palette, display-service and list objects. It also implements the separate
`$C160D6` entry whose direct BSR call is at `$C15D68`, after menu setup.
The `$C16084` first-list tail belongs to the initializer, not a new entry.
Earlier bounded page/pair helpers remain separate callers; their existence
did not supply this complete source ownership or actual native construction.

The initializer opens the native display owner with the original version
request 29 and retains its previous active View. The accepted host's View,
Viewport, bitmap and drawing-port data initialization establishes the source
320x200 five/four-plane structures, 40-byte rows and View offsets 129/44.
The raster initially selects the first bitmap. Four cleared 8,000-byte plane
allocations occur before their combined error check; the fifth has its own
check. Earlier successful allocations and pointer writes survive failure.
The lower four sources are then duplicated and copied into the second
bitmap; `$C2F4DE` produces the exact 8/10 renderer table orders. The index
orders now live once in `renderer_page_layout.h`, shared with the existing
private-offset renderer setup.

Next it allocates a 32-colour map and 64-byte dynamic buffer. The original
32-word mutable bank at `$C1AA9C` is copied sequentially and unmasked into
the map. This routine allocates the dynamic palette but **does not seed it**;
its later original owner remains part of startup integration. The map is
published to the viewport only after copying. X=0/Y=-2 are installed before
the first MakeVPort/MrgCop calls and pair-zero publication. The later entry
clears only the live LOFCprList/DspIns, selects the shared four-plane bitmap,
builds/publishes pair one and resets the shared draw page. The View's separate
SHFCprList survives the clear; the next successful merge replaces it.

`graphics_storage.c/.h` supplies actual native bounded plane/map/dynamic
allocators and separate record/merge services using the existing independent
viewport/RGB4 cores. One five-plane family backs both display lists. Native
plane payloads are private offsets into the renderer's actual buffer, paired
with ordinary buffer pointers; no guest address is used or dereferenced.
`fa18_bind_graphics_renderer` connects that existing family's objects and
offsets to the existing renderer bindings without clearing those buffers or
creating a second display family. Its existing page object is a renderer
cache, not another source display allocation.

The parent ignores MakeVPort/MrgCop operation results in the original.
Native services therefore distinguish a handled operation from an unavailable
service: construction failure retains its prior outputs, and the parent still
publishes them and performs the source page reset. The backend records the
construction failure for its caller. Allocation failure reaches the supplied
termination owner with 1 or -1000; full startup cleanup remains outside this
initializer. Invalid native buffers fail explicitly without manufacturing data.

`fa18_decode_copper_page_at_vpos`/`fa18_present_copper_page` now support the
actual second display's four-plane list. Existing blank/full-state decoding
is preserved; presentation uses the active four/five planes in the source's
bit order. This interprets constructed display metadata and renders native
buffers directly, without Copper/chipset cycle execution.

Run `python tools/recomp/check_native_graphics_setup.py`. It verifies sealed
source bytes and compares **4,096 first-entry invocations** (1,024 normal,
3,072 terminating), then **1,024 actual second-entry invocations**. All
**194 reachable instruction boundaries**, including the real table child,
are covered. Full Chip/Slow RAM outside `$C7FD00..$C7FF00` matches at all
29,184 allocation/construction/termination boundaries and after completion.
Native object identities and private plane payloads are serialized to packed
reference objects only in the oracle. Successful record/merge calls use the
actual native backend versus packed MakeVPort/MrgCop. Fixtures include each
plane allocation failure, library failure, raw high palette bits, arbitrary
draw pages, shared plane/table topology, unseeded dynamic buffers and ignored
second-list construction failures.

The three static error-call return sites `$C15DDE/$C15F88/$C15FBE` are not
returning game paths: `$C50DE8 -> $C522D0 -> $C0DFE6` restores the startup
stack from `$C07F74` at `$C0E028` and returns to the loader at `$C0E032`.
The checker validates this evidence and explicitly records those fallthroughs.
OpenLibrary/initialization use accepted host contracts; termination stops at
the controlled nonreturning child and does not claim native cleanup parity.
Exact Kickstart initialization/layout/timing remains a separate reference scope.

`display_palette_assets.c/.h` imports the initial palette from the actual
caller-selected `inst5`/`frnt5` ILBM CMAP, plus Hunk 21's 32 static words at
offset 0x40 and all sixteen raw mode palettes at 0x80+(15-mode)*32. Pointer
members are bound to views of one source-order bank after copying. The complete
native `$C0F812` publisher now consumes its first 32 words as the actual dynamic
seed; see `native_postflight_text.md` for its bootstrap dependency and proof.
The original disk evidence in `analysis/disk_graphics_assets.md` establishes
that both resources share the `$C1AA9C` palette; the splash palette differs.
No last-loaded resource, mode default, dynamic seed or pixel content is invented.

Run `python tools/recomp/check_display_palette_assets.py`. The strict GNU
standalone importer reads the original hash-verified ADF, its executable and
both resources; each matches all **320** initial/static/mode words against
the sealed source. Raw high-bit Hunk mutations, owned palette pointers and
truncated-resource rejection also pass. Its reference fixture is generated
only under ignored `build/recomp`; no palette snapshot enters production.
The extended proof also checks the contiguous seed and 32 bytes of selector
97's actual mutable descriptor in Hunk 64, with a write/read ownership check.

The native integration contract constructs the source's two different list
depths using real bounded storage, binds renderer pointers, presents their
actual buffers, then runs the real input callback on those same lists and
map. Explicit test pixels are fixtures, not a running flight scene. It also
checks construction failure publication, preserved SHFCprList, unseeded
allocation and storage exhaustion. Strict GNU/MSVC builds and all six affected
native CTests pass, including the existing blank-state decoder test. GNU
symbol inspection of integration and asset binaries finds no CPU/bus/machine/
guest/host-service dependencies. The unchanged guard passes 454 files.
Both outer display proofs still pass 8,192 calls and all 80 boundaries.

Checkpoints are `native_graphics_setup_checkpoint.json` and
`native_display_palette_assets_checkpoint.json` under `analysis/figures`.
Actual startup loading order/bootstrap and seed scheduling, native waits/presentation
scheduling, full gameplay/update composition, audio playback and termination
cleanup remain open. These components link into the native target, but its
bounded main still does not invoke this graph. The playable reference remains
emulated; the complete emulation-free goal remains unfinished.
