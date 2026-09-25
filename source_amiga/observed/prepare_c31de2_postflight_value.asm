; Byte-exact observed common postflight setup $C31DE2-$C31E05.

                org     $C31DE2

POSTFLIGHT_TEXT_BUFFER          equ     $C457FA
POSTFLIGHT_RECORD_BASE          equ     $C46184
POSTFLIGHT_RECORD_OFFSET        equ     $C458DE
POSTFLIGHT_MODE_WORD            equ     $C45946

prepare_c31de2_postflight_value:
                lea.l   POSTFLIGHT_TEXT_BUFFER.l,a2
                lea.l   POSTFLIGHT_RECORD_BASE.l,a0
                adda.w  POSTFLIGHT_RECORD_OFFSET.l,a0
                move.w  $56(a0),d0
                move.w  d0,d1
                asr.w   #3,d1
                add.w   d1,d0
                move.w  POSTFLIGHT_MODE_WORD.l,d6
                bge.b   $C31E08
