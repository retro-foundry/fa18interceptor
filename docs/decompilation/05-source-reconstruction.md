# 5. Reconstruct source and connect the implementation

[Process manual](../../REVERSE_ENGINEERING.md) · [Previous](04-tracing-and-ghidra.md) · [Next](06-validation-and-divergence.md)

Reconstruction has two products: source that reproduces authoritative bytes,
and source that expresses demonstrated behavior. Their evidence and completion
claims differ. Keep both useful while progressing toward the intended port.

Begin this semantic phase with the [recompiled baseline](../recomp/porting_game.md)
and its original comparison evidence. Generated C++ remains reproducible while
connected, contract-backed routines replace it. Exact assembly and semantic
meaning are still separate measurements.

## Begin with a byte-exact view

A whole-bank `incbin` scaffold preserves a captured memory image, including
OS/data/unused storage. It proves reproduction of that image, not recovery of
game modules. A Hunk/payload scaffold similarly preserves original container
structure; record which view and relocation phase is being rebuilt.

Keep the preserved binary alongside symbolic source. Replace raw bytes only
inside bounded, report-backed ranges: observed entry/exit, next handler,
table extent, Hunk boundary or known callee boundary. Leave uncertain gaps raw.
Symbols and comments should cite original coordinates and the evidence report.

For conventional `source_amiga/observed/*.asm` slices, an `org $ADDRESS` ties
the assembled output to captured runtime RAM:

```console
python -m amiga_decomp --project your-game run verify_reconstructions --snapshot captures/baseline --no-coverage
python -m amiga_decomp --project your-game run verify_reconstructions --snapshot captures/baseline --write-summary build/reconstruction-coverage.json
```

Supply `vasmm68k_mot` on PATH and one `REGION.bin` per configured memory region
inside the snapshot directory. Each dump must match its manifest size.
The command assembles slices with `-Fbin`, compares every output byte against
its mapped RAM range and rejects mismatches/out-of-bank output. The source
must correspond to that snapshot's relocation/code version.

Convert and compare after each small batch. Assembler output equality proves
the claimed bytes; a semantic comment still needs a routine/state contract.
Assembly counts can include overlaps; coverage uses distinct byte unions.
The current overlap report is diagnostic, so inspect nested/adjacent ranges
when investigating multiple claims on the same bytes.

## Write the routine contract first

For each behaviorally interpreted routine record:

| Contract part | Required evidence |
| --- | --- |
| Authority/boundary | Original bytes, loaded coordinates, code version, actual entries and exits. |
| Caller context | Call sites, stack/register inputs, selected record, mode and observed branch path. |
| Inputs | Width, signedness, table extents, pointer rules and read-before-write state. |
| Outputs | Returned values, caller-visible registers/flags and ordered memory stores. |
| Side effects | Custom-register writes, DMA interactions, callbacks, queues, sound and OS calls. |
| Timing/resumption | Interrupt/frame splits, waits, resumable stages and timestamp units. |
| Accept/reject | Byte/digest/trace oracle, edge cases, failed probes and unresolved paths. |

A convenient report shape is provided in [chapter 8](08-evidence-and-maintenance.md).
Read callers and consumers as well as the body. A final value is insufficient
when ordering affects display, DMA, interrupt acceptance or another routine.

Use conservative names (`candidate_*`, `field_0c`, `observed_*`) until repeated
data-flow/call/scenario evidence supports meaning. File strings, colors or
geometry can locate candidates but do not establish AI, collision or ownership.
Keep rejected interpretations in the report so later work does not repeat them.

## Preserve arithmetic and packed storage

Translate the actual widths and operations rather than the apparent formula.
Prove byte/word sign extension, truncation, carry/borrow, multiply/divide result
layout, shifts, fixed-point scaling, alignment and pointer wrap. A word write
can preserve another part of a register that a caller later reads. Store those
effects in an adapter while an original-register caller still depends on them.

