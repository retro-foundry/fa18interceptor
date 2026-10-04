"""Keep bounded whole-entry coverage separate from actual production segments."""
import hashlib,json
from audit_hud_render_parents_source import ROOT,inventory
from audit_command_dispatch import source_decoder

def verify():
 s=inventory();_,d=source_decoder()
 xs=[d.word(a) for a in range(0xc34560,0xc34578,4)]
 assert xs==[85,85,93,225,233,233]
 def signed(v):return (v+32768)%65536-32768
 # Execute the source's entry-domain conditions mathematically for every
 # origin and every original table segment. No table mutation is assumed.
 reaches=0
 for origin in range(-32768,32768):
  for left,right in zip(xs,xs[1:]):
   add=left+origin;lo=signed(add)
   if add<0:continue # C34270 chooses the other source tail.
   if lo>319:continue # C34282 chooses the other source tail.
   if right+origin<0:reaches+=1
 assert reaches==0
 # C32376 is reached with a positive old redraw byte and a cleared old page
 # bit. The startup path instead sets bit 15 and writes redraw=2. There is
 # no child call between that gate and the decrement.
 assert all(int(c['pc'],16)>=0xc32496 for c in s['owners']['C322EE']['child_call_sites'])
 assert all(redraw-1>=0 for redraw in range(1,128))
 segments=[]
 for name,entry,pcs in [('clamp','C34296',{'C34296','C3429C','C3429E','C342A0','C342A4','C342A6'}),('page','C32376',{'C32376','C3237C','C32380'})]:
  p=ROOT/f'build/recomp/hud_render_parents_{name}_full.log';t=p.read_text()
  assert f'oracle {entry}: 16384 complete calls matched all registers, PC, full SR and all RAM' in t
  observed=set(t.split('visited:')[1].splitlines()[0].split());assert pcs<=observed
  assert 'hardware: 0 ordered Custom writes validated' in t
  segments.append({'entry':entry,'calls':16384,'source_pcs':sorted(pcs),'log':str(p.relative_to(ROOT)).replace('\\','/'),'sha256':hashlib.sha256(p.read_bytes()).hexdigest()})
 return {'whole_entry_cold_pcs':['C32380','C3429E'],'source_frame_xs':xs,'origins_checked':65536,'whole_entry_coverage_not_claimed_for_cold_pcs':True,'production_segments':segments}

if __name__=='__main__':print(json.dumps(verify(),indent=2))
