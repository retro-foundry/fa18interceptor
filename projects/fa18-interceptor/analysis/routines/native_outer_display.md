# Native outer display synchronization

`port/outer_display.c/.h` implements the complete original `$C1612C` owner
as ordinary C. `python tools/recomp/port_info.py C1612C` gives the sealed
source from LINK at `$C1612C` through RTS at `$C16282`. It calls the same
native pointer-pair publication owner used by the complete `$C1718E` input
callback. Page selection, saved View.LOFCprList/Viewport.DspIns pair,
dynamic palette pointer and mode-state byte have one actual shared owner.
Activity and status are pointers to their enclosing game owners.

The source first waits for publication, captures the signed draw-page word,
publishes that pair, then loads the View. A nonzero activity byte causes an
additional viewport wait and blitter wait, even if the byte is negative.
Each positive signed-byte iteration loads 32 static words from `$C084D0`,
waits twice, rereads the dynamic pointer at `$C45660`, loads all 32 words,
waits twice and rereads/decrements the activity byte with byte wrapping.
A wait that changes zero to the final decrement therefore produces FF and
exits the signed loop. No artificial iteration limit is added to game code.

With zero activity, a nonzero mode-state byte is decremented before testing
status bit 8. If that bit is clear, the source waits and rereads/loads the
32-word dynamic palette. Finally it rereads and toggles the draw-page word
as `1 - page`, with word wrapping. Service calls may change shared fields,
palette pointers or table contents; subsequent source reads see those changes.
No original stack-frame locals are exposed as native game state.

Native wait, LoadView and palette callbacks return explicit success/failure.
Missing or failed services return failure without substitute execution;
preceding publication, mode decrement and palette writes remain. The
input/outer palette calls now share `fa18_load_native_display_palette`:
the input adapter requests 16 words, the outer owner requests 32. The service
reads the actual fixed ColorMap and currently published native display list.
The independent RGB4 component still handles raw map words and masked list
writes, including its partial hardware-write failures. Original dimensions,
palettes, input scheduling and scene production remain caller-owned.

Run `python tools/recomp/check_native_outer_display.py`. The checker derives
all 80 boundaries and exact instruction bytes from the sealed source, and
executes **4,096 controlled-service plus 4,096 actual-RGB4 calls**. Each run
covers all 80 boundaries and matches 305,158 ordered service boundaries.
Comparison includes full Chip/Slow RAM outside the original CPU ABI stack
`$C7FD00..$C7FF00`, source toggle register outputs, child argument identity,
all 32 palette words and every mutable fixture owner/list buffer at each
service call. Fixtures include signed neighbor pages, 0/1/2/3/127/128/FF
activity, wrapped mode-state bytes, status gating, changed page selection,
changed table entries, dynamic-pointer replacement, static/dynamic aliasing,
activity changes during waits, and ColorMap capacities 16/32.

WaitBOVP, WaitBlit and LoadView are explicit controlled synchronization and
presentation contracts in this oracle, changing only ordinary shared owners.
They do not mutate ABI locals or claim original Kickstart scheduling/timing.
The second run executes actual native 32-word RGB4 versus the packed host
service, whose semantics already have a separate frozen-service comparison.
Musashi/ROM/captured initial RAM appear only in that reference executable;
native production code imports no guest memory and contains no CPU model.

The native integration contract builds both lists using actual renderer
plane offsets, presents the selected list from actual native page bytes,
and invokes the real mouse/viewport/master-fade callback during a publication
wait. Input and outer update use the same objects throughout. It also checks
signed activity changes and retained writes on late wait/hardware failure.
Pixels and palette words in this test are explicit fixtures, not captured
frames or proof of a complete original scene producer.

Strict GNU and MSVC contracts pass; symbol inspection of the independent
GNU contract finds no CPU/bus/machine/guest/host-service symbols. All 19
selected native CTests pass after building the previously absent legacy
full-loop contract executable. The unchanged native guard passes 447 files.
Both complete input callback proofs still cover 198/198 boundaries, and
the RGB4 frozen-service comparison still passes 16,384 calls.

The checkpoint is `analysis/figures/native_outer_display_checkpoint.json`.
The older `outer_loop_child` wrapper retains its separate bounded page
metadata and callers; it is not the shared native input/display graph.
Original graphics setup, palette asset import, scheduling, sample playback
and full game composition remain open. The native main still does not call
these owners; the complete emulation-free game is not yet achieved.
