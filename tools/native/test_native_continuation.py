"""Reject altered later-mission replay claims using the actual earned recordings."""
import argparse
from copy import deepcopy
import json
from pathlib import Path
from unittest.mock import patch

from check_recorded_original_mission_trace import verified_update_mapping, verified_continuation, digest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('prefix','replay','runner','source-evidence','source-updates','trace-evidence','out'):
        parser.add_argument('--'+name, type=Path, required=True)
    args = parser.parse_args()
    original = json.loads((args.source_evidence/'report.json').read_text())
    mapping, updates = verified_update_mapping(args.source_updates, args.source_evidence/'driver.jsonl.gz', original)
    report_path = args.replay/'report.json'
    frozen = report_path.read_bytes()
    report = json.loads(frozen)
    def verify():
        return verified_continuation(args.prefix, args.replay, args.runner, original, mapping,
            args.source_updates/'update-consumed.fa18in', args.source_evidence/'driver.jsonl.gz',
            args.trace_evidence/'native.jsonl.gz')
    aligned, continued, prefix = verify()
    assert continued == report
    first,last = report['whole_successful_flight']['first'],report['whole_successful_flight']['last']
    alignment = report['whole_successful_flight']['execution_alignment']
    assert aligned[first] == alignment['first_native_update']
    assert aligned[last] == alignment['last_native_update']
    guards = {}
    for name in ('wrong_runner','changed_trace','changed_ram','incomplete_comparison',
                 'unverified_prefix','changed_origin','changed_controls','changed_anchor_input'):
        altered = deepcopy(report)
        bytes_path = changed = None
        if name == 'wrong_runner': altered['runner_sha256'] = '0'*64
        elif name == 'changed_trace': altered['native_trace_sha256'] = '0'*64
        elif name == 'changed_ram': altered['native_final_ram_sha256'] = '0'*64
        elif name == 'incomplete_comparison': altered['comparison_completed'] = False
        elif name == 'unverified_prefix': altered['native_prefix_execution_exact'] = False
        elif name == 'changed_origin': altered['input_segment']['first_actual_source_update'] += 1
        elif name == 'changed_controls':
            bytes_path = args.replay/'input.segment.fa18in'
            changed = bytes_path.read_bytes().replace(b' K ',b' X ',1)
            assert changed != bytes_path.read_bytes()
            altered['replay_input_sha256'] = altered['input_segment']['input_sha256'] = digest(changed)
        else:
            bytes_path = args.replay/'input.anchors'
            changed = bytes_path.read_bytes().replace(b'C10D8A',b'C10D8B',1)
            assert changed != bytes_path.read_bytes()
            altered['anchor_input_sha256'] = digest(changed)
        read_text, read_bytes = Path.read_text, Path.read_bytes
        def text(path,*positional,**keywords):
            if path == report_path: return json.dumps(altered)
            if path == bytes_path: return changed.decode('ascii').replace('\r\n','\n')
            return read_text(path,*positional,**keywords)
        def binary(path,*positional,**keywords):
            return changed if path == bytes_path else read_bytes(path,*positional,**keywords)
        with patch.object(Path,'read_text',text), patch.object(Path,'read_bytes',binary):
            try:
                verify()
            except AssertionError as error:
                guards[name] = dict(rejected=True,reason=str(error) or 'recorded continuation differs')
            else:
                raise AssertionError('Altered continuation accepted: '+name)
    assert report_path.read_bytes() == frozen
    result = dict(verified_prefix=prefix, first=first,last=last,native_first=aligned[first],native_last=aligned[last],
        complete_native_updates=len(set(aligned[i] for i in range(first,last+1))),
        replay_report_sha256=digest(frozen),mutation_rejections=guards,original_report_unchanged=True,
        scope='Actual earned ordinary mission-five prefix, exact remapped controls and source-defined event anchors. No original state or constructed qualification bytes initialize native gameplay.')
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2)+'\n')
    print(f'{len(guards)} altered continuation claims rejected; actual replay verified')


if __name__ == '__main__':
    main()
