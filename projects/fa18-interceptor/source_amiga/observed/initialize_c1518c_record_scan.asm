; Byte-exact observed entry and loop head $C1518C-$C151CD.
; This helper scans up to 20 64-byte records; record semantics are unresolved.

                org     $C1518C

RECORD_SCAN_TABLE_END           equ     $C46201
RECORD_SCAN_COUNTER             equ     $C461BF
RECORD_SCAN_RENDERER_SETUP      equ     $C2F490
RECORD_SCAN_INDEX_PATH          equ     $C151CE
RECORD_SCAN_FINISH              equ     $C153DC

initialize_c1518c_record_scan:
                link.w  a6,#-$16
                moveq   #0,d0
                move.b  d0,-$3(a6)
                move.b  d0,-$1(a6)
                move.b  d0,-$2(a6)
                jsr     RECORD_SCAN_RENDERER_SETUP.l
                move.l  #RECORD_SCAN_TABLE_END,-$8(a6)
                move.l  #RECORD_SCAN_COUNTER,-$c(a6)
                movea.l -$c(a6),a0
                move.b  (a0),d0
                subq.b  #1,d0
                move.b  d0,(a0)
                clr.w   -$10(a6)
.loop_head:
                move.w  -$10(a6),d0
                cmpi.w  #$14,d0
                bge.w   RECORD_SCAN_FINISH
