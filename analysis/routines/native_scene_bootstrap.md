# Native scene bootstrap parent and resolved viewed identity

`port/scene_bootstrap_native.c/.h` implements the complete `$C08F26` parent
with direct native `$C09266` recorder reset/root placement, `$C1C40C` template
gates, `$C1C63E` record update and `$C1C860` context refresh. It executes the
actual native startup clear/enable, renderer clear,
all-record/workspace clear, player preparation with its real mission-reset
child, start-position tuple, observer child and complete `$C1C40C` gate
construction. Lower children have no default implementation. Completion
status is separate from incidental original register outputs.

The parent references the shared command/context/view/aircraft bank, player
setup, renderer and queue owners. It binds known startup words directly to
spawn, command, cockpit, message and redraw fields. Phase, selection-active,
fifth-plane gate, previous-state and menu-transition queue fields refer to
the actual supplied owners. Other startup words require original imports;
no spare packed startup image or detached geometry state is synthesized.
Every required global and all 22 depth entries use caller-owned references.
Original store order, overlapping text fills, record tails and plane/depth
tails are preserved. Missing resources or child failure return zero after
preceding stores. Binding failure can retain preceding imports but executes
no startup stores.

`port/viewed_record_word.c/.h` resolves the startup viewed word to the actual
`flight.viewed` aircraft pointer in the supplied sixteen-record bank. There
is no duplicate persistent offset scalar. Its import accepts exactly the
sixteen offsets `512 * index`; malformed or out-of-bank data fail explicitly.
Reading derives the word from the current pointer. Writing zero selects
the actual root object. Rebinding reads the live pointer, preserving changes
made by subsequent children rather than restoring the imported word.
Owners, record banks and their metadata must remain at stable addresses.
This bounded identity domain does not establish behavior for arbitrary
16-bit offsets or unsupported record families.

The independent, header-only `field_bytes.h` mechanism now accepts a logical
word owner with value getter/setter operations. `port_fill_field_words`
requires the high/low views of that same owner and calls its setter once
with the whole word, without a getter or an intermediate identity. Ordinary
byte/integer pairs retain ordered stores with no observer between them.
This is an indivisible semantic operation, not thread synchronization.
The generic component supplies no record stride, original address lookup,
CPU/bus callbacks or game policy; F/A-18 supplies the identity adapter.
Byte writes still preserve the other bits and can fail when a resulting
logical value is outside the adapter's accepted domain.

`FA18NativeSceneBootstrapCall` provides a callback adapter for the existing
native startup text publisher. Placement requires no carried register input;
the earlier D7 dependency belonged only to the contracted boundary and has
been removed. Lower child graphs retain their semantic inputs, including the
caller-owned frame gate used by context refresh. The adapter alone does not
install bootstrap into the running game.

Run `python tools/recomp/check_native_scene_bootstrap.py`. Four sets of
4,096 calls cover **291/291** reachable original instruction boundaries:
the complete bootstrap parent, standalone player preparation and standalone
startup clear using the resolved viewed owner, and complete gate construction.
Standalone preparation covers the phase-nonzero branch that the parent's
preceding clear makes unreachable.
This sealed comparison predates the three direct native owners: its three named
children were contracted, while every other actual original
child executes fully. All sixteen valid viewed identities and fifth-plane
gate values 0/1/$80/$FF are exercised with varied records, workspace, globals,
all ten supplied plane buffers and surrounding sentinels.

Every Chip/Slow RAM byte matches, excluding only CPU ABI save/return stack
`$C7FD00..$C7FF00` for bootstrap/preparation/gate construction. Startup clear
has no exclusions. Three ordered full-RAM child-entry snapshots and the
then-carried placement word match. This remains parent-order evidence and does
not prove their current implementations; see `native_scene_placement.md`,
`native_record_update_stage.md` and `native_context_refresh.md`.
Independent mutations to viewed identity, message state, aircraft flags,
observer origin, smoothed delta, scene limit, countdown and a depth tail
propagate to later child entries. Contracted source children deliberately
vary incidental CPU outputs, proving that completion status is separate.
Named record owners are checked independently of their data-view exporter.
Source CPU state, original address packing and Kickstart exist only in this
validation executable. `analysis/figures/native_scene_bootstrap_parent_checkpoint.json`
records the exact proof scope; the earlier emulated bootstrap checkpoint is
preserved separately.

The native contract checks shared-owner composition, all six supplied plane
buffers including aliases, text overlaps, record/workspace/depth tails,
rebind/clear identity behavior, invalid offsets, missing resources and child
failure with preceding writes retained. GNU strict compilation and symbol
inspection pass without CPU/bus/machine/guest/host dependencies. Native MSVC
game/test builds, six focused CTests and the unchanged 478-file native guard
pass. Player/startup/queue/display regressions retain 122,880 comparisons at
their complete 98/21/28/62 source boundaries; their checkpoint hashes match.

`template_bitmask_buffers.c/.h` now supplies the complete gate child using
the same three mutable byte buffers as native game initialization and terrain
consumers. All three 32-byte clears remain interleaved. Directory offsets
must be positive signed words; a nonpositive offset stores $43/$44/$45 in
the actual supplied error-word owner, then returns normally. The original
`$C06C02` release-build hook is an actual RTS; it executes in the source proof
and has no native side effects. Missing data or fields are distinct native
failures, never misreported as those source faults.

Negative list lengths skip. Positive odd lengths truncate after division by
two. A length of 0 or 1 still executes 65,536 iterations, matching the source
predecrement/DBRA loop. A bit's signed arithmetic quotient selects a longword
before/after its row; the low five bits select the bit within that word.
Cross-row and cross-axis writes reach the existing actual gate buffers.
Writes before/after the three buffers require supplied canonical field views;
there is no padding allocation or original-address lookup. Unsupported owners
fail after preceding writes. Native callers with ordinary valid assets need
no surrounding-field storage. All referenced storage and metadata must remain
live at stable addresses.

`fa18_bind_template_bitmask_streams` attaches offsets 0/$100/$200 directly to
the original Hunk-66 payload without importing a copy or clearing output.
The asset proof checks all **1,848** bytes against the sealed source, applying
the recorded relocation targets only in validation. Raw disk-Hunk expansion
then matches all 6,144 gate-buffer bytes from actual original execution.
All **65,536** bit indices, negative/odd/0/1/$7FFF lengths, source faults on
all axes, signed writes into neighbouring owners and preserved surrounding
fields are covered. A placement-boundary mutation of the actual bound stream
is consumed by the real gate child, proving that it does not cache its input.
The legacy return-code initializer and bootstrap share the same algorithm.

Remaining work includes the individual `$C22C80` children, plus unported record
consumers, the actual tenth-plane producer, scene initializer, original asset
loading/checksum production, sample output and installed-stage scheduling.
The native main does not invoke this graph; the playable reference still
uses emulation. This proof establishes parent behavior around the explicit
contracts and cannot establish the children's behavior or the complete game.
