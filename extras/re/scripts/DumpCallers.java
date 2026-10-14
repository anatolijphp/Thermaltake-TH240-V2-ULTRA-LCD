// Ghidra headless script: for every caller of the given function addresses, print the decompiled
// lines that contain the call (plus context), grouped by caller.
// Args: <outFile> <hexAddr> [hexAddr...]
//@category ttlcd
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;
import java.io.PrintWriter;
import java.util.*;

public class DumpCallers extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        PrintWriter out = new PrintWriter(a[0], "UTF-8");
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        for (int i = 1; i < a.length; i++) {
            Address target = toAddr(Long.parseLong(a[i], 16));
            String name = "FUN_" + a[i].toLowerCase();
            Set<Function> callers = new LinkedHashSet<>();
            for (Reference r : getReferencesTo(target)) {
                Function f = getFunctionContaining(r.getFromAddress());
                if (f != null) callers.add(f);
            }
            out.println("######## callers of " + name + ": " + callers.size());
            for (Function f : callers) {
                DecompileResults res = dec.decompileFunction(f, 120, monitor);
                if (!res.decompileCompleted()) continue;
                String[] lines = res.getDecompiledFunction().getC().split("\n");
                // function label from first __FUNCTION__-style string in the body
                String label = "";
                for (String l : lines) {
                    int k = l.indexOf("\"void __cdecl ");
                    if (k < 0) k = l.indexOf("\"bool __cdecl ");
                    if (k < 0) k = l.indexOf("\"int __cdecl ");
                    if (k >= 0) { label = l.substring(k).trim(); break; }
                }
                out.println("== " + f.getName() + " @ " + f.getEntryPoint() + " " + label);
                for (int j = 0; j < lines.length; j++)
                    if (lines[j].contains(name + "(")) {
                        for (int c = Math.max(0, j - 2); c <= Math.min(lines.length - 1, j + 1); c++) out.println("   " + lines[c].trim());
                        out.println("   ..");
                    }
            }
        }
        out.close();
    }
}
