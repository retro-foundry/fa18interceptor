# 8. Preserve evidence and keep the investigation usable

[Process manual](../../REVERSE_ENGINEERING.md) · [Previous](07-hardware-os-and-audio.md)

A result should remain understandable after the original investigator leaves.
Keep authority, method, observed result and interpretation together. General
methods belong in this manual; a project's addresses, scenarios, routines,
measurements and unresolved questions belong under `projects/<id>/`.

## Store each artifact by purpose

| Location | Purpose |
| --- | --- |
| `local/` | Supplied media/ROMs, extracted files and local machine/tool configuration. |
| `captures/` | Canonical recordings, their authority/config/state/input and sealed reports. |
| `build/` | Derived traces, replay images, snapshots, temporary comparisons and build caches. |
| `analysis/` | Reviewed contracts, mappings, coverage reports, experiment results and open questions. |
| `pcode/raw/` | Scoped structural exports and their capture provenance. |
| `source_amiga/` | Exact assembly reconstructions and authority-linked slices. |
| `recomp/` | Game TOML, observed targets and project-owned hook source. |
| Project scripts/tests | Game-aware observers, invariants and acceptance scenarios. |
| Root scripts/tools/tests | Reusable analysis, hardware support and configuration-independent checks. |

These are conventions, not automatic semantic discovery. A directory name does
not establish completeness. Binary media/captures/local caches remain ignored;
version useful reports, manifests, source and tests. Check staged paths before
committing so private media or multi-gigabyte outputs do not enter history.

Original configs and recordings are immutable reference evidence. Relocation
creates derived runtime configs with separate identities. Preserve original
bytes, frame origins and serialized core payloads. Put replay edits and altered
scenarios in new files; never improve a passing result by editing its authority.

## Use a routine report that states what is known

The following is a template, not a completed proof:

```markdown
# Routine: observed role or honest structural name

Status: candidate / exact assembly / interpreted / replacement exercised.
Authority: media, executable, core/config, snapshot and trace hashes.
Coordinates: Hunk/offset, loaded range, code version and capture scenario.
Boundary: actual entries/exits, caller(s), interrupt/resumption conditions.

Inputs: registers, flags, stack, memory fields; widths, signs and bounds.
Outputs: return values, caller-live registers/flags and ordered stores.
Effects: hardware/OS calls, callbacks, queues, allocations and timing.
Consumers: who uses each result, with observed call/data-flow evidence.
Interpretation: meaning supported by the above evidence; rejected hypotheses.

Validation: exact command, build/commit, input/state identity, scope and result.
Limits: unvisited branches, excluded clocks/outputs, remaining dependencies.
Next experiment: a concrete unknown, trigger, bound and expected evidence.
```

Link actual files and measured values rather than leaving template tokens in a
published result. When an interpretation changes, keep its old experiment as
history and update the current conclusion. Distinguish a static possibility,
observed execution, causal interpretation and a qualified replacement.

For shared globals/records, use a field ledger:

| Coordinate | Width/stride | Writers/readers | Lifetime/alias | Meaning/evidence |
| --- | --- | --- | --- | --- |
| Measured address or record offset | Actual byte/word/long width | Observed routines | Initialization/reset/overlay scope | Known behavior or explicit candidate |

Keep unresolved fields structural. Include reset paths and alternative modes;
a field that looks constant in one capture can change ownership in another.

## Track semantic understanding separately

Establish a small meaning ledger early, alongside byte/source coverage. Promote
a name only to the highest level its evidence supports:

| Level | Established meaning |
| --- | --- |
| Structural | Boundaries, cross-references or shape; purpose remains open. |
| Dataflow | Inputs, outputs, writers/readers and callers are known. |
| Behavioral | A formula, transition or ordered effect is demonstrated. |
| Scenario | That behavior is anchored to actual input and visible/game state. |
| Port contract | A reusable oracle/fixture can qualify a replacement. |

Record promotions, proven state ownership, reusable contracts and unresolved
high-impact questions. The ledger's denominator is its declared inventory,
not every possible behavior in the game. A larger labelled map or generated
source tree can improve structure without improving semantic understanding;
report that result and choose the next producer/consumer experiment accordingly.

## Make experiments reproducible

Record the precise question and expected discriminating observation before a
large run. Use a bounded scenario and save:

1. Project ID, source/manifest revision, tool command and working directory.
2. Original/derived config hashes, core/source identity and observer ABI/build.
3. Initial state/input hashes, restore protocol and all frame/cycle offsets.
4. Selected memory/code ranges, scenario trigger and branch/caller filters.
5. Compared state/images/events/PCM and any declared timing/exclusion policy.
6. Results, counts, first divergence, negative controls and retained artifacts.

