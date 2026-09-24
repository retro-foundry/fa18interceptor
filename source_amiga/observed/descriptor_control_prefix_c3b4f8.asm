; Byte-exact static descriptor control prefix $C3B4F8-$C3B4FD.
; Observed as the +8 field selected from descriptor $C22708 in map-mode
; traces. A debugger-only C1F6F8 probe proves the first word selects the
; direct three-workspace-triple route; no unmodified sampled continuation
; reaches it at the walker, so preserve unresolved field meaning.
                org     $C3B4F8
descriptor_control_prefix_c3b4f8:
                dc.w    $42c0           ; first workspace-triple offset
                dc.w    $4086           ; direct triple-route control word
                dc.w    $c014           ; first relative-control offset word
