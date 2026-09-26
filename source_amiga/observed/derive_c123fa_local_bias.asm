; Byte-exact observed coordinate-update continuation $C12F46-$C12F65.
; It derives local -$A as $0310 minus twice and then one quarter of local
; -$2, before testing the signed-byte selector at local -$11.

                org     $C12F46

derive_c123fa_local_bias:
                move.w  -$2(a6),d0
                dc.w    $E340 ; asl.w #1,d0; retain original opcode
                move.w  #$0310,d1
                sub.w   d0,d1
                move.w  -$2(a6),d0
                asr.w   #2,d0
                sub.w   d0,d1
                move.w  d1,-$a(a6)
                cmpi.b  #$FE,-$11(a6)
                bne.b   $C12FA8
