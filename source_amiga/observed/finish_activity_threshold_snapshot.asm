; Byte-exact observed activity-threshold helper tail $C254CC-$C254E7.
; It stores the calculated threshold word, snapshots the two activity source
; longwords into their prior-value slots, and returns.

                org     $C254CC

ACTIVITY_THRESHOLD_WORD         equ     $C45AE6
ACTIVITY_SOURCE_FIRST           equ     $C45AF2
ACTIVITY_PRIOR_FIRST            equ     $C45AFA
ACTIVITY_SOURCE_SECOND          equ     $C45AF6
ACTIVITY_PRIOR_SECOND           equ     $C45AFE

finish_activity_threshold_snapshot:
                move.w  d0,ACTIVITY_THRESHOLD_WORD
                move.l  ACTIVITY_SOURCE_FIRST,ACTIVITY_PRIOR_FIRST
                move.l  ACTIVITY_SOURCE_SECOND,ACTIVITY_PRIOR_SECOND
                rts
