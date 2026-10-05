# Native scene regions

`port/native_scene_regions.c/.h` implements complete `$C28996`, `$C28B16`,
`$C28B34` and `$C28F16`. Spawning uses the actual native `$C2D954` orientation
owner, which now exposes its inverse-third-angle sine as the carried numeric
axis. The scheduler invokes this family directly; `$C23A7E` dispatch is its one
remaining outer child. The authority is the sealed original instruction graph;
`port/game/flight_dynamics.c` corroborates the source behavior.

Region directory traversal uses the index changed by each spawn/rejection, not
a separate eight-row loop. Empty or skipped spawning sets the index to minus
one; successful spawning clears it. The source increments its low word and
uses its low three bits for occupancy. Directory traversal can therefore exceed
eight entries. The disk's actual directory contains four region references and
a signed-word terminator. Native data windows require that explicit terminator.
Signed nonpositive region counts and the original mode gates remain intact.
A bare dispatch counter of `$FFFF` denotes 65,536 rows; missing supplied rows
fail after preceding completed stores.

Descriptor lookup resolves signed row keys to ordinary live groups, including
aliases into the mutable descriptor bank shared with placement. All descriptor
fields are copied before classification/admission rejection. The third field
is also consumed numerically before that rejection. `FA18NativeAssetReference`
now carries an explicitly bound numeric operand alongside its semantic data
reference; group copies preserve both. It is never reconstructed as an address
or used for dereferencing. Missing numeric metadata fails explicitly. The
original-disk loader deliberately leaves it unbound: the full startup producer
must identify and bind this source numeric input, rather than substituting an
unrelocated asset offset or host pointer.

Point/link publication uses the actual record field view. Exit processing keeps
live copy order and the original inactive-target quirk: the fourth release
coordinate has already been overwritten by the high-byte link selector.
Replacement stores read each supplied parameter immediately before its store;
initial parameter resolution precedes flag/counter changes. Jitter supports
both shared values and original random-word generation, including arithmetic
halving and word wrap. The coarse-position calculation retains sign extension,
word swap and shifted spill bits for negative coordinates. Actual orientation
publishes both matrices and its returned axis.

The Hunk-27 asset loader resolves relative parameters and relocated region
references into live data windows. The shared descriptor loader identifies its
two existing procedure types by relocation target (`10+$14`, `16+$A78`), which
also admits the actual region descriptors `$78` and `$C8`. It still rejects
unbound procedure types. A relocated zero offset now resolves to the first
byte of its target hunk; only an unrelocated zero is null. This repairs the
original `$3C` descriptor's third data reference.

`python tools/recomp/check_native_scene_regions.py` passes 40,960 complete
calls (8,192 per five entries), all 635/635 original boundaries, all game RAM
except the CPU ABI stack `C7FD00..C7FF00`, and independent typed records,
descriptors, carried axis and mutable region index. All source placement,
orientation and trig children run directly, with no child contracts. The disk
proof checks all 2,328 Hunk-27 bytes/relocations, four live region windows and
three original descriptor groups, including the relocated zero reference.
Fixtures exercise descriptor aliases, all count/mode gates, signed admission,
active/blocked records, both jitter paths, negative replacement, release/link
routes, extreme coordinates, arbitrary trig data and random orientation angles.

MSVC Release game/affected builds, eight affected CTests, strict GNU compilation
and the unchanged 509-file native guard pass. The checkpoint is
`analysis/figures/native_scene_regions_checkpoint.json`. The production startup
constructor, explicit numeric descriptor inputs, canonical recorder/counter
bindings, dispatch/lower flight/render owners and native main integration remain
required. Captured RAM, CPU and ROM exist only in the differential proof.
