# Run062 failure-callback setup

Classification: **scenario-backed callback-state setup with static tail**.

The sealed run062 GUI-frame-2,200 checkpoint reaches `$C11788` at core frame
19. Its first dynamic branch takes `$C11792-$C117B2`, clears bits in
`$C458CC`, and calls `$C092A0`; that helper enters `$C0840E` and begins a
bounded runtime-work-buffer reset. The 12,000-instruction no-input trace is
retained at `build/run062_c11788_to_failure_callback_long_trace/`.

The byte-exact continuation after that helper's return is:

```text
$C1180A  $C45AD6 := $0005
$C11812  $C45795 := 0
$C11818  callback slot $C1820C := $C118A0
```

The slot sampler independently observes `$C1820C` change from `$C11788` to
`$C118A0` at core frame 23; `$C118A0` is entered at frame 24 and queues
run062 selector `$0063` on the next scheduling interval. This joins the
failure message producer to a preceding callback-reset state.

It does not identify the event that originally selected `$C11788`, explain the
reset helper's game role, or establish a failure/collision predicate.
