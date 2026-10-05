# Native startup checksum text and palette publisher

`port/postflight_text.c/.h` implements the complete `$C0F812` body on ordinary
native owners. Its actual `$C0F56A` formatter is `port/hex_field.c/.h`, and its
actual `$C17B96` child uses the existing native audio-selection graph.
`$C08F26` bootstrap is a required explicit child: its complete native graph
is still pending. Missing owners, imports or child completion fail; no
substitute bootstrap or next-stage body is supplied.

The publisher calls bootstrap before reading the dynamic palette pointer,
then copies 32 raw words from the original `$C08510` bank sequentially,
preserving forward-overlap effects. It clears the command mode shared with
indexed controls, sets the aircraft's shared pause/origin gate and its
transition byte to `$FF`, writes the supplied countdown to 3 and installs
the semantic `$C11446` callback identity.

The checksum references are read in original order against `$C560`, `$7E70`,
`$4DE8`. The first mismatch assigns marker `$31`; later mismatches OR `$32`
and `$34`. Each mismatch replaces the selected value, including zero. A
nonzero final value is formatted and selects `$C113E4`. A zero final value
starts menu audio at volume 50 and keeps `$C11446`, even if an earlier word
differed. Each value is captured before its marker write. The reference hook
historically named `PM_DELAY_TEXT` calls audio, not a wait.

The formatter preserves the entire signed-width-byte domain. Positive width
writes from `anchor+width` backward through `anchor+1`; anchor stays untouched.
Leading zeroes become spaces, retaining the final digit. Width zero and
negative widths normally perform no data access. Width `$80` skips conversion,
then decrements to `$7F` and scans up to 127 zero bytes from `anchor-127`.
No NUL or conventional zero-based formatting is added. Bounds failures retain
preceding writes rather than fabricating text.

The data binder resolves selector 97 through the existing Hunk-64 message
parser and binds that executable's actual mutable descriptor. The existing
text reader therefore sees publisher writes on the same owner. No string
is synthesized. The three checksum producers remain caller-owned references
and are not initialized to sentinels.

`display_palette_assets` now owns one flat 256-word mode bank in source order.
Its first 32 words supply the publisher seed; sixteen mode pointers select
the original reversed 16-word views of the same bank. There is no duplicate
seed to synchronize or two-dimensional-array pointer crossing. The disk
proof checks every initial/static/mode word, both contiguous seed halves and
32 descriptor bytes against sealed source. Writing that descriptor and
rereading selector 97 verifies shared mutable ownership. Raw high bits and
atomic truncated-resource rejection remain covered.

`stage_callback.h` gives publisher, tick and older followup controllers one
semantic callback type. The older headers declared incompatible types with
the same name. Existing names remain aliases of the correct source identities;
these enums are never CPU addresses. The contract includes both controller
headers and dispatches an actual native tick into the publisher with shared
countdown/callback storage. The older followup still needs full-body/shared
integration; this type correction does not complete its omitted clear branch.

Run `python tools/recomp/check_native_postflight_text.py`. There are 4,096
complete publisher invocations plus 4,096 complete formatter invocations,
covering **221/221** boundaries: 70 publisher, 41 formatter and 110 actual
audio children. The parent matches 13,256 ordered acknowledgements and all
4,096 bootstrap entry states. Every width byte is exercised with zero,
decimal, alphabetic, full-width and sign-bit value patterns.

Only `$C08F26` is contracted. Both runs receive explicit shared pointer, mode,
countdown, transition and checksum changes at that call; its volatile CPU
return registers are independently varied and ignored. This proves the
complete parent around that dependency, not native bootstrap completion.
Formatter and menu-audio children execute their real original instructions.
Audio tests also stress shared table/flag/mask changes at actual acknowledgements.

Every Chip/Slow RAM byte is compared except the original CPU ABI stack:
`$C7FD00..$C7FF00` for the parent, `$C7FD00..$C7FF10` for the standalone
formatter. The latter rewrites its by-value argument slots as local cursors;
native C uses its host ABI. The larger exclusion includes those slots, not
game data. Full RAM before bootstrap and at each acknowledgement, plus final
custom/interrupt state, also match. Guest-address packing and CPU state exist
only in validation, never the native implementation.

Strict GNU tests and symbol inspection, native MSVC game/test builds and three
publisher/tick/followup CTests pass. The original ADF proof and 8,192-call,
110-boundary audio-selection regression pass. The unchanged native guard
passes 463 files. See `native_postflight_text_checkpoint.json` and refreshed
palette/audio checkpoints under `analysis/figures` for hashes and exact scope.

The complete emulation-free game remains unfinished. Next implement and bind
full `$C08F26` bootstrap and its actual children with shared clear extents,
record and geometry owners, then complete loading/checksum producers and the
installed-stage graph. Bounded native main still does not call this startup
graph. This proof establishes neither native flight, complete scheduling nor
sample playback.
