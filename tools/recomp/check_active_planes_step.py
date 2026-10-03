"""Compare resumable C bridge instructions with original opcodes.

Use --group audio for the voice iterator, its helpers and fading. Run from
any directory. Fixtures exercise all CCR combinations and word
boundaries; the sealed state and ROM are read only.
"""
import argparse
import subprocess
from check_record_region_probe import ROOT, build_oracle, default_bash
from port_info import instructions


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--cases", type=int, default=32,
                        help="fixtures per source instruction (at least 32 for every CCR)")
    parser.add_argument("--bash", default=default_bash())
    parser.add_argument("--bus", action="store_true",
                        help="include DMA bus contention in instruction timing fixtures")
    group_names = ("planes", "audio", "glyphs", "input", "page", "notify",
                   "command", "buffers", "polygon", "postflight", "followup", "faces", "regions", "map", "grid", "renderer", "sound_start", "screen_frame", "drawing", "cells", "startup", "number_field", "orientation", "tracking", "terrain_sort", "terrain_condition", "terrain_flags", "pixels", "interrupt_count", "view_controls", "record_rate", "matrix_pipeline", "flight_update", "marker_projection", "face_predicates")
    group_names += ("gauge", "workspace_records", "scene_transition", "placement_order", "template_placements", "scene_placements", "followup_placements", "post_input_tick")
    group_names += ("scene_bootstrap",)
    group_names += ("record_update_stage",)
    group_names += ("selector_origin", "update_sequence", "input_events", "command_dispatch")
    group_names += ("postflight_scheduler",)
    group_names += ("context_publication",)
    group_names += ("menu_transition", "menu_setup", "menu_cold", "menu_followup", "menu_outcome", "menu_return", "menu_context_finish", "postflight_completion", "postflight_messages", "postflight_file_callers")
    parser.add_argument("--group", choices=group_names + ("all",), default="planes")
    args = parser.parse_args()
    if args.cases <= 0:
        parser.error("--cases must be positive")
    groups = {
        "postflight_file_callers": ['C0F56A', 'C0EF08', 'C162E4', 'C1631C', 'C16386'],
        "postflight_messages": ['C0F4D8', 'C0F812', 'C11078', 'C110A4', 'C11350', 'C113E4', 'C1141E', 'C11446', 'C11478', 'C114D2', 'C1159E', 'C115BA', 'C1169A', 'C116B0', 'C116CE', 'C11738'],
        "postflight_completion": ['C11788', 'C11830', 'C11872', 'C118A0', 'C118E6', 'C118FC', 'C11934', 'C11958', 'C119D4', 'C1104C', 'C0F946', 'C0F974', 'C091E6'],
        "menu_context_finish": ['C10A24', 'C10C08', 'C10C68', 'C10AB2', 'C10AE6', 'C10B1E', 'C10CFE', 'C10D8A', 'C10DAE', 'C11A26', 'C11A50', 'C09192', 'C16D04', 'C25070'],
        "menu_return": ["C1064C","C108FE","C10900","C10970","C102D8","C0FB70","C0FBB6","C101FC","C10228","C10942","C109AC","C10302","C10BAE","C10362"],
        "menu_outcome": ["C104C2","C105F4","C1072E","C1078A","C105A6","C10626","C1075A","C108DA","C29368"],
        "menu_followup": ["C1029E","C10418","C10458","C10678","C1643A"],
        "menu_cold": ["C0FE36","C1017E","C10272","C103E4","C09120","C29490","C2949A","C09148","C10B90","C16406"],
        "menu_setup": ["C0FBE0","C17B96","C1082C","C11BB0","C24FA4"],
        "menu_transition": ["C0FCB4","C0FECE","C0FFE2","C1000A","C17C2A","C24E8A"],
        "context_publication": ["C1B7A6","C1BEE8","C1C214","C083A6","C09DD0"],
        "postflight_scheduler": ["C09E06","C09E98","C09EC4","C0A002","C0A12E","C0A15C",
                                 "C0A1E0","C0A2F0","C0A334","C0A364","C0A3EA"],
        "command_dispatch": ["C1AC28", "C1AD74"],
        "input_events": ["C16EAE", "C16BF2", "C16C56", "C13D34"],
        "update_sequence": ["C0EFD4", "C0F3C4", "C0D730"],
        "selector_origin": ["C29042"],
        "record_update_stage": ["C22C80", "C1C63E"],
        "scene_bootstrap": ["C1C860", "C08F26", "C0F920", "C0F992", "C090C2", "C090F2", "C0910C", "C0915A", "C0F4A6", "C11ACC"],
        "post_input_tick": ["C0F5F8"],
        "followup_placements": ["C1CCBC", "C1D0A4", "C1D0B6", "C25876"],
        "scene_placements": ["C1CB14", "C1CB26"],
        "planes": ["C2FD8C"],
        "template_placements": ["C1D10C", "C1D722"],
        "gauge": ["C30918"],
        "workspace_records": ["C1EBB0", "C1EC84"],
        "placement_order": ["C1E540", "C1EBB0", "C1EBC0", "C1EBE0", "C1EC3A", "C1EC84", "C1EC96", "C1ECD4", "C1ECFC"],
        "scene_transition": ["C0FAA4", "C0924A", "C09266", "C092A0", "C095C0", "C09620", "C0840E"],
        "audio": ["C4FFB0", "C4FFB4", "C50158", "C501E0", "C50212", "C24FE8"],
        "glyphs": ["C330FE", "C32806"],
        "input": ["C1715C", "C16F1C"],
        "page": ["C2F558"],
        "notify": ["C11B44"],
        "command": ["C17B08", "C17B2C", "C17EF2", "C3316A", "C3316E", "C33180", "C3318E", "C33186"],
        "buffers": ["C2FD22"],
        "polygon": ["C30466", "C304B2", "C305AA"],
        "postflight": ["C31226"],
        "followup": ["C0FA04"],
        "faces": ["C2005C"],
        "regions": ["C2B05A"],
        "map": ["C2AA9C", "C2AB34", "C2AB5A"],
        "grid": ["C279D0"],
        "renderer": ["C212B0", "C332BC", "C23CA6", "C2469E", "C246A0", "C247C0", "C248B2", "C24996",
                     "C091E0", "C091CE", "C091A8"],
        "sound_start": ["C17E4A", "C17CF6", "C17DAA", "C17C62", "C17D6E", "C18096", "C1803C", "C180FC",
                        "C50AB4", "C50B02"],
        "terrain_sort": ["C1E328", "C1E4A6", "C1D91A", "C1D974"],
        "terrain_condition": ["C09A78", "C09A98", "C09AB8"],
        "pixels": ["C2F5C0", "C2F5D4", "C2F5F4", "C2F60A", "C2F63A", "C2F64E", "C2F66E",
                   "C2F826", "C2F83A", "C2F844", "C2F84E", "C2F858", "C2F862", "C2F86C",
                   "C2F876", "C2F880", "C2F88A", "C2F894", "C2F89E", "C2F8A8", "C2F8B2",
                   "C2F8EA", "C2F904", "C2F91E", "C2F96C", "C2F986", "C2F9A0", "C2F9BA",
                   "C2F9D4", "C2F9EE", "C2FA08", "C2FA22"],
        "face_predicates": ["C1FB82", "C1FB8C", "C1FB9C", "C1FC42", "C2F490"],
        "marker_projection": ["C0DAEE", "C2EC90", "C2EC94", "C2EC9C", "C2ECA4", "C2F1C0", "C06C02"],
        "flight_update": ["C230B0", "C244E2", "C1C54E", "C254E8", "C122A2", "C1C2C8", "C2374C", "C2574A", "C25704", "C231A2", "C2559A", "C25864"],
        "matrix_pipeline": ["C2D99C", "C2D9BA", "C2DB18", "C2DEE0", "C2DAF2", "C2E370",
                            "C2E346", "C2E38E", "C2E3DE", "C2E5AC", "C2D970", "C258C8"],
        "view_controls": ["C12098", "C1B906", "C1BA86", "C08324", "C082B8", "C082B0"],
        "record_rate": ["C1C7F6"],
        "interrupt_count": ["C06132"],
        "terrain_flags": ["C1CA82"],
        "tracking": ["C123FA", "C25980", "C2564E"],
        "startup": ["C11312", "C11B0E", "C28722", "C287DA", "C28800", "C28AFE", "C28B34", "C28F16"],
        "number_field": ["C24E2C", "C24F76", "C25A08", "C0F56A"],
        "orientation": ["C2D954", "C2E47A", "C2E514", "C2E5F6", "C2E6DA"],
        "cells": ["C1D3F4", "C1D4E4", "C1D520", "C1D5D8"],
        "drawing": ["C2FF48", "C301F0", "C301F6", "C3040C", "C2FA78", "C2FA7E", "C2EE4A", "C2F0C6", "C2F0F4", "C2F128", "C2F156"],
        "screen_frame": ["C0D74A", "C0D752", "C0DAA0", "C0DAD0", "C0DAD4", "C0DADC", "C0DAE6",
                         "C2E758", "C2EA5A", "C2EAD0", "C2EB4C", "C2EBC2"],
    }
    entries = (entry for name in group_names for entry in groups[name]) \
        if args.group == "all" else iter(groups[args.group])
    addresses = {}
    # The delayed menu owner includes 66 unrecorded instructions behind its
    # original indexed jump. Use the audited complete byte scope for this group.
    import json
    menu_owners = json.loads((ROOT / "analysis/data/menu_transition_source_scope.json").read_text())["owners"]
    menu_owners.update(json.loads((ROOT / "analysis/data/menu_cold_source_scope.json").read_text())["owners"])
    menu_owners.update(json.loads((ROOT / "analysis/data/menu_followup_source_scope.json").read_text())["owners"])
    menu_owners.update(json.loads((ROOT / "analysis/data/menu_outcome_source_scope.json").read_text())["owners"])
    menu_owners.update(json.loads((ROOT / "analysis/data/menu_return_source_scope.json").read_text())["owners"])
    menu_owners.update(json.loads((ROOT / "analysis/data/menu_context_finish_source_scope.json").read_text())["owners"])
    menu_owners.update(json.loads((ROOT / "analysis/data/postflight_completion_source_scope.json").read_text())["owners"])
    menu_owners.update(json.loads((ROOT / "analysis/data/postflight_messages_source_scope.json").read_text())["owners"])
    menu_owners.update(json.loads((ROOT / "analysis/data/postflight_file_callers_source_scope.json").read_text())["owners"])
    for entry in entries:
        if entry in menu_owners:
            for pc in menu_owners[entry]["source_pcs"]: addresses.setdefault(pc,entry)
            continue
        source = instructions(entry)
        if not source:
            raise SystemExit(f"missing original instructions for {entry}")
        for line in source:
            pc = line.split(":")[0]
            addresses.setdefault(pc, entry)
    header = "static const struct { uint32_t pc; int (*step)(void); } step_oracle_cases[] = {\n"
    header += "".join(f"    {{0x{pc}u, glue_{entry}_step}},\n" for pc, entry in sorted(addresses.items()))
    header += "};\n"
    (ROOT / "build/recomp").mkdir(parents=True, exist_ok=True)
    header_path = ROOT / "build/recomp/step_oracle_cases.h"
    if not header_path.exists() or header_path.read_text() != header:
        header_path.write_text(header)
    executable = build_oracle("active_planes_step_oracle",
                             "tools/recomp/active_planes_step_oracle.c", args.bash)
    subprocess.run([str(executable), str(args.cases), args.group,
                    "bus" if args.bus else "cpu"], cwd=ROOT, check=True)


if __name__ == "__main__":
    main()
