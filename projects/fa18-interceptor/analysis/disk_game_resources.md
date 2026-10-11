# Game disk resource inventory

game-owned executable, graphics, text, configuration, and icon files; excludes AmigaOS support tree

## hunk_executable

| ADF path | Bytes | SHA-256 |
| --- | ---: | --- |
| `F-18 Interceptor` | 331,232 | `d7301e20958548f9b5f1ba744f8ff91fef476306c220eb886e7c90bac2802ffa` |

## workbench_icon_container

| ADF path | Bytes | SHA-256 |
| --- | ---: | --- |
| `F-18 Interceptor.info` | 7,756 | `e2e60b3ec2d527a768746ba03ae5d9354d26acd14917d263b929fbafbd482e5f` |

`F-18 Interceptor.info`: Amiga DiskObject magic `0xE310`, version 1. Workbench desktop icon container; not one of the game's pix/ ILBM resources and not evidence for a game renderer input.

## game_configuration_data

| ADF path | Bytes | SHA-256 |
| --- | ---: | --- |
| `config` | 78 | `f268dc8e925a020dadce6bae12b6158fed9020ec6d82729bbe816a0cd7d3d063` |

## ILBM_graphics

| ADF path | Bytes | SHA-256 |
| --- | ---: | --- |
| `pix/frnt5` | 988 | `7cf5300d75a10f0f7727506a91f2eb46968a625fdbbc9f13e13c88b26a52d012` |
| `pix/inst5` | 7,230 | `870329e081934311588fe90d28897a82c775a39e22f6f4525f163502a008889c` |
| `pix/splsh` | 23,224 | `d64e541d3e91808dd10a9e934216f1f28601f7e2c7c7a8db8b9d4845a47e1c70` |

## text_resource

| ADF path | Bytes | SHA-256 |
| --- | ---: | --- |
| `text/text201` | 568 | `f1ea05f80960ff2d7006fd6210f51dde104c78d1f10c4ecbb2776687e931f47f` |
| `text/textcpt` | 13,202 | `ca7a3d917c8c4a7b9145f8c98bcbee3741127a04470c481eb586f1d94e668ad7` |
| `text/textctl` | 14,704 | `0b7b6fdcf833113fc2ca02a89263a23b7eb890f414bb724414a15f143f5c2003` |
| `text/textegn` | 13,818 | `894561be833fb4e4c03a4461574a87d7d4f5c9a54d6a5cafab8807f4bdfa97a0` |
| `text/textegn2` | 7,818 | `4cbd5a5cd77d55290916afe8b6924f21f5c147e906e39052106fd346844f3fe8` |
| `text/textger` | 4,124 | `ab4352391a8f282bc93e9a6b1ae267e27d253b545c4e5daab99992484d7166b8` |
| `text/texti0a` | 2,044 | `e22ee6afadb6c7825456db3a8ea2640a7ddfae796779298a45b86b9b0f282565` |
| `text/texti0b` | 2,012 | `775bc74d2647b2ea8445ece027d199d926b0b484f2be89747aaf5241cc93816b` |
| `text/texti1` | 32,770 | `4cb22150b61d37935cb3a0c7ad0688b3a82fbc2cd59ffe143fa4e24290223a1e` |
| `text/texti2` | 32,786 | `9b90b44e6b185554f19b33cc3a6d60f34f4d984578f978d92b7a10c10a89f6a2` |
| `text/texti3` | 32,914 | `7e1cb735642c0f75c3fdc35625e05fe4590ed05539241f686f20e161f4126777` |
| `text/texti4` | 32,720 | `bdbfc8d95be0694f900f693148170e800d62139a38db244566415a5dc074653c` |
| `text/texti5` | 65,590 | `8e97c62afa76e4ae66c6f2e631d2efb49e774f45213d996349d28f880aeb5b1b` |
| `text/textply` | 3,676 | `a4c8b947d4ba0efa71d260f17045b3496fffb340909c5df133136c40a44a6745` |
| `text/texttre` | 3,462 | `86e1ce8814647948d4a6b245a46eb0eab35bf73cf68bf2c6ad9041219e0c7c0a` |
| `text/textwnd` | 20,000 | `518bf5f40c1228eb6ea1b9b98ac00836682682ffad0d4d49e1c0ca5ab213111f` |

The executable remains in the Hunk/code-data boundary inventory. ILBMs are detailed in `analysis/disk_graphics_assets.md`; text resources remain opaque data until their consumers are traced.