Hash the data being claimed, including decompressed bytes where compression is
only a storage choice. A full file hash authenticates identity; it does not
prove a trace is complete or its labels are correct. Use tool-level coverage,
interval and dropped-row checks as well.

When linking a historical result, state its required checkout and scope.
Historical runner names are provenance, not a request to rebuild retired code.
Keep the project's active README separate from archived progress narratives.

## Retain enough evidence within a bounded cache

The default project build-cache budget is **4 GiB**. Diagnostic bridge capture
defaults to **512 MiB**. These are different limits: one bounds retained build
files, the other bounds a particular capture. A larger experiment needs an
explicit reason and a retention plan; it does not change the defaults.

```console
python -m amiga_decomp --project your-game run prune_build_artifacts --dry-run
python -m amiga_decomp --project your-game run prune_build_artifacts --keep build/current-failure
```

Inspect the dry run when changing policy. The pruner only accepts manifest
build roots within the selected project; policy paths must include a `build`
component and cannot escape through `..`. It does not prune canonical
`captures/`, media or the framework's shared tools installation. Junctions and
symlinks are not followed. Busy files are skipped rather than treated as removed.

Candidates must meet the default 0.5 MiB threshold and disposable suffix/policy
rules. It removes older disposable files first, with configured canonical
prefixes retained later. Compressed evidence, reports, object/build dependencies
and executables are protected by default; project policy can explicitly mark
obsolete executable directories disposable. Live capture prefixes/processes
and `--keep` paths protect active work. Small/protected/busy files can leave the
cache above budget; the report states that rather than deleting authority.

`--max-gib` or `AMIGA_DECOMP_BUILD_MAX_GIB` explicitly changes a run's budget.
Do not raise it merely to avoid cleanup. Retain reusable RAM losslessly as
`.dat.gz` with its original byte hash and evidence report. Readers vary in
compressed-format support, so preserve the documented naming and use explicit
decompression in temporary storage when necessary.

Use temporary capture storage for passing comparisons, retaining reports and
the smallest failing case. The shared audio `CaptureWorkspace` supports scoped
temporary storage; it is a helper, not a universal automatic capture wrapper.
Run the project pruner after direct/manual emulator captures. Avoid accumulating
whole RAM snapshots for every passing frame when a hash and failure snapshot
answer the same comparison question.

## Extract reusable work without erasing the case study

Before promoting a tool, identify its assumptions: game addresses/table names,
fixed RAM sizes, frame offsets, ownership rules, scenario assertions and private
paths. Separate a hardware/parser primitive from the game's caller/acceptance
policy. Pass sizes/bases explicitly or obtain them from the selected manifest;
keep genuinely restricted format profiles stated rather than pretending they
support every Amiga configuration.

Shared tools must not import a project module implicitly or discover the game
from the working directory. The launcher supplies explicit project context and
configured project Python paths to children. A hardware observer can use a
manifest-selected game reader through a defined interface, while its parser and
transport remain shared. Preserve old stream identifiers when existing sealed
evidence depends on that ABI.

Move reusable explanations into the framework and replace active project
instructions with links. Keep address-bound findings and original result
reports in the project. Archive outdated project narratives with their dates
and required checkout; do not present their old commands/counts as current
capabilities. Update imports, launchers, paths, tests and links together.

## Validate and publish a connected batch

For a shared tool change, run affected unit checks and at least another project
configuration where memory/layout selection matters. Exercise the affected
original capture/analysis path when execution behavior changes. Use a temporary
manifest/fixture for configuration probes; do not invent a second game's result.

For documentation, check CLI arguments, relative links, output schemas and
platform limitations against current code. Run documented smoke commands where
bounded and appropriate. Do not rerun every historical mission or rewrite
sealed evidence to validate a prose edit. Preserve unrelated user settings and
recording hashes during a reorganization.

Commit complete, validated batches and push regularly. When a project contains
a native submodule, commit and push its changes first, then commit the updated
pointer in the framework. Framework methodology changes normally do not require
altering that native repository.

## Leave a current handoff

A useful handoff states the active objective and accepted scope, current branch
and repositories, completed results with evidence links, unresolved causal
questions, exact next experiment, required local tools and any live processes.
Separate historical backlog from the next authorized work. Include a user's
accepted waivers and deferred cleanup so future sessions do not reimpose them.

Prefer a small, current project entry point linked to detailed reports and this
manual. Revise the current result when new evidence changes it; preserve the
old measurement as history. Do not turn a method template into a completion
claim or list every retired experiment as a required new step.

Acceptance: another investigator can reproduce the result and understand its
limits; authority remains intact; shared guidance has one current home; caches
are bounded and the next meaningful investigation is explicit.
