; Byte-exact observed candidate-stream shifted-component comparison $C273D4-$C273ED.
; It loads a stream entry, derives an indexed word from the candidate record,
; shifts it by the local count, sign-extends it, and compares it to D3.

                org     $C273D4

compare_candidate_stream_shifted_component:
                movea.l (a4)+,a5
                move.w  2(a5),d7
                addi.w  #$A4,d7
                move.w  $2(a3,d7.w),d4
                move.w  -$5A(a6),d7
                asr.w   d7,d4
                ext.l   d4
                cmp.l   d4,d3
                blt.b   $C27410
