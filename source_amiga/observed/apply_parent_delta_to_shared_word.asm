; Byte-exact $C14876-$C148A1 helper reached by the isolated joystick-state packet.
; Its caller supplies a word at the measured caller-frame-relative location.

                org     $C14876

SHARED_CONTROL_RECORD_PTR      equ $C18210
SHARED_RECORD_WORD_26          equ $26
CALLER_FRAME_INPUT_WORD        equ $0A
INPUT_SCALE_SHIFT              equ 3

apply_parent_delta_to_shared_word:
                link.w  a6,#-2
                movea.l SHARED_CONTROL_RECORD_PTR.l,a0
                move.w  SHARED_RECORD_WORD_26(a0),d0
                ; VASM emits the shorter (A7) form; preserve MOVE.W D0,0(A7).
                dc.w    $3F40,$0000
                sub.w   CALLER_FRAME_INPUT_WORD(a6),d0
                asr.w   #INPUT_SCALE_SHIFT,d0
                ; Preserve MOVE.W 0(A7),D1 for the same reason.
                dc.w    $322F,$0000
                sub.w   d0,d1
                movea.l SHARED_CONTROL_RECORD_PTR.l,a0
                move.w  d1,SHARED_RECORD_WORD_26(a0)
                unlk    a6
                rts
