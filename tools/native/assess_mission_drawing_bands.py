"""Locate complete-flight drawing differences without excluding any pixels.

Five adjoining full-width bands cover every byte of both complete pages.
Band results are diagnostics, never a replacement full-page acceptance gate.
"""
import argparse
import copy
import gzip
import json
from pathlib import Path

from check_gameplay_checkpoint import ROOT
from check_qualification_message_cadence import digest
from check_recorded_original_mission_trace import verified_update_mapping
from compare_flight_traces import number, read_trace


def assess(source, native, mapping, bands, first, last):
    results = [dict(**band, matching=0, compared=last - first + 1,
                    matching_planes=[0] * 8, first_difference=None) for band in bands]
    strict = 0
    patterns = {}
    for i in range(first, last + 1):
        a, b = source[i], native[mapping[i]]
        assert a['pages_valid'] and b['pages_valid']
        strict += a['pages'] == b['pages']
        # Every whole-plane difference must occur in at least one adjoining
        # band; a matching plane must match in every band, without exclusions.
        for plane in range(8):
            assert all(a['drawing_bands'][band][plane] == b['drawing_bands'][band][plane]
                       for band in range(len(bands))) == (a['pages'][plane] == b['pages'][plane]), ('whole-plane/band disagreement', i, plane)
        changed = []
        for band, result in enumerate(results):
            different = [plane for plane in range(8)
                         if a['drawing_bands'][band][plane] != b['drawing_bands'][band][plane]]
            result['matching'] += not different
            for plane in range(8):
                result['matching_planes'][plane] += plane not in different
            if different:
                changed.append(band)
                if result['first_difference'] is None:
                    result['first_difference'] = dict(source_iteration=i, native_iteration=mapping[i],
                        game_tick=number(a, 'game_tick'), planes=different)
        pattern = ','.join(map(str, changed)) or 'none'
        patterns[pattern] = patterns.get(pattern, 0) + 1
    return dict(first=first, last=last, observations=last - first + 1,
                complete_pages_matching=strict, bands=results, changed_band_patterns=patterns)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--traces', type=Path, required=True)
    parser.add_argument('--source-evidence', type=Path, required=True)
    parser.add_argument('--source-updates', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    reference = json.loads((args.traces / 'report.json').read_text())
    original = json.loads((args.source_evidence / 'report.json').read_text())
    assert reference['baseline_source_trace_sha256'] == original['driver_trace_sha256']
    for relative, expected in reference['input_hashes'].items():
        assert digest((ROOT / relative).read_bytes()) == expected
    headers, traces = {}, {}
    for name in ('source', 'native'):
        path = args.traces / f'{name}.jsonl.gz'
        assert digest(gzip.decompress(path.read_bytes())) == reference[f'{name}_trace_sha256']
        headers[name], traces[name] = read_trace(path)
    assert headers['source'] == headers['native'] and 'drawing_bands' in headers['source']
    mapping, updates = verified_update_mapping(args.source_updates,
        args.source_evidence / 'driver.jsonl.gz', original)
    assert updates == reference['source_update_evidence']
    comparison = reference['whole_successful_flight']
    assert comparison['strict_gameplay_matching']
    first, last = comparison['first'], comparison['last']
    bands = headers['source']['drawing_bands']
    result = assess(traces['source'], traces['native'], mapping, bands, first, last)
    assert result['complete_pages_matching'] == comparison['complete_pages_matching']
    corrupted = dict(traces['native'])
    j = mapping[first]
    corrupted[j] = copy.deepcopy(corrupted[j])
    corrupted[j]['drawing_bands'][0][0] = '00' * 32
    assert corrupted[j]['drawing_bands'][0][0] != traces['native'][j]['drawing_bands'][0][0]
    try:
        assess(traces['source'], corrupted, mapping, bands, first, last)
    except AssertionError:
        result['corrupted_matching_plane_rejected'] = True
    else:
        raise AssertionError('corrupted matching-plane band accepted')
    result.update(runner_sha256=reference['runner_sha256'],
        trace_sha256={name: reference[f'{name}_trace_sha256'] for name in traces},
        source_update_mapping_sha256=updates['mapping_sha256'],
        acceptance='Localization only. Every complete page remains compared; no drawing band '
                   'or pixel is excluded from acceptance. Source/native observations are paired '
                   'only by their instruction-proven update identities.')
    args.out.mkdir(parents=True, exist_ok=True)
    (args.out / 'report.json').write_text(json.dumps(result, indent=2) + '\n')
    for band in result['bands']:
        print(f"Rows {band['y']}..{band['y'] + band['rows'] - 1}: "
              f"{band['matching']}/{band['compared']} match; first difference {band['first_difference']}")


if __name__ == '__main__':
    main()
