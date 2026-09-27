; Byte-exact normal-layout map control-stream selector $C2ABDE-$C2AC18.
; D3/D4 arrive from the shared coordinate setup at $C2ABBC.

                org     $C2ABDE

select_normal_map_packet_control_stream:
                moveq   #$a,d5
                lsr.w   d5,d3
                lsr.w   d5,d4
                andi.w  #3,d3
                andi.w  #3,d4
                subq.w  #3,d3
                neg.w   d3
                subq.w  #3,d4
                neg.w   d4
                add.w   d4,d4
                add.w   d4,d4
                add.w   d3,d4
                cmpi.l  #$5000,-$28(a6)
                bgt.b   .far_bank
                lea     $C2A032(pc),a0
                add.w   d4,d4
                add.w   d4,d4
                bra.b   .stream_ready
.far_bank:
                lea     $C29F32(pc),a0
                asl.w   #4,d4
.stream_ready:
                adda.w  d4,a0
                bra.w   $C2AD00
