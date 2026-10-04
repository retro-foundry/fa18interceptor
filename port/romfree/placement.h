/* Verified original Hunk placement and Exec ABI vector identifiers.
 * Derived from analysis/data/romfree_hunk_layout.json and the original
 * cold-entry contracts. No asset payload, ROM code or captured RAM. */
#ifndef FA18_ROMFREE_PLACEMENT_H
#define FA18_ROMFREE_PLACEMENT_H
#include "../amiga/hunk_loader.h"
#include "../amiga/exec_bootstrap.h"
static const AmigaHunkPlacement fa18_placements[]={
    {0xC0DEB0u,41812u}, /* 0 CODE */
    {0xC07F38u,884u}, /* 1 DATA */
    {0xC18208u,10764u}, /* 2 BSS */
    {0xC077A0u,156u}, /* 3 CODE */
    {0xC08EB8u,2044u}, /* 4 CODE */
    {0xC1AC18u,5808u}, /* 5 CODE */
    {0xC082B0u,480u}, /* 6 CODE */
    {0xC06BF0u,28u}, /* 7 CODE */
    {0xC1C2C8u,6428u}, /* 8 CODE */
    {0xC1DBE8u,4428u}, /* 9 CODE */
    {0xC1ED38u,4016u}, /* 10 CODE */
    {0xC07840u,296u}, /* 11 CODE */
    {0xC096B8u,1816u}, /* 12 CODE */
    {0xC1FCE8u,4980u}, /* 13 CODE */
    {0xC21060u,4068u}, /* 14 CODE */
    {0xC0CF98u,1924u}, /* 15 CODE */
    {0xC22048u,3124u}, /* 16 CODE */
    {0xC22C80u,6664u}, /* 17 CODE */
    {0xC09DD0u,1636u}, /* 18 CODE */
    {0xC24688u,1832u}, /* 19 CODE */
    {0xC24DB0u,3256u}, /* 20 CODE */
    {0xC08490u,648u}, /* 21 CODE */
    {0xC25A68u,2940u}, /* 22 CODE */
    {0xC265E8u,2252u}, /* 23 CODE */
    {0xC26EB8u,2840u}, /* 24 CODE */
    {0xC279D0u,3408u}, /* 25 CODE */
    {0xC28720u,3772u}, /* 26 CODE */
    {0xC295E0u,2336u}, /* 27 CODE */
    {0xC29F00u,5308u}, /* 28 CODE */
    {0xC2B3C0u,1848u}, /* 29 CODE */
    {0xC2BAF8u,4520u}, /* 30 CODE */
    {0xC2CCA0u,1892u}, /* 31 CODE */
    {0xC2D408u,4944u}, /* 32 CODE */
    {0xC0D720u,1068u}, /* 33 CODE */
    {0xC2E758u,1304u}, /* 34 CODE */
    {0xC2EC70u,2076u}, /* 35 CODE */
    {0xC2F490u,4652u}, /* 36 CODE */
    {0xC306C0u,2664u}, /* 37 CODE */
    {0xC31128u,2008u}, /* 38 CODE */
    {0xC31900u,6488u}, /* 39 CODE */
    {0xC33258u,6132u}, /* 40 CODE */
    {0xC34A50u,2840u}, /* 41 CODE */
    {0xC35568u,3232u}, /* 42 CODE */
    {0xC36208u,2076u}, /* 43 CODE */
    {0xC36A28u,2032u}, /* 44 CODE */
    {0xC08718u,732u}, /* 45 CODE */
    {0xC37218u,1128u}, /* 46 CODE */
    {0xC37680u,780u}, /* 47 CODE */
    {0xC0DB50u,616u}, /* 48 CODE */
    {0xC37990u,1520u}, /* 49 CODE */
    {0xC37F80u,1132u}, /* 50 CODE */
    {0xC383F0u,3448u}, /* 51 CODE */
    {0xC39168u,3292u}, /* 52 CODE */
    {0xC39E48u,2576u}, /* 53 CODE */
    {0xC3A858u,2336u}, /* 54 CODE */
    {0xC3B178u,2020u}, /* 55 CODE */
    {0xC3B960u,1744u}, /* 56 CODE */
    {0xC3C030u,1712u}, /* 57 CODE */
    {0xC3C6E0u,2496u}, /* 58 CODE */
    {0xC3D0A0u,1520u}, /* 59 CODE */
    {0xC3D690u,256u}, /* 60 CODE */
    {0xC3D790u,880u}, /* 61 CODE */
    {0x012988u,464u}, /* 62 CODE */
    {0xC3DB00u,4604u}, /* 63 CODE */
    {0xC3ED00u,9264u}, /* 64 CODE */
    {0xC41130u,4448u}, /* 65 CODE */
    {0xC42290u,1856u}, /* 66 CODE */
    {0xC429D0u,728u}, /* 67 CODE */
    {0xC42CA8u,6232u}, /* 68 CODE */
    {0xC44500u,896u}, /* 69 CODE */
    {0xC44880u,3500u}, /* 70 CODE */
    {0xC45630u,11612u}, /* 71 CODE */
    {0xC48390u,25572u}, /* 72 CODE */
    {0xC4E778u,5808u}, /* 73 CODE */
    {0xC4FE28u,268u}, /* 74 CODE */
    {0xC4FF38u,412u}, /* 75 CODE */
    {0xC500D8u,428u}, /* 76 CODE */
    {0xC50288u,352u}, /* 77 CODE */
    {0xC06CB8u,48u}, /* 78 DATA */
    {0xC06D00u,12u}, /* 79 BSS */
    {0xC503E8u,1188u}, /* 80 CODE */
    {0xC0A438u,172u}, /* 81 DATA */
    {0xC014C8u,8u}, /* 82 BSS */
    {0xC50890u,740u}, /* 83 CODE */
    {0xC07288u,12u}, /* 84 DATA */
    {0xC015B8u,8u}, /* 85 BSS */
    {0xC50B78u,352u}, /* 86 CODE */
    {0xC015D8u,8u}, /* 87 CODE */
    {0xC07298u,12u}, /* 88 DATA */
    {0xC06698u,8u}, /* 89 BSS */
    {0xC06958u,8u}, /* 90 CODE */
    {0xC50CD8u,268u}, /* 91 DATA */
    {0xC072A8u,8u}, /* 92 BSS */
    {0xC074F0u,8u}, /* 93 CODE */
    {0xC074F8u,12u}, /* 94 DATA */
    {0xC07508u,8u}, /* 95 BSS */
    {0xC50DE8u,112u}, /* 96 CODE */
    {0xC07510u,8u}, /* 97 DATA */
    {0xC07518u,8u}, /* 98 BSS */
    {0xC50E58u,168u}, /* 99 CODE */
    {0xC07968u,12u}, /* 100 DATA */
    {0xC07520u,8u}, /* 101 BSS */
    {0xC07978u,8u}, /* 102 CODE */
    {0xC07980u,12u}, /* 103 DATA */
    {0xC07990u,8u}, /* 104 BSS */
    {0xC50F00u,720u}, /* 105 CODE */
    {0xC07998u,8u}, /* 106 DATA */
    {0xC079A0u,8u}, /* 107 BSS */
    {0xC511D0u,340u}, /* 108 CODE */
    {0xC079A8u,8u}, /* 109 DATA */
    {0xC079B0u,8u}, /* 110 BSS */
    {0xC51328u,132u}, /* 111 CODE */
    {0xC079B8u,8u}, /* 112 DATA */
    {0xC079C0u,8u}, /* 113 BSS */
    {0xC079C8u,8u}, /* 114 CODE */
    {0xC07B00u,12u}, /* 115 DATA */
    {0xC07B10u,8u}, /* 116 BSS */
    {0xC513B0u,1552u}, /* 117 CODE */
    {0xC07B18u,24u}, /* 118 DATA */
    {0xC07B30u,8u}, /* 119 BSS */
    {0xC519C0u,1016u}, /* 120 CODE */
    {0xC07B38u,8u}, /* 121 DATA */
    {0xC07B40u,8u}, /* 122 BSS */
    {0xC51DB8u,372u}, /* 123 CODE */
    {0xC07B48u,8u}, /* 124 DATA */
    {0xC07BB0u,20u}, /* 125 BSS */
    {0xC51F30u,652u}, /* 126 CODE */
    {0xC07B50u,8u}, /* 127 DATA */
    {0xC07B58u,8u}, /* 128 BSS */
    {0xC521C0u,144u}, /* 129 CODE */
    {0xC07BC8u,8u}, /* 130 DATA */
    {0xC07BD0u,8u}, /* 131 BSS */
    {0xC089F8u,44u}, /* 132 CODE */
    {0xC07BD8u,8u}, /* 133 DATA */
    {0xC07BE0u,8u}, /* 134 BSS */
    {0xC07BE8u,8u}, /* 135 CODE */
    {0xC08A28u,12u}, /* 136 DATA */
    {0xC52250u,128u}, /* 137 BSS */
    {0xC522D0u,112u}, /* 138 CODE */
    {0xC07BF0u,8u}, /* 139 DATA */
    {0xC08A38u,8u}, /* 140 BSS */
    {0xC08A40u,8u}, /* 141 CODE */
    {0xC08A48u,8u}, /* 142 DATA */
    {0xC52340u,488u}, /* 143 BSS */
    {0xC52528u,2180u}, /* 144 CODE */
    {0xC52DB0u,28u}, /* 145 DATA */
    {0xC08A50u,8u}, /* 146 BSS */
    {0xC0A778u,8u}, /* 147 CODE */
    {0xC0DDB8u,24u}, /* 148 DATA */
    {0xC0A780u,8u}, /* 149 BSS */
    {0xC52DD0u,48u}, /* 150 CODE */
    {0xC52E00u,200u}, /* 151 CODE */
    {0xC52EC8u,76u}, /* 152 CODE */
    {0xC52F18u,168u}, /* 153 CODE */
    {0xC52FC0u,76u}, /* 154 CODE */
    {0xC53010u,24u}, /* 155 CODE */
    {0xC0A788u,8u}, /* 156 DATA */
    {0xC0A7F0u,8u}, /* 157 BSS */
    {0xC53028u,176u}, /* 158 CODE */
    {0xC530D8u,224u}, /* 159 CODE */
    {0xC531B8u,28u}, /* 160 CODE */
    {0xC531D8u,36u}, /* 161 CODE */
    {0xC53200u,28u}, /* 162 CODE */
    {0xC53220u,64u}, /* 163 CODE */
    {0xC53260u,32u}, /* 164 CODE */
    {0xC53280u,32u}, /* 165 CODE */
    {0xC532A0u,924u}, /* 166 CODE */
    {0xC53640u,628u}, /* 167 CODE */
    {0xC538B8u,88u}, /* 168 DATA */
    {0xC53910u,148u}, /* 169 BSS */
    {0xC0A7F8u,8u}, /* 170 CODE */
    {0xC539A8u,340u}, /* 171 CODE */
    {0xC53B00u,448u}, /* 172 CODE */
    {0xC53CC0u,8u}, /* 173 CODE */
    {0xC53CC8u,28u}, /* 174 CODE */
    {0xC53CE8u,248u}, /* 175 CODE */
    {0xC53DE0u,8u}, /* 176 DATA */
    {0xC53DE8u,52u}, /* 177 CODE */
    {0xC53E20u,8u}, /* 178 DATA */
    {0xC53E28u,140u}, /* 179 CODE */
    {0xC53EB8u,8u}, /* 180 DATA */
    {0xC53EC0u,328u}, /* 181 CODE */
    {0xC54008u,8u}, /* 182 DATA */
    {0xC54010u,8u}, /* 183 DATA */
    {0xC54018u,24u}, /* 184 CODE */
};
static const AmigaLibraryVector fa18_exec_vectors[]={
    {6,0xFC231Cu},
    {12,0xFC2324u},
    {18,0xFC2328u},
    {24,0xFC2328u},
    {30,0xFC08E6u},
    {36,0xFC0E9Cu},
    {42,0xFC0EC2u},
    {48,0xFC1F74u},
    {54,0xFC0F1Cu},
    {60,0xFC0F66u},
    {66,0xFC100Au},
    {72,0xFC0B2Cu},
    {78,0xFC0C04u},
    {84,0xFC1528u},
    {90,0xFC15B2u},
    {96,0xFC0AFCu},
    {102,0xFC0B64u},
    {108,0xFC3012u},
    {114,0xFC236Au},
    {120,0xFC1428u},
    {126,0xFC1436u},
    {132,0xFC1F96u},
    {138,0xFC1F9Cu},
    {144,0xFC115Eu},
    {150,0xFC1184u},
    {156,0xFC11B0u},
    {162,0xFC11CAu},
    {168,0xFC1210u},
    {174,0xFC1250u},
    {180,0xFC135Cu},
    {186,0xFC16D8u},
    {192,0xFC1740u},
    {198,0xC06550u},
    {204,0xFC187Cu},
    {210,0xFC182Cu},
    {216,0xFC190Cu},
    {222,0xFC195Au},
    {228,0xFC19E8u},
    {234,0xFC15E8u},
    {240,0xFC1614u},
    {246,0xFC1624u},
    {252,0xFC163Cu},
    {258,0xFC164Au},
    {264,0xFC165Au},
    {270,0xFC1670u},
    {276,0xFC1696u},
    {282,0xFC1C84u},
    {288,0xFC1D30u},
    {294,0xFC1DB0u},
    {300,0xFC1E04u},
    {306,0xFC1E5Eu},
    {312,0xFC1E54u},
    {318,0xFC1F0Cu},
    {324,0xFC1E84u},
    {330,0xFC2000u},
    {336,0xFC2038u},
    {342,0xFC1FCAu},
    {348,0xFC1FF0u},
    {354,0xFC1B54u},
    {360,0xFC1B6Cu},
    {366,0xFC1B70u},
    {372,0xFC1BEAu},
    {378,0xFC1C18u},
    {384,0xFC1C32u},
    {390,0xFC1C5Au},
    {396,0xFC1448u},
    {402,0xC06582u},
    {408,0xFC146Cu},
    {414,0xC0656Eu},
    {420,0xFC14B6u},
    {426,0xFC14D4u},
    {432,0xFC0690u},
    {438,0xC0658Cu},
    {444,0xC06564u},
    {450,0xC06578u},
    {456,0xFC0718u},
    {462,0xFC0706u},
    {468,0xFC078Au},
    {474,0xFC072Eu},
    {480,0xFC07A6u},
    {486,0xFC1C64u},
    {492,0xFC1C6Cu},
    {498,0xFC1C70u},
    {504,0xFC2234u},
    {510,0xFC223Eu},
    {516,0xFC226Au},
    {522,0xFC2124u},
    {528,0xFC117Cu},
    {534,0xFC1856u},
    {540,0xFC2D98u},
    {546,0xFC2DAEu},
    {552,0xC0655Au},
    {558,0xFC2DD0u},
    {564,0xFC2DF0u},
    {570,0xFC2E40u},
    {576,0xFC2EA4u},
    {582,0xFC2ED4u},
    {588,0xFC2F4Au},
    {594,0xFC2F70u},
    {600,0xFC2F60u},
    {606,0xFC2F6Cu},
    {612,0xFC0A78u},
    {618,0xFC1A26u},
    {624,0xFC2F80u},
    {630,0xFC2F7Cu},
};
#endif
