; Byte-exact observed entry prefix $C11BFC-$C11C2B.
; Called by the parent postflight sequence at $C0F12C.  The later conditional
; body is intentionally not reconstructed here.

                org     $C11BFC

POSTFLIGHT_STATE_WORD           equ     $C45AE0
POSTFLIGHT_CONTEXT_POINTER      equ     $C45B50
POSTFLIGHT_FLAGS                equ     $C458CC
POSTFLIGHT_ALTERNATE_PATH       equ     $C11D7C

initialize_c11bfc_postflight_state:
                link.w  a6,#-$c
                clr.b   -$8(a6)
                move.w  POSTFLIGHT_STATE_WORD.l,d0
                move.w  d0,-$4(a6)
                andi.w  #$ff,d0
                move.l  POSTFLIGHT_CONTEXT_POINTER.l,-$c(a6)
                move.w  POSTFLIGHT_FLAGS.l,d1
                move.w  d0,-$6(a6)
                btst    #0,d1
                bne.w   POSTFLIGHT_ALTERNATE_PATH
