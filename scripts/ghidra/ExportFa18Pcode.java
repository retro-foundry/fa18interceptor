// Deterministic raw P-code, retaining runtime addresses and original bytes.
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.pcode.*;
import ghidra.program.model.lang.Register;
import com.google.gson.*;
import java.nio.file.*;
import java.io.*;

public class ExportFa18Pcode extends GhidraScript {
    JsonElement varnode(Varnode v) {
        if (v==null) return JsonNull.INSTANCE;
        JsonObject o = new JsonObject();
        o.addProperty("space",v.getAddress().getAddressSpace().getName());
        o.addProperty("offset",Long.toUnsignedString(v.getOffset(),16));
        o.addProperty("size",v.getSize());
        if (v.isRegister()) { Register r=currentProgram.getRegister(v); if(r!=null)o.addProperty("register",r.getName()); }
        return o;
    }
    public void run() throws Exception {
        Path dir=Paths.get(getScriptArgs()[0]); Files.createDirectories(dir);
        Gson gson=new Gson();
        int count=0, ops=0;
        try(BufferedWriter out=Files.newBufferedWriter(dir.resolve("instructions.pcode.jsonl"));
            BufferedWriter asm=Files.newBufferedWriter(dir.resolve("observed.asm.txt"))) {
            for(Instruction ins:currentProgram.getListing().getInstructions(true)) {
                JsonObject row=new JsonObject();
                long pc=ins.getAddress().getOffset();
                row.addProperty("address",pc);
                row.addProperty("bank",pc<0x80000?"chip":"slow");
                row.addProperty("file_offset",pc<0x80000?pc:pc-0xc00000);
                StringBuilder hex=new StringBuilder(); for(byte b:ins.getBytes())hex.append(String.format("%02x",b&255));
                row.addProperty("bytes",hex.toString()); row.addProperty("assembly",ins.toString());
                Function fn=getFunctionContaining(ins.getAddress());
                row.addProperty("function",fn==null?null:fn.getName());
                row.addProperty("flow_type",ins.getFlowType().toString());
                JsonArray flows=new JsonArray(); for(var a:ins.getFlows())flows.add(a.getOffset()); row.add("static_flows",flows);
                JsonArray code=new JsonArray();
                for(PcodeOp op:ins.getPcode()) {
                    JsonObject p=new JsonObject(); p.addProperty("opcode",op.getMnemonic()); p.add("output",varnode(op.getOutput()));
                    JsonArray inputs=new JsonArray(); for(Varnode v:op.getInputs())inputs.add(varnode(v)); p.add("inputs",inputs); code.add(p);ops++;
                }
                row.add("pcode",code); out.write(gson.toJson(row));out.newLine();
                asm.write(String.format("%08x  %-24s %s%n",pc,hex,ins));count++;
            }
        }
        JsonArray functions=new JsonArray();
        for(Function fn:currentProgram.getFunctionManager().getFunctions(true)) {
            JsonObject f=new JsonObject();f.addProperty("address",fn.getEntryPoint().getOffset());f.addProperty("name",fn.getName());
            f.addProperty("body_bytes",fn.getBody().getNumAddresses());f.addProperty("meaning","structural"); functions.add(f);
        }
        Files.writeString(dir.resolve("functions.json"),new GsonBuilder().setPrettyPrinting().create().toJson(functions)+"\n");
        JsonObject summary=new JsonObject();summary.addProperty("instructions",count);summary.addProperty("pcode_ops",ops);
        summary.addProperty("functions",functions.size()); summary.addProperty("language",currentProgram.getLanguageID().toString());
        summary.addProperty("caution","Ghidra uses its 68040 superset language; observed lengths are cross-checked against Capstone 68000. No automatic unobserved disassembly.");
        Files.writeString(dir.resolve("summary.json"),new GsonBuilder().setPrettyPrinting().create().toJson(summary)+"\n");
        println(summary.toString());
    }
}
