# Vroom recompilation case

Vroom (1991, Lankhor) is the real-game case imported from `amiga-recomp`
commit `0f71e52134ba64fa74975b6bbe75b19830b225c4`. Its loader mappings,
captured register/entry state and observed targets are under [recomp/](recomp/).
The method lives in the [shared recomp-first guide](../../docs/recomp/porting_game.md).

[project.json](project.json) selects this project's manifest and local paths.
The emulator configuration/ROM installation is user-supplied; no new original
Engine9000 recording is claimed by this import. Local copied media/snapshot
identities and their source derivation are recorded in `local/imported-assets.json`.

From the framework root, after building the shared tools:

```console
python -m amiga_decomp --project vroom run recompile analyze
python -m amiga_decomp --project vroom run recompile recompile --output build/recomp-game
python -m amiga_decomp --project vroom run recompile analyze recomp/vroom.toml
```

`recomp/vroom.toml` describes explicit ADF loader reads and a native capture at
the decrunched entry. Its output is `build/vroom-decrunched.bin` plus chipset/CPU
sidecars. `recomp/vroom-game.toml` uses the preserved `local/` copy, keeps the disk
attached and records observed targets. The snapshot contains 512 KiB of captured
bytes; the declared machine provides 1 MiB. That is the imported case's setup,
not an automatic original-memory qualification.

[Imported case history](analysis/RECOMP_HISTORY.md) retains the upstream results
and Vroom-specific hook. Counts and original-output claims there refer to that
checkout. Additional dynamically loaded code, timing, sound and full-game
original parity require their own qualification; this integration does not
declare Vroom complete. Generated code/builds and local media remain ignored.
