# Framework instructions

Read `README.md` and the selected project's `AGENTS.md` before work.
The user's latest instructions override historical plans and proof notes.

- Shared Python tools, Amiga hardware support and Ghidra integration belong at
  framework level. Game addresses, names, routines, data, media, reports and
  game-specific emulator forks belong under `projects/<id>/`.
  Reusable Engine9000 observer support belongs under `tools/`.
- Select projects explicitly through `python -m amiga_decomp --project <id>`.
  Do not add a default game or dependencies on another project's layout.
- Original bytes and observed execution remain authoritative. Preserve calling
  context, addresses and hardware side effects; distinguish semantic evidence
  from structural disassembly and historical qualification.
- Preserve sealed recordings and unrelated user settings, including `.vscode/`.
  Relocate absolute asset paths only in derived runtime configs; retain original
  config hashes and record the derived hash separately.
- Keep binary captures in project-local ignored storage, use bounded windows,
  retain reusable RAM compressed and preserve reports/failing cases.
- Retain the 4 GiB artifact budget and the 512 MiB diagnostic capture default.
  Run the selected project's pruner after direct emulator captures.
- Validate shared tools with more than one project configuration and exercise
  affected original capture/analysis paths. Commit validated batches and push
  regularly as requested by the user.
