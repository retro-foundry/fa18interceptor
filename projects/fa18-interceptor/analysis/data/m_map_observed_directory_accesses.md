# Observed M-map relative-offset directory accesses

Classification: **scenario-backed selector access inventory**.

Each row is an observed C2ADCE relative-offset read decoded from the paired slow-RAM snapshot. The alternate 64-byte stride is inferred from the byte-exact selector mode, but the frame-local mode word is not itself recorded here. Targets are packet-entry candidates, not terrain ownership or global world coordinates.

104 `$C2ADCE` visits resolve to 44 unique table slots across 2 static bases; 13 unique slots immediately reject at `$C2AEFC`.

| base | index | table slot | target | entry class | observations |
| --- | ---: | --- | --- | --- | ---: |
| `$C42CA8` | `$0036` | `$C42CDE` | `$C42E6A` | immediate_C2AEFC_reject | 2 |
| `$C42CA8` | `$0038` | `$C42CE0` | `$C42E52` | nonnegative_packet_entry_candidate | 2 |
| `$C42CA8` | `$003A` | `$C42CE2` | `$C42E52` | nonnegative_packet_entry_candidate | 2 |
| `$C42CA8` | `$003C` | `$C42CE4` | `$C42E52` | nonnegative_packet_entry_candidate | 2 |
| `$C42CA8` | `$0046` | `$C42CEE` | `$C42E6A` | immediate_C2AEFC_reject | 2 |
| `$C42CA8` | `$0048` | `$C42CF0` | `$C42DC4` | nonnegative_packet_entry_candidate | 4 |
| `$C42CA8` | `$004A` | `$C42CF2` | `$C42E1A` | nonnegative_packet_entry_candidate | 4 |
| `$C42CA8` | `$004C` | `$C42CF4` | `$C42E52` | nonnegative_packet_entry_candidate | 2 |
| `$C42CA8` | `$0056` | `$C42CFE` | `$C42E6A` | immediate_C2AEFC_reject | 2 |
| `$C42CA8` | `$0058` | `$C42D00` | `$C42DFC` | nonnegative_packet_entry_candidate | 4 |
| `$C42CA8` | `$005A` | `$C42D02` | `$C42D28` | nonnegative_packet_entry_candidate | 3 |
| `$C42CA8` | `$005C` | `$C42D04` | `$C42E52` | nonnegative_packet_entry_candidate | 2 |
| `$C42CA8` | `$0066` | `$C42D0E` | `$C42E6A` | immediate_C2AEFC_reject | 1 |
| `$C42CA8` | `$0068` | `$C42D10` | `$C42E6A` | immediate_C2AEFC_reject | 1 |
| `$C42CA8` | `$006A` | `$C42D12` | `$C42E3E` | nonnegative_packet_entry_candidate | 1 |
| `$C42CA8` | `$006C` | `$C42D14` | `$C42E52` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$03DA` | `$C43246` | `$C43992` | immediate_C2AEFC_reject | 2 |
| `$C42E6C` | `$03DC` | `$C43248` | `$C43992` | immediate_C2AEFC_reject | 3 |
| `$C42E6C` | `$03DE` | `$C4324A` | `$C43994` | nonnegative_packet_entry_candidate | 3 |
| `$C42E6C` | `$03E0` | `$C4324C` | `$C439A8` | nonnegative_packet_entry_candidate | 3 |
| `$C42E6C` | `$03E2` | `$C4324E` | `$C439CC` | nonnegative_packet_entry_candidate | 3 |
| `$C42E6C` | `$03E4` | `$C43250` | `$C439CC` | nonnegative_packet_entry_candidate | 1 |
| `$C42E6C` | `$041A` | `$C43286` | `$C439E4` | immediate_C2AEFC_reject | 2 |
| `$C42E6C` | `$041C` | `$C43288` | `$C439E6` | immediate_C2AEFC_reject | 3 |
| `$C42E6C` | `$041E` | `$C4328A` | `$C439E8` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$0420` | `$C4328C` | `$C43A78` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$0422` | `$C4328E` | `$C43B1A` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$0424` | `$C43290` | `$C43B1A` | nonnegative_packet_entry_candidate | 1 |
| `$C42E6C` | `$045A` | `$C432C6` | `$C43B32` | immediate_C2AEFC_reject | 2 |
| `$C42E6C` | `$045C` | `$C432C8` | `$C43B32` | immediate_C2AEFC_reject | 3 |
| `$C42E6C` | `$045E` | `$C432CA` | `$C43B34` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$0460` | `$C432CC` | `$C43BAC` | nonnegative_packet_entry_candidate | 5 |
| `$C42E6C` | `$0462` | `$C432CE` | `$C43CBE` | nonnegative_packet_entry_candidate | 4 |
| `$C42E6C` | `$0464` | `$C432D0` | `$C43D1A` | nonnegative_packet_entry_candidate | 1 |
| `$C42E6C` | `$049A` | `$C43306` | `$C43D34` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$049C` | `$C43308` | `$C43D84` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$049E` | `$C4330A` | `$C43E24` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$04A0` | `$C4330C` | `$C43ECA` | nonnegative_packet_entry_candidate | 4 |
| `$C42E6C` | `$04A2` | `$C4330E` | `$C43F5E` | nonnegative_packet_entry_candidate | 4 |
| `$C42E6C` | `$04DA` | `$C43346` | `$C44066` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$04DC` | `$C43348` | `$C4407A` | immediate_C2AEFC_reject | 2 |
| `$C42E6C` | `$04DE` | `$C4334A` | `$C4407C` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$04E0` | `$C4334C` | `$C4409E` | nonnegative_packet_entry_candidate | 2 |
| `$C42E6C` | `$04E2` | `$C4334E` | `$C440BA` | immediate_C2AEFC_reject | 2 |
