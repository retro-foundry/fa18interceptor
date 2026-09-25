; Byte-exact observed basic block $C14904-$C14921.

                org     $C14904

ACTIVE_RECORD_POINTER           equ     $C18210

prepare_c14904_relative_components:
                move.w  -$2(a6),d0
                asr.w   #1,d0
                movea.l ACTIVE_RECORD_POINTER.l,a0
                move.w  $6a(a0),d1
                move.w  d0,-$2(a6)
                move.w  d1,-$4(a6)
                cmpi.w  #$3840,d1
                blt.b   $C1492C
