# Remaining original model command slots

The isolated `model-command-completion` build implements the five remaining
slots in the original 78-entry model command directory. Release and Debug
startup, flight, model and gameplay preallocation checks pass. The original
directory is now fully dispatched; this count describes command coverage,
not all possible model inputs or flight routes.

The playable caller is `main` -> frontend tick -> flight tick -> scene draw
-> native scene placement -> model draw -> command. Production changes stay
in the existing `port/game/` sources. They remove the missing-command abort
for slots 014, 070, 074, 0B8 and 0BC. No CPU, ROM or chipset adapter is added
to the native executable.

Original commands 070 and 074 return zero without consuming stream data.
0B8 aliases the existing workspace extension command; 0BC aliases its midpoint
extension. Command 014 copies three face-test vertices, counts the face result,
then consumes segment pairs regardless of face rejection. It draws a pair
only when the bitwise AND of both depth words is zero. Negative and zero
pair counts exit after the original word decrement, including signed overflow.

A complete comparison exposed a register result that ordinary line callers
do not consume. Original C2FB4C returns its signed blitter stride in A3;
C20656 retains that register as the next pair's vertex base after drawing.
The existing typed line result now carries that stride. The exact failing
case, its native/original outputs and original instruction trace are retained.
Resetting the base to the workspace after each pair would change the original.

The independent original oracle checks 917 cases against all non-stack RAM,
display bytes, cursor and full return value. Of 757 face/pair cases, 159 pass
the face predicate and 598 reject it; both groups still exercise pair handling.
The other 160 cases cover both zero-return slots and both aliases. Inputs
include all face modes, counter wrap, signed count extremes, depth combinations,
stored normal selection and workspace word extremes.

Synthetic multi-pair inputs can make the original's returned base address
ROM. The oracle memory backend permits those original reads. These cases
establish component computation, not ROM-independent support in the playable
runtime. Native storage still requires owned memory and retains its explicit
out-of-range failure. Whether actual model assets select 014 and reach a
non-owned address remains unverified. Directory coverage does not establish
asset reachability.

Connected Release and Debug model checks cover the actual 4,000-, 5,300- and
6,100-tick startup/flight inputs: 279, 3,858 and 7,148 native model calls.
Forty startup ground records, aircraft/scenery descriptors, scene parents and
32 inactive-record cases retain their existing original comparisons. Final
model tests take 19.00 seconds in Release and 20.13 seconds in Debug.

Both configurations preserve complete WAV, RAM, image, saved pilot and counter
outputs for intro, Free Flight and final-mission combat against the pinned
native preallocation reference. Project heap violations, SDL gameplay pool
requests and pool failures are zero. These guards do not observe OS or driver
heaps. Production additions use stack values and existing fixed storage.

Earlier tests timed out because a diagnostic environment lookup ran on every
original instruction. The observer now reads that setting once outside its
instruction loop. Comparison rules and ordinary time limits are unchanged;
the final terminal runs pass. Historical timeout and pair-base failure evidence
remain retained rather than being relabelled as passing.

The Mission Five complete drawing run used the unchanged main runner while
this work was isolated. Its evidence remains bound to that runner. Integration
and preservation of its recorded flight must be checked separately before
extending that qualification to a new executable.

See [checkpoint](figures/native_model_directory_completion_checkpoint.json)
for source/executable hashes, allocation results, terminal qualifications and
the compressed retention manifest. Evidence is retained under
`build/native-flight/model-directory-completion`; passing duplicate RAM is
not accumulated. Complete native sound matching remains open, and named-state
cleanup remains outside the current goal as requested.
