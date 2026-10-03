"""Audit the ten complete flight-record action owners against sealed bytes."""
import argparse,json
from audit_command_dispatch import ROOT,audit
ENTRIES=('C230E8','C23116','C23186','C23228','C233AA','C23578','C236AA','C23716','C2377E','C257EC')
MANIFEST=ROOT/'analysis/data/flight_record_actions_source_scope.json'
def inventory(): return audit(ENTRIES,additional_cold_entries=ENTRIES)
def main():
    p=argparse.ArgumentParser(description=__doc__); p.add_argument('--write',action='store_true'); a=p.parse_args(); result=inventory()
    if a.write: MANIFEST.write_text(json.dumps(result,indent=2)+'\n')
    elif json.loads(MANIFEST.read_text())!=result: raise ValueError('flight-record action source changed')
    print(f"flight-record action source: {result['unique_instruction_count']} unique / {result['shared_instruction_count']} shared boundaries")
if __name__=='__main__': main()
