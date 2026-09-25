# Demonstration-flight `$C3B6A6` renderer path

Classification: **event-faithful bounded control-to-renderer execution**. This
is an observed reusable 3D component path during the built-in Demonstration
Flight; it does not assign an in-game name, prove a complete building, or
place the component on the global M-map.

The no-human-input demo holds the menu-selection key only on replay frames 60
through 64. A normal full-frame replay applies those events before a debugger
breakpoint is armed at `$C1CC70`. The first hit is at frame 117. A bounded
6,000-instruction trace then remains after all recorded input, so its stepped
portion does not alter the input sequence.

At trace index 681, `$C1CC70` reads descriptor `$C224D0` field `$C3B6A6`.
`$C1EF10` publishes `$C3B6AE`; `$C1F70E` loads that exact value into `A5`
for the `$C1F6F8` walker. The same bounded path enters `$C1F4AC` with
immutable source `$C3B720` and calls `$C2FF48` at indices 1,500, 2,455,
3,421, and 4,390 with `A5=$C3B6B0`.

```text
$C224D0 descriptor field $C3B6A6
  -> $C1EF10 cursor $C3B6AE
  -> $C1F6F8 walker A5=$C3B6AE
  -> $C1F4AC source $C3B720
  -> four $C2FF48 calls, context $C3B6B0
```

`$C3B720/$C3B6B0` remains the separately established local-triple and
face-topology component. This trace extends its scenario coverage to the demo
flight, rather than claiming that it is a named building or landmark.

Authority: `build/attract_demo_first_control_store_trace/{report.json,trace.jsonl,slow.bin}`,
[`run037 C3B6A6 path`](run037_c3b6a6_placement_to_face_context.md), and
[`C3B720/C3B6B0 component boundary`](c3b720_c3b6b0_static_component_boundary.md).
