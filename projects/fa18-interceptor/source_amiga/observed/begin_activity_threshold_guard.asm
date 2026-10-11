; Byte-exact observed activity-threshold guard prefix $C25488-$C2549F.
; The leading return closes the prior leaf; the adjacent helper exits through
; its tail for a negative prior snapshot, otherwise loads the current source
; and tests it against $7FFF.

                org     $C25488

ACTIVITY_PRIOR_FIRST            equ     $C45AFA
ACTIVITY_CURRENT_SOURCE         equ     $C45B0A

                rts

begin_activity_threshold_guard:
                tst.l   ACTIVITY_PRIOR_FIRST
                blt.b   $C254D2
                move.l  ACTIVITY_CURRENT_SOURCE,d1
                cmpi.l  #$7FFF,d1
                ble.b   $C254A8
