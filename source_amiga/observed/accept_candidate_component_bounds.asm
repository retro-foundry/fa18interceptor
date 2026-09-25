; Byte-exact observed candidate component-bound acceptance $C26FC0-$C26FF5.
; It normalizes the remaining two relative component magnitudes, rejects either
; outside the current bound, then marks and tags the accepted candidate before
; class-$20 dispatch.

                org     $C26FC0

CANDIDATE_SCAN_STATUS           equ     $C4589F

accept_candidate_component_bounds:
                sub.l   d3,d6
                bge.b   $C26FC6
                neg.l   d6
                cmp.l   a1,d6
                bgt.w   $C26F00
                sub.l   d4,d7
                bge.b   $C26FD2
                neg.l   d7
                cmp.l   a1,d7
                bgt.w   $C26F00
                move.b  #1,CANDIDATE_SCAN_STATUS.l
                ori.w   #1,$2(a0,d0.w)
                move.b  $62(a0,d0.w),d7
                andi.b  #$F0,d7
                cmpi.b  #$20,d7
                beq.w   $C270AE