C signed overflow and shifts of negative values need deliberate, tested
semantics. Do not rely on a compiler accident to reproduce a CPU instruction.
Prefer the project's existing exact arithmetic helpers and compare boundary
values with original execution before replacing them with simpler expressions.

Do not invent structures from neighboring bytes. Record offsets/strides,
padding, aliases and lifetimes before moving shared memory into named owners.
That cleanup improves clarity but does not itself prove new gameplay coverage.
Do not merge visual and collision records without a traced handoff.

## Choose an implementation path explicitly

The default path uses the shared literal translator and machine runtime to
establish executable behavior, then replaces related routines with readable
source. Projects can also reconstruct exact assembly or use their existing
native runtime where the accepted task calls for it. The
[recompiler guide](../recomp/README.md) records implemented capabilities and gaps.

With temporary translation or emulation, mark each remaining dependency:
generated instruction helper, shared register state, guest-bus access, chipset
service or ROM call. Static compilation is not readable decompilation, and a
ROM-free launcher can still emulate the CPU and chipset. Separate those claims.

An interpreter fallback or code-invalidation path, if present in a project's
architecture, must be visible in its inventory/report. Unsupported operations
must not silently turn into guessed mechanics or successful no-ops.

## Work in related, connected batches

Before writing a replacement, identify the actual executable, build target,
source selection, registered entry and runtime caller. A compiled owner module
does not count as gameplay progress if that caller never reaches it.

Choose a batch with useful callers/callees and an actual scenario trigger.
Implement the behavior, connect it, then demonstrate execution through that
path. Record what compiled, what was exercised, what dependency was removed,
and what remains. Keep component tests and integrated results separate.

During mixed original/C execution, keep register/flag and guest-memory adapters
at a clear boundary. Reconstruct every caller-live value, including preserved
high words and flags. Direct C-to-C callers can eventually replace adapters,
but only after indirect dispatch, OS callbacks and interrupted/resumed entries
are accounted for. Count each semantic invocation once even if it resumes.

## Compare replacements without perturbing the reference

Where a project's harness supports shadow execution, compare entry state,
outputs, caller-live values, written bytes and ordered hardware effects. A
sandbox must restore its changes and must not advance the primary timeline.
Observed hardware input replay can test calculations, but does not prove the
replacement's live event timing. State exactly which calls were excluded.

Liveness-based comparisons require a correct liveness model. A poison test can
challenge values declared dead; unchanged images alone still cannot prove all
future consumers. Interrupt dispatchers and callers outside the known graph
need conservative treatment. The current framework has no general shadow,
liveness or poison command; retain project-specific harness identities.

After a related batch passes local proof, exercise actual startup/menu/gameplay
scenarios and compare first divergences under [chapter 6](06-validation-and-divergence.md).
Commit validated chunks and update the implementation inventory. Do not spend
an entire session proving disconnected replacements while integration waits.

## Read coverage honestly

The shared reports distinguish observed-executed byte unions, reconstructed
runtime-backed bytes, static-only bytes, all CODE bytes and estimated code/data
denominators. None is a whole-game completion percentage.

```console
python -m amiga_decomp --project your-game run generate_ghidra_function_coverage --output build/function-coverage.md
python -m amiga_decomp --project your-game run list_unrepresented_functions --limit 25
python -m amiga_decomp --project your-game run list_unreconstructed_observed --limit 25 --json build/source-gaps.json
python -m amiga_decomp --project your-game run plan_captures --output build/capture-targets.md
```

These consume conventional P-code/source/analysis inventories. Function reports
also expect `analysis/semantics.json` with `behavioral_routines`. A Ghidra entry
represented inside one exact slice does not establish its whole body or meaning.
Coverage's no-execution/data-reference classification is bounded by the scanned
exports/source; audit indirect/computed consumers before claiming global data.
Candidate data evidence stays separate from the strict denominator.

Acceptance: claimed ranges assemble exactly; interpreted routines have bounded
contracts; replacements are connected and exercised; remaining translation,
machine and semantic debt is explicit.
