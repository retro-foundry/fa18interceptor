# Native command queue publication

`port/command_queue.c/.h` implements the complete original publication routine
at `$C1C23C-$C1C2B8` with ordinary C objects. It joins the shared command,
indexed, flight, view and context state in `fa18_command_input`.

The taken latch gates presses; releases do not claim it. A press claims the
latch even when the signed count is at least ten. The raw write index is
signed: values at least ten reset to zero, while negative values are retained.
The translated write index is signed and is neither advanced nor reset.
Count and raw index increments wrap as bytes. Every call clears all three
modifier bytes. A queued event retains its high word and replaces its whole
low word with the original table translation; gated events remain intact.

Signed indices can write adjacent globals. The 266-byte original neighboring
window starts at raw index -128 and ends at translated index +127. References
bind currently modeled bytes to their canonical native owners and the two
throttle words by value, respecting their original big-endian byte layout on
either host byte order. Other neighboring bytes and the 128-byte translation
table require original data from the asset owner. No values are invented.
Initialization imports this window into the shared owners and binds the
context's taken pointer to the queue. Owners must retain stable addresses.
Future ports of neighboring globals must bind those globals to their owner
in this initializer rather than maintain duplicate values.

The byte-of-value mechanism is now the independent `field_bytes.h` component,
also used by the two complete native startup range leaves. Queue slots retain
their existing byte/signed-word owners through an alias of `PortFieldByte`;
this core also supports supplied unsigned words and signed/unsigned longs.
Missing destinations return an explicit error, retaining preceding writes
and leaving the result unset. No address space or CPU state was introduced.
Extraction regression retains all 73,728 calls, 28 boundaries and every raw/
translated destination. See `native_startup_ranges.md` for reuse and limits.

Validation runs `python tools/recomp/check_native_command_queue.py` against
the sealed original demo state. Original source bytes are checked before any
call executes. The oracle uses the actual original instructions, without
child contracts, and compares every Chip/Slow RAM byte, the complete event and
independent canonical field mappings. The first 8,192 fixtures exercise taken,
release, signed counts, queue boundaries and CCR variations. Another 65,536
fixtures enumerate every raw byte and raw-index byte, distributing translated
indices across their entire range. Neighboring bytes are varied independently.

All 73,728 calls pass, covering 28/28 instruction boundaries, all 138 possible
raw destinations and all 256 translated destinations. The checkpoint is
`analysis/figures/native_command_queue_checkpoint.json`. GNU strict-warning and
MSVC contracts cover native word-byte aliasing, queue-global write order,
modifier clearing and selection -> real view action -> publication composition.
All six input CTests pass. The GNU contract has no CPU/bus/machine symbols;
the native MSVC game builds and its unchanged guard passes 423 files.

The component is linked into the native library but is not called by the
incomplete native game loop. Both native parent dispatch/reset/fault owners
and the full eject publication child now compose it; see
`native_command_parent.md`. Remaining audio/space/spawn/input callback and host
registration backends, original data loading and full runtime integration
remain unfinished. This proof does not complete the
emulation-free game objective.
