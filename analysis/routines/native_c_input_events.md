# Complete input-event owners (2026-10-02)

The complete C16EAE external-event, C16BF2 keyboard-source, C16C56 raw-poll
and C13D34 changed-button owners are implemented in `port/game/input_events.c`.
CPU/stack adaptation and resumable timing are independent files in
`port/game/glue/`. This batch adds four complete game entries and 109 source
instructions; internal labels are not additional functions.

## Source and behavior

`analysis/data/input_events_source_scope.json` decodes the sealed demo bytes
linearly through each exclusive routine end, checks the generated instruction
set for omissions, and records the state and individual source-span SHA-256.
There are 29/20/41/19 instructions and 110/72/130/80 bytes respectively; no
additional cold instructions were omitted by the generated lists.

C16EAE clears C4582D on an empty result and skips descriptor release. A present
event dispatches precisely $0068/$00E8, reloads the descriptor after that child,
clears its word at +6 and releases it. Both paths refresh the direction child.
The older report incorrectly said both paths release; it is corrected against
the actual branch to C16F16. Descriptor ownership/protocol remain the external
child's responsibility; no key mapping or OS behavior is invented.

C16BF2 saves the event byte before release and restores only D0.B afterwards;
the release child's other D0 bytes survive. C16C56 first consumes the latch
as a word without normalization. Otherwise empty and filtered raw input return
positive $00FF; accepted bytes are sign extended to a word, preserving D0's
high word on the press path. The original release path explicitly extends to
a long before adding $80, so its upper bytes differ. No clean long return is
substituted for these original register effects.

C13D34 requires a nonzero matrix-side metric and ready byte, then both low
button bits. Flag bit 3 selects $77. Otherwise signed level $78..$7F selects
$79; negative levels do not. The mirror is consumed even if that level does
not select a command, but remains unchanged when either readiness gate or
the two-button gate fails.

## Independent whole-call evidence

`python tools/recomp/check_input_events.py` compares all D/A registers,
PC, full SR and every Chip/Slow RAM byte, without exclusions or source patches.
There are 16,384 real-child calls per entry (65,536 total), dispatching original
children through the existing runtime with chipset work held. These observe
12/10/14/19 parent boundaries respectively: the recording's empty event
queues do not exercise the cold nonempty source branches.

A separate executable replaces only the child bridge with declared test
contracts and runs 16,384 cases per entry (65,536 total). The original parent
instructions stay unmodified. Before each child, complete CPU/SR/RAM and exact
entry/return addresses are compared against the readable adapter; contracts
then supply child effects. These cases cover every 29/20/41/19 parent boundary,
all raw bytes and CCR combinations, signed command thresholds, latch words,
child-clobbered registers and replacement of the event descriptor after dispatch.
This is parent structural proof, not a claim to execute original child bodies.

`python tools/recomp/check_whole_call_glue.py C16EAE C16BF2 C16C56`
disables timing bridges for the independent readable-C replay check. It matches
32,126 shadow / 16,736 sandbox completed calls across all three recordings,
with zero mismatches and original production masks.

| Entry | Shadow matches | Sandbox matches | Shadow incomplete | Sandbox hardware |
| --- | ---: | ---: | ---: | ---: |
| C16EAE | 15,439 | 0 | 573 | 16,391 |
| C16BF2 | 8 | 0 | 0 | 0 |
| C16C56 | 16,679 | 16,736 | 57 | 0 |

C16EAE retains the ordered source-first JOY input contract already established
by the enclosing update owner. It supplies input values only; C computes its
own outputs. Hardware/incomplete calls remain separate classifications.
C13D34 has zero calls in every recorded mode. It has complete independent
structural proof above and is not claimed as recorded gameplay coverage. The
four-entry replay invocation correctly refused to count that cold entry as a
recorded comparison; the successful replay command therefore lists the three
observed entries explicitly. No acceptance masks or comparisons were relaxed.

## Timing and integration

The local DMA instruction check passes 109 instructions / 3,488 cases,
comparing registers, full SR, PC, cycles and RAM. It caught and corrected
SEQ's conditional two-cycle charge and the short BSR to C16F1C. The check
is not a separate assertion of a raw bus-access log.

The full **442-entry** gate matches **555,538 shadow / 412,898 sandbox**
completed calls, zero mismatches, all RAM seals exact and poison frames
identical. The fresh combined oracle passes **13,224 instructions / 423,168
DMA cases**, resetting the sealed machine before each fixture.

The isolated group matches fresh source OFF output on all **36,236 frames**
and every sealed final RAM hash. C13D34 remains cold in those replays; its
structural proof is independent. GNU headless and MSVC Release builds pass.
The 600-frame group probe is exact; ALL remains **416/361**. Temporary replay
streams are removed and build/ is **0.386 GiB**. Copper fade remains deferred.
Machine-readable evidence: `analysis/figures/native_input_events_checkpoint.json`.

Next audit complete C1AC28 command-word and C1AD74 keyboard dispatch ownership
together, including their shared command tails and out-of-range C06BF0 path.
The generated lists contain shared spans beyond the immediate entry region;
a prefix or a collection of selected keys would not complete either owner.

The two generated dispatch lists contain 1,104 unique boundaries, with 482
shared (562 for C1AC28, 1,024 for C1AD74). These are planning counts, not a
complete-source audit; check cold bytes and shared ownership before porting.
