# Loaded code, depackers and overlays

[Recompiler index](README.md) · [Recomp-first workflow](porting_game.md)

Identify the executed byte view before translating. A disk/file can contain a
depacker, compressed payload, resources and trackloader metadata rather than a
single resident program. Short discovery or invalid instructions are reasons
to inspect the loading path, not to invent a decoder or widen code ranges blindly.

## Resident snapshot route

Capture a repeatable boundary after the needed loading/depacking, preferably
with a known update/interrupt phase. Preserve RAM, CPU registers/flags/stack,
chipset/DMA/interrupt state, disk identity and the capture's derivation.
Use [framework captures](../decompilation/02-capture-and-replay.md) and verified
[placements](../decompilation/03-memory-and-hunks.md) as original evidence.

The imported runtime's chipset serializer writes `ARCS` version 1; it is not
compatible with an Engine9000/UAE `ASF` savestate. The `[input].chipset_state`
field currently reads that native format. There is no general original-core
state converter. A RAM dump can feed static discovery but does not by itself
resume the original machine correctly.

State exactly where a snapshot-start executable begins. Calls to OS/ROM,
resource loading or overlays can remain after that boundary; trace them rather
than assuming takeover eliminates every dependency.

## Recompiled loader route

Translate the original loader/depacker, let it create resident bytes, and capture
the resulting state. `[capture]` forces a boundary at its trigger and can retain
memory, CPU metadata and native chipset state. That is derived evidence from
this runtime; qualify it against the original before treating it as an oracle.

`arecomp bootblock DISK --disasm` recognizes particular boot-block read patterns
and prints candidate `[[adf.load]]` mappings. Check disassembly, actual reads and
loaded bytes. The ADF loader accepts explicit disk offset/length/address mappings;
it does not universally solve trackloaders/protection. Runtime disk DMA supports
the imported standard disk path, with scope established by tests and case evidence.

## Offline depacking route

When the format and a reliable depacker are known, preserve input/output hashes
and tool/version/parameters. Treat output as a derived authority. The framework
does not currently contain a general archive/disk extraction or depacker library;
provide the required tool rather than guessing a transformation.

## Changing code versions

When execution writes/loads code, capture the full written range and consumers,
not just an address. Recompiled blocks describe a specific byte version; the
runtime invalidates overlapping translations on writes. Development interpretation
can keep investigation moving, but is an explicit remaining dependency.

Use run logs and `tools/converge.py` to locate candidate entries. A nonzero
window is only a screening heuristic; it does not prove instruction boundaries.
Separate static target discovery from observed dispatch. Additional captures or
version-specific translations are needed where an initial snapshot lacks code.
Keep unresolved versions in the report and qualify each exercised loading path.
