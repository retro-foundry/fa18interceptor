; Byte-exact observed active-record continuation $C14C72-$C14C93.
; It sign-extends record word +$4E, scales it by 256, biases it by $0708,
; stores the long in a frame local, and takes the nonnegative continuation.

                org     $C14C72

CURRENT_CONTROL_RECORD          equ     $C18210
RECORD_WORD_4E                  equ     $4E
RECORD_WORD_4E_BIAS             equ     $0708

derive_active_record_word4e_offset:
                movea.l CURRENT_CONTROL_RECORD.l,a0
                move.w  RECORD_WORD_4E(a0),d0
                ext.l   d0
                asl.l   #8,d0
                addi.l  #RECORD_WORD_4E_BIAS,d0
                move.l  d0,-$20(a6)
                cmpi.l  #RECORD_WORD_4E_BIAS,d0
                bge.w   $C14D1E
