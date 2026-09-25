; Byte-exact observed postflight record setup $C31D64-$C31D89.

                org     $C31D64

POSTFLIGHT_RECORD_BASE          equ     $C46184
POSTFLIGHT_RECORD_OFFSET        equ     $C458DE
POSTFLIGHT_STAGE_MARKER         equ     $C45954
POSTFLIGHT_SHARED_LONG          equ     $C456E6

initialize_c31d64_postflight_record:
                lea.l   POSTFLIGHT_RECORD_BASE.l,a4
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a4
                move.w  #$d,POSTFLIGHT_STAGE_MARKER.l
                move.l  #$fffff,POSTFLIGHT_SHARED_LONG.l
                cmpi.b  #$11,$62(a4)
                beq.b   $C31DA6
