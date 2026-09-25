; Byte-exact observed entry prefix $C148E2-$C14903.
; It loads a word from the active record and conditionally normalizes the local
; copy; the later arithmetic is reconstructed in separate blocks.

                org     $C148E2

ACTIVE_RECORD_POINTER           equ     $C18210
RELATIVE_VALUE_NORMALIZE_PATH   equ     $C14904

initialize_c148e2_relative_value:
                link.w  a6,#-$a
                movem.l d2,-(sp)
                movea.l ACTIVE_RECORD_POINTER.l,a0
                move.w  $66(a0),d0
                move.w  d0,-$2(a6)
                cmpi.w  #$3840,d0
                blt.b   RELATIVE_VALUE_NORMALIZE_PATH
                subi.w  #$7080,-$2(a6)
