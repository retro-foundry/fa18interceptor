# Native renderer clear and post-input display stages

`port/renderer_clear.c/.h` implements the complete `$C2FD22` body with the
actual native graphics plane objects. `port/post_input_display_stages.c/.h`
implements complete `$C0FA04`, `$C0FA4C` and `$C0FA80` with references to the
ordinary command, viewport, countdown, callback and auxiliary owners.
There are no CPU registers, opcode handlers, guest pointers or bus accesses
in these production modules.

The clear captures the first five buffer spans at entry and makes 2,000
iterations of four ordered longword clears. It then reads the live fifth
buffer flag; the fifth cursor advances only when that byte is nonzero.
After the A phase, it captures all five B spans and clears them interleaved
for 2,000 iterations. Aliases and overlapping spans retain the source order.
An unused fifth A pointer need not resolve. A missing or short required span
returns an explicit error, retaining the preceding stores.

The native graphics setup supplies nine source pointers, sharing its lower
four planes between five-plane and four-plane displays. The tenth pointer
at original `$C456E2` requires a separate supplied plane owner. Its allocation
and lifetime have not been established in the native startup graph. The
clear supplies no additional allocation or assumed alias. The queue binder
attaches the live fifth-buffer flag at bounded neighboring-data offset `$75`.

`$C0FA04` clears through the real native routine while the signed countdown
is nonnegative. On expiry, it calls the required complete `$C0FAA4` scene
initializer before storing recorder mode 3, origin mode 0, countdown 2,
viewport target 15/current 0 and the semantic `$C0FA4C` callback identity.
Its actual scene child remains explicitly contracted; the complete shared
native initializer graph is pending. Child completion is separate from
the original incidental D0 value, which this parent does not use.

Expired `$C0FA4C` clears the auxiliary byte before testing viewport equality.
A mismatch keeps that clear and leaves the callback/countdown alone; equality
sets countdown 2 and `$C0FA80`. Expired `$C0FA80` clears the shared command
event/origin gate, sets auxiliary to 1 and installs `$C10C08`. Both entries
do nothing while the signed countdown is nonnegative. Their binder attaches
the auxiliary byte at queue offset `$34`. Countdown and callback references
can directly use the native tick/publisher controller's storage. The full
legacy tick phase/event state still requires shared integration.

Run `python tools/recomp/check_native_post_input_display.py`. Four sets of
4,096 complete calls match every Chip/Slow RAM byte outside the explicit
CPU ABI stack exclusion `$C7FD00..$C7FF00`. All **62/62** original boundaries
are covered: 27 clear instructions and 35 parent-stage instructions. The
real clear instructions execute fully, including when called by `$C0FA04`.
Only `$C0FAA4` is contracted: 1,638 scene entry states match, and independent
shared-field effects in both runs prove the parent resumes in source order.
Volatile original CPU return registers vary independently and are ignored.

Fixtures cover signed countdown boundaries, both viewport equality outcomes,
gate values 0/1/$80/$FF, shared/shifted/overlapping spans and unused NULL A4.
For isolated clear entries, an A0 or A4 span contains the actual gate byte;
clearing it stops the fifth cursor and preserves the remaining fifth-plane
data. Its Slow span is the canonical owner in the fixture, so serialization
does not overwrite clear effects with stale scalar copies. Original-address
packing exists only in this validation harness.

The native integration contract initializes the real graphics storage,
shares its lower four planes, supplies a separate tenth buffer and binds
the actual command queue. A signed-index publication changes the same fifth
flag consumed by the clear. It also checks stage write order, shared controller
storage and preservation of writes on missing spans/scene-child failure.
Strict GNU compilation and CPU/bus/machine/guest/host symbol inspection pass.
Native MSVC game/test builds, six focused CTests and the unchanged 467-file
native guard pass. The checkpoint records source seals and implementation
hashes in `analysis/figures/native_post_input_display_checkpoint.json`.

These stages are F/A-18 policy. Reuse the existing ordinary-buffer graphics
cores in future ports; their source-specific clear topology and countdown
rules should stay in this adapter. Full `$C08F26` bootstrap, `$C0FAA4` and
other installed stages, the tenth buffer producer, loading/checksum producers,
sample output and complete native scheduling remain open. Native main does
not call this graph; the playable reference still uses emulation.
