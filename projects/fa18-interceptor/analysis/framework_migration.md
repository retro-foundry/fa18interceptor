# Framework/project separation (2026-10-11)

The framework owns project selection, shared capture/analysis tools and generic
Ghidra import/export. F/A-18 data, reconstruction, scripts, reports, local media,
recordings, Ghidra workspaces, caches and its pinned emulator fork now belong to
`projects/fa18-interceptor/`. The playable native repository was not changed.

The relocation baseline recorded 4,187 hashes, including tracked source/evidence,
sealed recording configs/input/metadata and user `.vscode` files. All original
analysis, patches, patterns, P-code, reconstruction fragments and checked sealed
recordings retain their bytes. The three protected historical files also retain
their hashes. The emulator submodule remains clean at
`ace4c3a9553e7005ed32c6fee8817126a53a8887`.

Shared tool validation:

- Eight framework tests pass with two separate temporary project manifests,
  propagated child context, a non-A500 memory map, isolated artifact pruning,
  rejected changed instruction bytes and checked config/snapshot relocation.
- A real five-frame power-on capture before and after relocation matches CPU
  registers, cycles, frame counters, complete chip/slow RAM, video, PCM and
  core options. Serialized state chunks match after substituting only the disk
  and ROM paths in `DSK0` and `ROM `; source recording bytes are not edited.
- Real original 32-frame PCM, voice, LED and DMA capture passes preservation and
  corruption rejection. All 48 original audio unit tests pass.
- Seven compact-frame tests and four artifact-policy tests pass; the Windows
  symlink-permission test is skipped.
- A real restored-menu trace contains 10,991 executed instructions. The shared
  pipeline validates 441 RAM instruction starts (1,746 bytes), imports them in
  Ghidra and exports 2,163 P-code operations plus 36 structural call targets.
  These structural results do not establish additional game semantics.
- The relocated disassembler decodes the known `C32D24` entry successfully.

Absolute paths in original UAE configs are retained for provenance. Runtime
copies point to relocated media. GUI replay derives only the config checksum
in copied version-8 snapshot headers, retaining serialized core state bytes.
Its local `build/engine9000-replay/e9k-debugger.exe` is absent, so complete GUI
replay was not rerun; the project keeps the corresponding host patch. Direct
core capture and the Ghidra pipeline were exercised with the available tools.

Detailed migration hashes and startup comparison results are in the ignored
project-local `build/framework-migration-validation.json`. Diagnostic captures
were bounded and the project artifact pruner was run afterwards.
