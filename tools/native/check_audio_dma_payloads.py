"""Bind original fetched words to catalogued sample bytes in current native RAM.

The historical native request trace establishes channel/catalog identity only.
Current native assets come from a sealed playable flight snapshot. Neither
source RAM nor these diagnostics initialize gameplay. Uncatalogued fetches are
retained explicitly; this does not accept native handoff timing or waveform.
"""
import argparse
import gzip
import hashlib
import json
from pathlib import Path

from check_original_audio_dma import ROOT, validate_dma, frames, FETCH, open_stream
from check_original_audio_capture import recorded_bytes
from frame_delta import snapshots


def digest(data):
    return hashlib.sha256(data).hexdigest()


def payload_spans(catalog, original_chip, native_data):
    spans = [[] for _ in range(4)]
    for channel, items in enumerate(catalog['payload_catalog_by_channel']):
        for item in items:
            assert item['native_requests'] and item['native_sample_addresses']
            buffers = []
            for address in item['native_sample_addresses']:
                offset = address if address < 0x80000 else address-0xc00000+0x80000
                assert 0 <= offset <= len(native_data)-item['bytes']
                payload = native_data[offset:offset+item['bytes']]
                assert digest(payload) == item['sha256'], ('Current native sample bytes changed',channel,address)
                buffers.append(payload)
            assert all(buffer == buffers[0] for buffer in buffers)
            for address in item['original_sample_addresses']:
                assert 0 <= address <= len(original_chip)-item['bytes']
                payload = original_chip[address:address+item['bytes']]
                assert digest(payload) == item['sha256'] and payload == buffers[0]
                spans[channel].append((address,address+item['bytes'],buffers[0]))
    return spans


def word_witness(spans, channel, address, value):
    for start, end, payload in spans[channel]:
        if start <= address and address+2 <= end:
            assert int.from_bytes(payload[address-start:address-start+2],'big') == value, \
                ('Fetched word differs from current native asset',channel,address,value)
            return True
    return False


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--capture',type=Path,required=True)
    parser.add_argument('--baseline',type=Path,required=True)
    parser.add_argument('--pcm-reference',type=Path)
    parser.add_argument('--catalog',type=Path,required=True)
    parser.add_argument('--native-delta',type=Path,required=True)
    parser.add_argument('--native-report',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    args = parser.parse_args()
    result = validate_dma(args.capture,args.baseline,args.pcm_reference)
    catalog_bytes = args.catalog.read_bytes()
    catalog_seal = json.loads((ROOT/'analysis/figures/native_complete_audio_payload_checkpoint.json').read_text())
    assert digest(catalog_bytes) == catalog_seal['full_report_sha256']
    catalog = json.loads(catalog_bytes)
    assert catalog['original_validation']['audio_events']['sha256'] == result['audio_events']['sha256']
    assert catalog['original_validation']['authority'] == result['authority']
    native_seal = json.loads((ROOT/'analysis/figures/native_complete_frame_body_checkpoint.json').read_text())['evidence']['complete_release']
    native_report_bytes = args.native_report.read_bytes()
    assert digest(native_report_bytes) == native_seal['report_sha256']
    native_report = json.loads(native_report_bytes)
    assert native_report['original_bodies_matching'] and native_report['runtime_counters_preserved']
    assert native_report['runner_sha256'] == native_seal['runner_sha256']
    with gzip.open(args.native_delta,'rb') as file:
        assert hashlib.file_digest(file,'sha256').hexdigest() == native_seal['stream_sha256'] == native_report['stream_sha256']
    stream = snapshots(args.native_delta)
    try:
        native = next(stream)
    finally:
        stream.close()
    assert native['boundary'] == 2 and native['iteration'] == native_report['native_first']
    assert native['ram_sha256'] == native_report['identities'][0]['entry_sha256']
    spans = payload_spans(catalog,recorded_bytes(args.capture,'chip.bin'),native['data'])
    matched, unknown, first = [0]*4, [0]*4, [None]*4
    uncatalogued = []
    audio = result['recorded_audio']
    with open_stream(args.capture/result['audio_dma']['file']) as file:
        for row,payload in frames(file,audio['first_replay_call'],audio['first_replay_call']+audio['replay_calls']-1):
            for index,address,value,register,hpos,vpos,channel in FETCH.iter_unpack(payload):
                if word_witness(spans,channel,address,value):
                    matched[channel] += 1
                else:
                    unknown[channel] += 1
                    event = dict(call=row[0],source_frame=row[2],channel=channel,
                        address=f'{address:06X}',fetched=value,beam=[hpos,vpos])
                    if first[channel] is None: first[channel] = event
                    # Bound diagnostic output independently of the trace size.
                    if len(uncatalogued)<32: uncatalogued.append(event)
    assert [a+b for a,b in zip(matched,unknown)] == result['fetched_words_by_channel']
    result.update(catalog_report_sha256=digest(catalog_bytes),
        historical_native_request_runner_sha256=catalog['native_runner_sha256'],
        current_native_asset_runner_sha256=native_report['runner_sha256'],
        native_report_sha256=digest(native_report_bytes),native_delta_sha256=native_seal['stream_sha256'],
        native_asset_snapshot={k:v for k,v in native.items() if k!='data'},
        matched_current_native_asset_words_by_channel=matched,
        uncatalogued_fetched_words_by_channel=unknown, first_uncatalogued_fetch_by_channel=first,
        first_uncatalogued_fetches=uncatalogued,
        all_catalogued_fetched_words_equal_current_native_assets=True,
        complete_fetched_word_catalog_coverage=not any(unknown),
        payload_witness_scope='Historical same-channel request catalog plus current native asset bytes; actual original words compared without replacement. Uncatalogued fetches remain explicit. No current-native request order, onset, repetition, handoff or whole-waveform acceptance.')
    args.out.parent.mkdir(parents=True,exist_ok=True)
    args.out.write_text(json.dumps(result,indent=2)+'\n')
    print(f"{sum(matched)} original fetched words match current native asset bytes; {sum(unknown)} uncatalogued fetches retained; sound timing remains open")


if __name__ == '__main__':
    main()
