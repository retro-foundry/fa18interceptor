"""Read original action tables/producers without asserting complete caller bounds.

The C2C392 selector is decremented before EXT.W of its low byte. Every
nonzero byte is evaluated exactly. This is source evidence, not a port or
a runtime selector guard. Record copies and saved-state sources remain open.
"""
import argparse,hashlib,json
from audit_command_dispatch import ROOT,STATE,source_decoder
OUT=ROOT/'analysis/data/action_dispatch_producer_read.json'
def inventory():
 state,d=source_decoder()
 def rows(start,end):
  result=[];pc=start
  while pc<end:
   decoded=d.decode(pc)
   if decoded is None:raise ValueError(f'undecodable producer source {pc:06X}')
   length,opcode,handler,text=decoded
   result.append({'pc':f'{pc:06X}','instruction':text,'bytes':''.join(f'{d.word(pc+i):04x}' for i in range(0,length,2))});pc+=length
  assert pc==end
  return result
 def longword(pc):return (d.word(pc)<<16)|d.word(pc+2)
 pointers=[{'action':i+1,'slot':f'{0xc2baf8+4*i:06X}','target':f'{longword(0xc2baf8+4*i):06X}'} for i in range(40)]
 parameters=[{'slot':f'{pc:06X}','countdown':d.word(pc),'action':d.word(pc+2)} for pc in range(0xc2bb98,0xc2bcc8,4)]
 assert len(parameters)==76 and {r['action'] for r in parameters}=={2,5,9}
 streams=[{'slot':f'{pc:06X}','first_longword':f'{longword(pc):08X}','action_longword':f'{longword(pc+4):08X}',
  'action_byte':longword(pc+4)&255} for pc in range(0xc2366a,0xc236aa,8)]
 assert [r['action_byte'] for r in streams]==[35,11,39,14,17,20,23,26]
 indices=[]
 for byte in range(1,256):
  decremented=(byte-1)&255;signed=decremented if decremented<128 else decremented-256
  slot=0xc2baf8+4*signed
  indices.append({'action_byte':byte,'signed_index_after_decrement_and_ext':signed,'slot':f'{slot:06X}',
   'original_longword':f'{longword(slot):08X}','within_declared_forty_pointer_table':0<=signed<40})
 windows={name:rows(start,end) for name,start,end in (
  ('consumer_index',0xc2c45e,0xc2c470),('parameter_selection',0xc241b4,0xc242d4),
  ('negative_stream_publication',0xc2354c,0xc23578),('saved_state_read',0xc08eb8,0xc08ed0),
  ('saved_state_write',0xc08ed0,0xc08ee4),('restore_scene_limit',0xc08f2e,0xc08f38),
  ('clear_scene_limit',0xc0faa4,0xc0fab6),('increment_scene_limit',0xc113a6,0xc113c2))}
 return {'state':STATE.relative_to(ROOT).as_posix(),'state_sha256':hashlib.sha256(state).hexdigest(),
  'meaning':'behavioral table/index formulas and identified producer stores; complete producer-domain proof remains absent',
  'action_pointers':pointers,'parameter_pairs':parameters,'original_stream_descriptors':streams,
  'all_nonzero_byte_index_results':indices,'original_source_windows':windows,
  'facts':['The original consumer has no general bounds check; SUBQ.W precedes EXT.W of the low byte.',
   'Forty contiguous pointers end at C2BB98. The following 76 countdown/action pairs end at C2BCC8.',
   'Eight original negative stream descriptors publish actions 35,11,39,14,17,20,23,26.',
   'Parameter selection sign-extends C458A7, clamps its signed upper bound to 3, and supplies no lower clamp.',
   'For nonnegative bounded scene-limit inputs, parameter actions are 2,5,9.'],
  'open':['Reconcile every actor-record bulk copy and overlapping word/long store with the action byte.',
   'Reconcile allocation, saved-state load/write and all C458A7/C458A8 producers before asserting nonnegative scene limits.',
   'Complete C2C392 native parent, including every valid computed arm, actual children and original non-returning/fault behavior.'],
  'implementation':False,'runtime_guard_added':False,'whole_parent_complete':False}
def main():
 p=argparse.ArgumentParser(description=__doc__);p.add_argument('--write',action='store_true');a=p.parse_args();r=inventory()
 if a.write:OUT.write_text(json.dumps(r,indent=2)+'\n',encoding='utf-8')
 elif json.loads(OUT.read_text())!=r:raise ValueError('Action-dispatch producer evidence changed')
 print('Action-dispatch read: 40 pointers, 76 parameter pairs, 8 stream descriptors, 255 exact byte indices')
 print('Producer domain and whole parent remain incomplete; no runtime guard or implementation supplied')
if __name__=='__main__':main()
