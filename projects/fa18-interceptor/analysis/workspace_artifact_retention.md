# Workspace artifact retention

User request, 2026-10-07: clear the nearly 60 GB directory and stop generated
output accumulating. Inventory measured 60.501 GiB total, 58.111 GiB in build.
Raw native RAM alone occupied 27.581 GiB; CMake validation RAM occupied another
4.220 GiB. Old pixel/index streams and 309 separately linked GNU executables
accounted for most remaining generated storage.

The existing pruner had no callers and its eight-MiB minimum excluded every
one-MiB RAM snapshot. It now defaults to a four-GiB build budget and half-MiB
minimum, recognizes diagnostic file formats, and can evict obsolete GNU probe
executables. Active CMake outputs, compiler objects, fetched dependencies,
playable reference runners, logs/reports and compressed original RAM remain
protected. Explicit `--keep` protects current input/output. Only repository
build roots are accepted; traversal skips symlinks and Windows reparse points.
Busy files are skipped. Protected storage can exceed the budget; the tool
reports that instead of deleting source or dependencies.

Cleanup is connected to native CMake post-build (including Visual Studio),
the GNU reference build, the native build script and a CTest cleanup fixture.
The native artifact-policy test covers small captures, old probes, protected
files, live temporary captures, foreign-directory rejection, dry runs and
failure-only retention. Three tests pass; the symlink-creation test is skipped
on this Windows host because creating symlinks is unavailable.

The mode and postflight comparison tools now allocate temporary capture
workspaces. Passing checks leave logs/JSON and no raw RAM; an oracle mismatch
copies only that before/after/source case. `--keep-captures` is a deliberate
debug choice. The independent-window checker similarly keeps only its first
failed boundary, reads original `.dat.gz`, and requests entry-only native
captures. This cuts its transient output by two thirds. Manual native ranges
default to 512 MiB; oversized requests fail before creating files unless an
explicit larger diagnostic budget is supplied.

Before pruning, 586 original takeoff/later boundaries were compressed and
verified by SHA256: 586 MiB became 204.9 MiB. The manifest is retained at
`build/native-flight/source-cache-manifest.json`. These are comparison-only
original artifacts; native behavior still starts from its normal disk/input.
Sealed `captures/`, ADF/ROM, source and IDE settings were untouched. The older
`port/build/_deps/sdl2-src` is still used by the active CMake cache and remains.

After rebuilding and exercising affected paths, the workspace measures
3.719 GiB and build 1.329 GiB: 56.782 GiB reclaimed relative to initial size.
Size/removal reports are `build/workspace-size-before.json`,
`build/workspace-size-after.json` and `build/workspace-cleanup*.log`.
Temporary later/takeoff comparisons reproduce the established 83/363 and
223/223 drawing results, with all 9,376 complete record cores matching after
the separate startup fix. Passing takeoff retains zero RAM; later failure
retains one one-MiB boundary. Free Flight callback and controlled postflight
checks retain zero raw RAM and pass 66 input intervals/62 bodies in total.
Eight affected runtime CTests plus automatic cleanup pass; policy and cleanup
CTest pass separately. Debug/Release builds pass and public Release is updated.

For future direct emulator diagnostics, capture a bounded range, compress
necessary reusable original checkpoints, keep the report and run the pruner.
Do not restore large passing dumps merely because historical notes name them.
The complete-port goal remains unfinished; this batch changes diagnostic
retention and removes generated storage, not gameplay acceptance policy.
