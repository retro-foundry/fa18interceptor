# `$C1AE28` direct command-key dispatch table

Classification: **structural with byte-exact source**. Once the preceding
dispatcher context tests permit direct controls, this table compares `D0` to
raw Amiga key bytes and branches to the respective command handlers. The table
ends at `$C1AEE0`, where an alternate context route starts.

The table includes routes whose handlers have sealed-run evidence (eject,
gear, flare, chaff, ECM, HUD, target, Return via the later table) and routes
whose game behavior has not yet been isolated. `dispatch_direct_command_keys.asm`
records the routing without promoting untraced routes to behavioral claims.
