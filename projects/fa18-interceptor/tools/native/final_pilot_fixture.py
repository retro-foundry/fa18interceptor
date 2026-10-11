"""Load final-mission availability earned by the rescue/cruise save chain."""
import hashlib
import json
from pathlib import Path

from cruise_pilot_fixture import load_cruise_pilot

ROOT = Path(__file__).resolve().parents[2]
FIXTURE = ROOT / 'tools/native/fixtures/final-mission-earned-pilot.json'


def load_final_pilot():
    metadata = json.loads(FIXTURE.read_text())
    config = bytes.fromhex(metadata['config_hex'])
    assert metadata['schema'] == 1 and len(config) == 78
    assert hashlib.sha256(config).hexdigest() == metadata['config_sha256']
    assert metadata['mission_mode'] == 8 and metadata['availability_earned_by_mode'] == 7
    assert metadata['mission_availability_earned'] and not metadata['full_tour_earned']
    assert not metadata['native_flight_state_seeded']
    assert config[6] == 7 and config[25] == 1 and int.from_bytes(config[56:58], 'big') == 5
    assert hashlib.sha256(load_cruise_pilot()).hexdigest() == metadata['initial_config_sha256']
    assert hashlib.sha256((ROOT / 'local/media/fa18.adf').read_bytes()).hexdigest() == metadata['adf_sha256']
    for name, digest in metadata['input_sha256_lf'].items():
        assert hashlib.sha256((ROOT / name).read_bytes().replace(b'\r\n', b'\n')).hexdigest() == digest, name
    return config
