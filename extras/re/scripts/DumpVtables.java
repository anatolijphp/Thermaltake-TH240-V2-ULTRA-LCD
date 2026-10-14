// Ghidra headless script: for every RTTI-recovered vftable, list its slots and decompile each method.
// Args: <outFile> [classFilterRegex]
//@category ttlcd
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import java.io.PrintWriter;
import java.util.*;

public class DumpVtables extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        PrintWriter out = new PrintWriter(a[0], "UTF-8");
        String filter = a.length > 1 ? a[1] : ".*";
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        int ptr = currentProgram.getDefaultPointerSize();
        Set<Function> done = new HashSet<>();
        SymbolIterator it = currentProgram.getSymbolTable().getAllSymbols(true);
        List<Symbol> vts = new ArrayList<>();
        while (it.hasNext()) {
            Symbol s = it.next();
            if (s.getName().contains("vftable") && s.getName(true).matches(filter)) vts.add(s);
        }
        for (Symbol s : vts) {
            out.println("######## " + s.getName(true) + " @ " + s.getAddress());
            Address p = s.getAddress();
            for (int i = 0; i < 64; i++) {
                long v = ptr == 4 ? (getInt(p) & 0xffffffffL) : getLong(p);
                Address t = toAddr(v);
                Function f = getFunctionAt(t);
                if (f == null) {
                    if (getInstructionAt(t) == null) break;
                    f = createFunction(t, null);
                    if (f == null) break;
                }
                out.println("  [" + i + "] +" + (i * ptr) + "  " + f.getName() + " @ " + t);
                if (done.add(f)) {
                    DecompileResults res = dec.decompileFunction(f, 90, monitor);
                    String c = res.decompileCompleted() ? res.getDecompiledFunction().getC() : "// decompile failed";
                    for (String line : c.split("\n")) out.println("      " + line);
                }
                p = p.add(ptr);
                if (i > 0 && getSymbolAt(p) != null && getSymbolAt(p).getName().contains("vftable")) break;
            }
        }
        out.close();
        println("vtables: " + vts.size());
    }
}
