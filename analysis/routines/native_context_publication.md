# Native context/view publication

`port/native_context_publication.c/.h` implements complete `$C1BEE8`,
`$C1BA86` and `$C1B906`, including actual native maximum zoom, cockpit redraw
and command queue publication. The authority is the sealed original instruction
graph; `port/game/context_publication.c` and `view_commands.c` corroborate it.

Record publication sets the actual viewed aircraft reference after original
word scaling/wrap. The target index (`$C458DC`) is separate from the post-flight
target offset (`$C458C2`). Only references resolving to the supplied sixteen
records are supported; unsupported references fail after preceding stores,
without choosing a substitute record. View heading reads the same bank geometry.
Origin mode consumes the shared context return-mode byte. Class-$30 publication
retains its nested redraw/queue call and the second queue tail. Queue gating and
negative indices decide any second write, including writes to neighboring owners.

Zero-view preparation calls the existing view body directly, without synthesizing
an input command or emitting a request bit. Redraw resets the whole carried axis
before mode/queue indices replace its byte/word. The queue preserves that value
on gated paths and retains signed indexing and live write order. The existing
event-only queue API delegates to the same implementation with no axis output.

Post-flight's `publication` binding invokes the actual owner and checks shared
record, origin, sequence, detail, refresh, heading and marker pointers. Its
parent-proof `ops` seam remains available when publication is unbound. Production
must bind the actual owner; the current bootstrap constructor still needs complete
canonical state/assets and runtime integration. No CPU, bus, guest memory, ROM or
captured state is used by the production implementation.

`python tools/recomp/check_native_context_publication.py` passes 24,576 complete
calls at all 122/122 source boundaries with no child contracts. It executes actual
zoom/redraw/queue instructions, compares all game RAM except the CPU ABI stack
`C7FD00..C7FF00`, and independently checks typed records, viewed identity, target,
selection marker, heading, event and carried axis. Fixtures cover every mode byte,
signed counts and every queue index, context/noncontext routes, class-$30 records,
retained/cleared redraw state and indices displaced by plus/minus 128 before
word scaling. The mode-zero predicate excludes the impossible pre-redraw mode-three
arm; original state and predicate bytes are sealed.

`python tools/recomp/check_native_postflight_publication.py` passes 90,112 calls
at all 481/481 reachable source boundaries with no child contracts. All eleven
post-flight/readiness/restoration entries execute actual publication and its
children when reached. Additional unreachable branches follow the unchanged
class test and explicit mode-zero stores; the original zoom/redraw graphs do not
change either input. Canonical queue bindings retain neighboring post-flight
field writes; typed view fields are checked directly against source state. The
composed native test also makes queue translation write phase three through the
shared field and verifies that the parent rereads it before publishing an outcome.

MSVC Release game and affected targets, strict GNU compilation, eight affected
CTests and the unchanged 507-file native guard pass. Checkpoints are
`analysis/figures/native_context_publication_state_checkpoint.json` and
`analysis/figures/native_postflight_publication_checkpoint.json`. Periodic/dispatch,
lower flight/render/audio owners, complete original-state construction and native
main integration remain open.
