// Ghidra headless script: decompile functions that reference given strings.
// Args: <outFile> <needle1> [needle2 ...]
// For each defined string containing a needle, every referencing function is decompiled once.
//@category ttlcd
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.data.StringDataInstance;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.Reference;
import java.io.PrintWriter;
import java.util.*;

public class DumpLcd extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        PrintWriter out = new PrintWriter(a[0], "UTF-8");
        List<String> needles = Arrays.asList(a).subList(1, a.length);
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        Set<Function> done = new LinkedHashSet<>();
        Listing listing = currentProgram.getListing();
        for (Data d : listing.getDefinedData(true)) {
            if (monitor.isCancelled()) break;
            StringDataInstance s = StringDataInstance.getStringDataInstance(d);
            if (s == StringDataInstance.NULL_INSTANCE) continue;
            String val = s.getStringValue();
            if (val == null) continue;
            boolean hit = false;
            for (String n : needles) if (val.contains(n)) { hit = true; break; }
            if (!hit) continue;
            for (Reference r : getReferencesTo(d.getAddress())) {
                Function f = getFunctionContaining(r.getFromAddress());
                if (f == null || done.contains(f)) continue;
                done.add(f);
                DecompileResults res = dec.decompileFunction(f, 120, monitor);
                out.println("//==== " + f.getName() + " @ " + f.getEntryPoint() + "  (refs \"" + val + "\")");
                out.println(res.decompileCompleted() ? res.getDecompiledFunction().getC() : "// decompile failed: " + res.getErrorMessage());
            }
        }
        out.close();
        println("dumped " + done.size() + " functions");
    }
}
