"""Follow the original C2C392 flow, including all forty action-table arms.

Out-of-table producer domains remain unresolved; their original transfer is
retained at the live compatibility boundary rather than clamped or invented.
"""
import argparse
import json
import re
from audit_command_dispatch import ROOT, audit, source_decoder

MANIFEST = ROOT / 'analysis/data/record_autopilot_source_scope.json'

def inventory():
    _, decoder = source_decoder()
    targets = [(decoder.word(0xc2baf8 + 4*i) << 16) |
               decoder.word(0xc2bafa + 4*i) for i in range(40)]
    result = audit(('C2C392',), dynamic_targets={0xc2c46e: targets},
                   additional_cold_entries=('C2C392',))
    result['action_table_targets'] = [f'{target:06X}' for target in targets]
    result['limits'] = ['Forty original action arms, including cold jumps.',
                        'No complete bounds proof for arbitrary action bytes.',
                        'Unknown/modified targets retain original signed-byte transfer.']
    return result

def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--write', action='store_true')
    args = parser.parse_args()
    result = inventory()
    if args.write:
        MANIFEST.write_text(json.dumps(result, indent=2) + '\n')
    elif json.loads(MANIFEST.read_text()) != result:
        raise ValueError('record-autopilot original source changed')
    owner = result['owners']['C2C392']
    fixture = (ROOT / 'tools/recomp/record_autopilot_oracle.c').read_text()
    boundaries = re.search(r'source_boundaries\[\]=\{([^}]+)\}', fixture).group(1)
    pcs = [f'{int(pc, 16):06X}' for pc in re.findall(r'0x([0-9A-F]+)u', boundaries)]
    if pcs != owner['source_pcs']:
        raise ValueError('record-autopilot oracle does not cover the audited source owner')
    print(f"record autopilot: {result['unique_instruction_count']} source instructions, "
          f"{owner['additional_cold_instructions']} cold instructions, forty action targets")

if __name__ == '__main__':
    main()
