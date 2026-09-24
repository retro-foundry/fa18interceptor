; Byte-exact rejection epilogue $C2EE44-$C2EE49 for C2EE4A's linked frame.
                org     $C2EE44
reject_projected_segment_cleanup:
                unlk    a6
                moveq   #0,d0
                rts
