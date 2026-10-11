# Ordinary-buffer viewport construction and merge

`port/amiga/viewport_list.c/.h` implements the existing host's MakeVPort and
MrgCop data operations without guest memory or allocation. It builds the
six-byte CopIns records from ordinary geometry, bitmap and palette fields,
then merges them into caller-owned four-byte list buffers. Both packed host
services now call this core. The independent `amiga_viewport_list` library
depends only on the ordinary-buffer RGB4 component and the C standard library.

The builder retains the source width/height/depth checks, signed coordinate
sums, low-byte display-window packing, fetch floor and minimum, mode mask,
wrapped modulo words and unsigned 32-bit raster/plane payload addition.
It writes one WAIT, fixed viewport setup, two records per plane and at most
32 initial colour records. Raw colour words are masked only for list output.
Build and palette stages remain separate so a late palette lookup failure
retains the host's preceding writes and allocation/free order.

Merge preserves list order and accepts only MOVE and WAIT records. WAIT
packing and the final `$FFFF,$FFFE` marker match the previous host body.
Unsupported records fail after preceding output writes. The packed adapter
retains its linked-list cycle/count checks, descriptor updates before each
source stream, allocation/free behavior and publication after a successful
merge. The ordinary-buffer core performs no Copper execution, CPU execution,
DMA scheduling or custom-register access.

`AmigaNativeViewportLists` owns real bounded record and merged buffers with
native RGB4 descriptors. `amiga_build_native_viewport` builds/merges one
viewport and attaches the actual buffer writer used by the existing input
palette backend. Its source parameters and palette must come from the caller;
it supplies no game dimensions, plane storage, scene, scheduler or palette.
Invalid geometry leaves a previous owner intact. Later errors retain the
preceding construction writes and return failure. Owners stay at a stable
address while their internal descriptor references are used.

Plane words are list-format payloads. The packed adapter supplies original
pointer values; native code supplies private identities/offsets paired with
its actual buffers. The builder never dereferences or fabricates a pointer.
The integration contract uses the actual renderer setup's five offsets into
its owned 40,000-byte page, then resolves the constructed list to those same
buffers at presentation. No captured RAM or guest address is used natively.

Run `python tools/amiga/check_viewport_list.py`. It reads the frozen
pre-extraction implementation from commit `5671d334`, builds it only in the
oracle and compares **16,384 construction plus 16,384 merge calls**. Returns,
every byte of the 128 KiB fixture bank, and the complete allocator/host state
match. Construction has 4,864 successes and 11,520 errors; merge has 3,171
successes and 13,213 errors. Fixtures cover boundary geometry/depth, signed
positions, raster and pointer wrap, colour counts, rebuilding old lists,
allocation failure, invalid metadata/palettes, empty/multiple/cyclic viewports,
invalid records and failure after preceding writes. These prove extraction of
the accepted behavior-level host services, not exact Kickstart layout/timing.

The expanded `fa18_input_palette_contract` builds both native lists from
renderer-owned plane offsets, runs the actual input callback/palette/fade,
checks the old/new display lists, retains higher colours, and presents the
constructed view from actual native page bytes. Stable reloads and real
buffer-write failures remain checked. Test pixels are explicit fixtures;
this does not establish a complete native scene producer or running flight.

GNU strict-warning core/integration contracts and symbol inspection pass
without CPU/bus/machine/guest-memory/host-service symbols. Native and reference
ROM-free MSVC builds pass, all sixteen affected native CTests and the host
compatibility test pass, and the unchanged native guard passes 445 files.
RGB4's 16,384 frozen-service comparisons still pass. Both callback runs still
pass 4,096 calls and all 198 original boundaries independently.

The checkpoint is `analysis/figures/native_viewport_list_checkpoint.json`.
Original game graphics setup, asset import, active page publication,
input/tick scheduling and gameplay/audio composition remain to be connected.
The bounded native main still does not call these owners; the playable
reference remains emulated. The complete native game goal remains unfinished.
