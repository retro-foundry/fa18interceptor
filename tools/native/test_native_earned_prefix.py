"""Reject altered claims about an actual native earned-save prefix."""
import argparse
import copy
import json
from pathlib import Path
from unittest.mock import patch

from check_recorded_original_mission_trace import digest, verified_native_prefix, verify_prefix_execution


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--prefix', type=Path, required=True)
    parser.add_argument('--runner', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--comparison', type=Path, help='also verify the retained continuation prefix and reject altered menu boundaries')
    args = parser.parse_args()
    report_path = args.prefix / 'report.json'
    before = report_path.read_bytes()
    original = json.loads(before)
    saved, evidence = verified_native_prefix(args.prefix, args.runner)
    assert digest(saved) == original['earned_save_sha256']
    mutations = {}
    for name in ('unearned', 'seeded', 'wrong_runner', 'changed_capture',
                 'changed_controls', 'counterfeit_save', 'wrong_menu', 'reset'):
        altered = copy.deepcopy(original)
        if name == 'unearned':
            altered['qualification_accepted'] = False
        elif name == 'seeded':
            altered['flight_state_seeded'] = True
        elif name == 'wrong_runner':
            altered['runner_sha256'] = '0' * 64
        elif name == 'changed_capture':
            altered['complete_native_artifacts']['dat_sha256'] = '0' * 64
        elif name == 'changed_controls':
            altered['physical_input_sha256'] = '0' * 64
        elif name == 'counterfeit_save':
            data = bytearray(saved)
            data[22] = 0
            altered['earned_save_hex'] = data.hex()
            altered['earned_save_sha256'] = digest(data)
        elif name == 'wrong_menu':
            altered['canonical']['stage'] = 'C10DAE'
        else:
            altered['canonical']['postflight_resets'] = 1
        read_text = Path.read_text
        def mutated_text(path, *positional, **keywords):
            return json.dumps(altered) if path == report_path else read_text(path, *positional, **keywords)
        with patch.object(Path, 'read_text', mutated_text):
            try:
                verified_native_prefix(args.prefix, args.runner)
            except AssertionError as error:
                mutations[name] = str(error) or 'actual runtime evidence rejected'
            else:
                raise AssertionError(f'{name}: altered prefix accepted')
    assert report_path.read_bytes() == before
    continuation = None
    if args.comparison:
        retained = json.loads((args.comparison / 'report.json').read_text())
        assert retained['native_prefix'] == evidence
        assert retained['comparison_completed'] and retained['native_prefix_execution_exact']
        anchors = json.loads((args.comparison / 'anchors.json').read_text())
        assert anchors == retained['event_report']
        capture = args.comparison / 'native.jsonl.gz'
        rows = verify_prefix_execution(args.prefix, capture, anchors, original)
        assert rows == retained['native_prefix_observations_exact']
        for field in ('native_first', 'frame'):
            altered = copy.deepcopy(anchors)
            altered['anchors'][len(original['escort_anchors'])][field] += 1
            try:
                verify_prefix_execution(args.prefix, capture, altered, original)
            except AssertionError as error:
                mutations['menu_' + field] = str(error)
            else:
                raise AssertionError(f'altered continuation menu {field} accepted')
        continuation = dict(report_sha256=digest((args.comparison / 'report.json').read_bytes()),
                            exact_prefix_observations=rows)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(dict(actual_prefix_verified=True, evidence=evidence,
        mutation_rejections=mutations, continuation=continuation, retained_report_unchanged=True), indent=2) + '\n')
    print(f'{len(mutations)} altered native earned-prefix claims rejected; actual save verified')


if __name__ == '__main__':
    main()
