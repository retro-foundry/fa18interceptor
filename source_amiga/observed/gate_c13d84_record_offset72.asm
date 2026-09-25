; Byte-exact observed C13D84 continuation $C141AC-$C141ED.
; It gates on current-record +$72 and a local flag, subtracts a fixed signed
; term from that field, then prepares a bounded local comparison value.

                org     $C141AC

CURRENT_CONTROL_RECORD          equ     $C18210
RECORD_OFFSET_LONG_72           equ     $72

gate_c13d84_record_offset72:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                tst.l   RECORD_OFFSET_LONG_72(a0)
                beq.w   $C14600
                movea.l -$30(a6),a0
                move.w  (a0),d0
                btst    #3,d0
                beq.w   $C142A6
                move.w  #$600,d0
                move.w  d0,-$26(a6)
                ext.l   d0
                movea.l CURRENT_CONTROL_RECORD.l,a0
                sub.l   d0,RECORD_OFFSET_LONG_72(a0)
                move.w  #$3A98,d0
                sub.w   -$1a(a6),d0
                move.w  d0,-$16(a6)
                cmpi.w  #$4650,d0
                ble.b   $C141F4
