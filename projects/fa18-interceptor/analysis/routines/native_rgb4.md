# Ordinary-buffer RGB4 service

`port/amiga/rgb4.c/.h` extracts the existing behavior-level LoadRGB4 service
into an independent ordinary-buffer component. `amiga_rgb4` builds with the
C standard library alone. Both `host_graphics.c` and the actual native
`input_palette.c` backend use this same implementation. Guest address lookup
remains entirely in the compatibility adapter; the component has no CPU,
guest memory, allocator, ROM, SDL, game state or machine timeline dependency.

The palette owns big-endian raw words. Loading clips to its actual word
capacity, copies those words, then updates matching six-byte CopIns records
in order. Only opcode-zero colour destinations within the supplied count
change; their data words receive the low twelve bits. WAIT masks, other
registers, higher colours and unmatched records remain untouched. An actual
merged-list owner receives each corresponding four-byte list data write
after the internal word changes. Missing display lists are valid before
construction; missing required buffers and failed hardware writes return
failure with preceding writes retained. Buffer bounds are explicit.

The separate copy and patch stages preserve the compatibility adapter's
order: it resolves the list after copying the colour map. An invalid list
therefore retains a completed map copy; a later invalid hardware location
retains the internal word and earlier hardware writes. Odd packed register
offsets preserve the existing byte read when its trailing byte is available.
The core defines overlapping palette copies with memmove. Its compatibility
proof excludes overlap, which invoked undefined memcpy behavior in the old
implementation.

The actual `$C1718E` viewport argument is `$C1822A`. Its fixed ColorMap field
is at `$C1822E` (offset 4), while its published DspIns field is `$C18232`
(offset 8). The other published field, `$C1821C`, is View.LOFCprList in the
View at `$C18218`. The right-hand pair field now has the correct native name
`display_list`, replacing the earlier ambiguous `palette` name. Publication
behavior and pointer identity are unchanged by that rename.

`fa18_bind_native_input_palette` connects the actual viewport ColorMap and
display owner. Each call converts the callback's sixteen native raw words to
the original byte format and calls the real RGB4 component with the current
published display list. The first transition load reaches the old list;
publication makes the second reach the newly selected list. Both calls share
the viewport's ColorMap. No callback substitute or captured palette is used
in this production backend.

Run `python tools/amiga/check_rgb4.py`. It first checks that its frozen oracle
body exactly matches `amiga_host_load_rgb4` from commit `9645f4de`, then
compiles with GNU strict warnings. **16,384 comparisons** match return and
all 4,096 fixture buffer bytes: 7,014 successes and 9,370 partial/error paths.
Fixtures cover clipped/zero counts, raw high bits, opcode/destination filters,
odd offsets, absent lists/hardware, invalid source/map/list locations and
failure after preceding writes. This proves extraction of the accepted host
semantics; it is not an exact Kickstart-layout or timing proof.

`python tools/recomp/check_native_input_callback.py` now runs two separate
4,096-case comparisons. The controlled LoadRGB4 child-effect fixtures retain
all 198/198 original boundaries. The second run executes the actual native
palette backend against the packed host service while running the complete
original parent and actual fade instructions, also covering all 198/198
boundaries and 2,048 ordered palette boundaries. Both compare every Chip/Slow
RAM byte outside the original ABI stack, entry RAM/arguments and final return.
The second additionally projects all imported colour-map, internal-list and
merged-list buffers back into reference RAM for comparison. The shared RGB4
implementation is independently checked against the frozen pre-extraction
body by the first validator; exact ROM service behavior remains separate.

Standalone GNU core/backend contracts pass with strict warnings, and the
native backend binary has no CPU, bus, machine, guest-memory or host-service
symbols. MSVC native/reference ROM-free builds pass, all fifteen affected
native CTests and the host-compatibility contract pass, and the unchanged
native guard passes 443 files. The integration contract executes the actual
callback, palette service, pair publication and fade, and checks stable
reload, higher-colour preservation and failure before fade.

`analysis/figures/native_rgb4_checkpoint.json` records the extraction proof;
`native_input_callback_checkpoint.json` records both callback runs. Full
native viewport/list construction, asset import, scheduling, sample playback
and gameplay integration remain open. The bounded native main still does
not invoke these new owners; the playable reference still uses emulation.
