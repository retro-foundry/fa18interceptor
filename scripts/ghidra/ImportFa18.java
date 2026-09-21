// Import only executed instruction starts. RAM bytes remain at runtime addresses.
import ghidra.app.script.GhidraScript;
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.cmd.function.CreateFunctionCmd;
import ghidra.program.model.address.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;
import java.nio.file.*;
import java.io.*;
import java.util.*;

public class ImportFa18 extends GhidraScript {
    public void run() throws Exception {
        Path dir = Paths.get(getScriptArgs()[0]);
        MemoryBlock chip = currentProgram.getMemory().getBlock(toAddr(0));
        chip.setName("chip_ram");
        chip.setWrite(true);
        chip.setExecute(true);
        if (currentProgram.getMemory().getBlock(toAddr(0xc00000)) == null) {
            byte[] slow = Files.readAllBytes(dir.resolve("slow.bin"));
            MemoryBlock b = currentProgram.getMemory().createInitializedBlock("slow_ram", toAddr(0xc00000),
                new ByteArrayInputStream(slow), slow.length, monitor, false);
            b.setRead(true); b.setWrite(true); b.setExecute(true);
        }
        if (currentProgram.getMemory().getBlock(toAddr(0xdff000)) == null) {
            MemoryBlock b = currentProgram.getMemory().createUninitializedBlock("custom_mmio", toAddr(0xdff000), 0x200, false);
            b.setRead(true); b.setWrite(true); b.setVolatile(true); b.setExecute(false);
        }
        if (currentProgram.getMemory().getBlock(toAddr(0xbfe001)) == null) {
            MemoryBlock b = currentProgram.getMemory().createUninitializedBlock("cia_a_mmio", toAddr(0xbfe001), 0xf00, false);
            b.setRead(true); b.setWrite(true); b.setVolatile(true); b.setExecute(false);
        }
        long[] regs = {0xdff002,0xdff004,0xdff006,0xdff020,0xdff024,0xdff040,0xdff058,0xdff080,
            0xdff088,0xdff096,0xdff09a,0xdff09c,0xdff0e0,0xdff100,0xdff108,0xdff10a,0xdff180,0xbfec01,0xbfed01};
        String[] names = {"DMACONR","VPOSR","VHPOSR","DSKPTH","DSKLEN","BLTCON0","BLTSIZE","COP1LCH",
            "COPJMP1","DMACON","INTENA","INTREQ","BPL1PTH","BPLCON0","BPL1MOD","BPL2MOD","COLOR00","CIAA_SDR","CIAA_ICR"};
        for (int i=0;i<regs.length;i++) createLabel(toAddr(regs[i]), names[i], true, SourceType.USER_DEFINED);
        List<Address> entries = new ArrayList<>();
        for (String line: Files.readAllLines(dir.resolve("instructions.tsv")).subList(1,
                Files.readAllLines(dir.resolve("instructions.tsv")).size())) {
            String[] col = line.split("\t");
            Address addr = toAddr(Long.parseLong(col[0],16));
            int length = col[1].length()/2;
            AddressSet scope = new AddressSet(addr, addr.add(length-1));
            if (getInstructionAt(addr) == null && !new DisassembleCommand(addr, scope, false).applyTo(currentProgram, monitor))
                throw new IOException("Disassembly failed at " + addr);
            Instruction ins = getInstructionAt(addr);
            if (ins == null || ins.getLength()!=length) throw new IOException("Decoder length disagreement at " + addr);
            if (col[2].equals("1")) entries.add(addr);
        }
        for (Address addr: entries) {
            String name = "observed_call_" + addr;
            if (getFunctionAt(addr)==null) new CreateFunctionCmd(name, addr, null, SourceType.USER_DEFINED).applyTo(currentProgram, monitor);
            setPlateComment(addr, "Structural observed call target. See snapshot trace.jsonl. Purpose and complete boundary unproven.");
        }
        println("Imported observed RAM instructions and " + entries.size() + " call-target candidates.");
    }
}
