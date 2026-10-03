# Complete formatter and game-side mode-file callers (2026-10-03)

C0F56A, C0EF08, C162E4, C1631C and C16386 now have complete readable
whole-call domains and source timing. The registry contains 477/624
translated entries plus 56 original source-only callable entries: 533 rows
and 347 timing entries (291 translated plus 56 source-only).
C0EF08 is an added translated owner; C162E4/C1631C/C16386 are added
source-only callable owners. C0F56A was already registered and timed; this
batch completes its CPU/frame adapter without counting it as a new entry.
The 624 denominator remains recording-seeded translation coverage.

`python tools/recomp/audit_postflight_file_callers_source.py` seals all 174
unique / zero shared boundaries within the batch. The formatter's 41 PCs
reuse the existing `glue_C24E2C_step` timing engine through a wrapper with
exact source-PC ownership. The former duplicate wrapper is removed from
`glue_number_field_step.c`; its original instruction cases remain intact.
There are 133 new PCs in the independently tested timing union.

`postflight_file_callers.c` owns the complete decisions and frame effects.
Its CPU adapter retains sixteen actual child sites with original return PCs
and stack arguments. It invokes the actual platform children. Controlled
children are confined to the proof fixtures; they are not production service
implementations or evidence of completed OS parity.

The formatter repeatedly reloads its rewritten pointer, value, width and
local counter. It retains both loops, the leading-zero space writes, byte
arithmetic wrap, all argument/local stores, D0 high halves and A0 effects.
In particular width 0x80 decrements to 0x7F before the second loop. The old
adapter inferred the rewritten fields from the final output, omitted the
original frame and CPU effects, and used a bulk shift that could shift by
32 bits. The complete frame domain uses the original four-bit shifts and
preserves reads after potentially aliased output writes. The existing typed
`format_hex` helper remains available to ordinary typed callers; registered
original whole calls use the complete frame domain.

C0EF08 retains the original allocation size/flags, pointer alignment, lock,
examine, unlock and free sequence. It classifies the original negative tag,
BAD tag and size 0x50, without adding a new allocation-error branch.
C162E4 retains release/check/load-or-save/own ordering and the word result
stores. C1631C preserves the original nonpositive-open and write-minus-one
returns, including the absence of an added close on the failed-write route.
C16386 rejects only read result -1; zero and short reads retain the original
success route. It reloads the saved read result after the actual close child.

`python tools/recomp/check_postflight_file_callers.py` passes 57,344 complete
CPU/full-SR/all-RAM calls, without exclusions: 40,960 controlled-child cases
across all five owners, covering every one of the 174 boundaries, plus
16,384 real formatter cases covering all 41 formatter boundaries. Fixtures
exercise all CCR combinations, signed width edges including 0x80, values
requiring hexadecimal letters, long fields and output overlapping rewritten
caller arguments. Controlled file children check complete child-entry CPU
and RAM, then change registers, buffer/tag values, saved handles and the
saved read result across close. These changes test the original reloads.

The four file owners each stop at FC0FF0 in case 0 of the sealed real-child
fixture. Their exit-1 logs and raw service-stop JSON remain retained. The
verifier requires these exact original stop classifications, and counts no
complete real-child proof for them. Actual file loading and independent
OS/hardware timing parity remain open. Actual children permit CIA reads and
hardware side effects through write-log mode 2.

`python tools/recomp/check_postflight_file_callers_dispatch.py` passes 768
actual ON/shadow/sandbox formatter fixtures. Every mode covers all 41 PCs,
compares full CPU/SR/RAM and retains non-call, OFF, selection and code-write
guards. ON requires a native continuation; both reference modes require
completed matches. All 256 source calls per mode are hardware-free. The
four service-dependent file owners are excluded from this real dispatch
proof, retaining their separate complete controlled-child proof.

The step-disabled recording proof matches nine formatter calls per mode:
three demo, four carrier-success and two qualification-failure calls, with
zero mismatches, hardware or incomplete classifications. All four file
owners have zero recorded calls. The six raw reports retain these cold
rows, and the generic tool correctly rejects C0EF08's zero comparisons.
No completed recording calls are inferred from structural fixtures or RAM.

Local DMA passes 174 instructions / 5,568 cases. The independently tested
instruction union is 17,030 unique PCs / 544,960 DMA cases after the 41-PC
overlap. No fresh combined oracle was run in this batch; the last fresh
combined run remains 16,384 / 524,288. Shared runtime CPU/bus/math helpers
and instruction fixtures are unchanged, and all twelve older generator
outputs are byte-for-byte unchanged.

The full 533-row gate passes 554,063 shadow / 413,303 sandbox matches,
zero mismatches, exact RAM seals and identical poison frames. All 36,236
isolated live ON frames and seals match source OFF. The group is exact
through frame 600; ALL remains frame 416 / 361 pixels. Copper fade remains
deferred. GNU and MSVC Release builds pass; build/ is 0.867 GiB. Evidence
paths, hashes, counts and service limitations are in
`analysis/figures/native_postflight_file_callers_checkpoint.json`.

`python tools/recomp/audit_input_device_callbacks.py` seals the next eight
related game-side owners: C1718E/C17456/C1748C/C174A0/C16CD8/C16B8C/
C17104/C1712C, 274 unique / zero shared boundaries. This inventory
implements none of them and replaces no OS service. Continue the complete
original game graph, then Stage F and only necessary Stage E services.
Stage D and the full C port remain open.
