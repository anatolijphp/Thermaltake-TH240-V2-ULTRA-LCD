// Ghidra headless script: decompile functions at the given addresses, plus list their callers.
// Args: <outFile> <hexAddr> [hexAddr...]
//@category ttlcd
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;
import java.io.PrintWriter;

public class DumpFunc extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        PrintWriter out = new PrintWriter(a[0], "UTF-8");
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        for (int i = 1; i < a.length; i++) {
            if (a[i].startsWith("d:")) { // d:<hexAddr>:<len> -> hex dump + ascii/utf16 guess
                String[] p = a[i].split(":");
                byte[] b = getBytes(toAddr(Long.parseLong(p[1], 16)), Integer.parseInt(p[2]));
                StringBuilder sb = new StringBuilder("// data " + p[1] + ": ");
                for (byte x : b) sb.append(String.format("%02X ", x));
                sb.append(" |");
                for (byte x : b) sb.append(x >= 32 && x < 127 ? (char) x : '.');
                out.println(sb);
                continue;
            }
            if (a[i].startsWith("r:")) { // r:<hexAddr> -> decompile every function referencing that address
                java.util.Set<Function> fs = new java.util.LinkedHashSet<>();
                for (Reference r : getReferencesTo(toAddr(Long.parseLong(a[i].substring(2), 16)))) {
                    Function c = getFunctionContaining(r.getFromAddress());
                    if (c != null) fs.add(c);
                }
                for (Function c : fs) {
                    DecompileResults res = dec.decompileFunction(c, 120, monitor);
                    out.println("//==== refs " + a[i] + " in " + c.getName() + " @ " + c.getEntryPoint());
                    out.println(res.decompileCompleted() ? res.getDecompiledFunction().getC() : "// decompile failed");
                }
                continue;
            }
            Function f = getFunctionAt(toAddr(Long.parseLong(a[i], 16)));
            if (f == null) { out.println("// no function at " + a[i]); continue; }
            out.print("//==== " + f.getName() + " @ " + f.getEntryPoint() + "  callers:");
            for (Reference r : getReferencesTo(f.getEntryPoint())) {
                Function c = getFunctionContaining(r.getFromAddress());
                out.print(" " + (c != null ? c.getName() : r.getFromAddress().toString()) + "(" + r.getReferenceType() + ")");
            }
            out.println();
            DecompileResults res = dec.decompileFunction(f, 120, monitor);
            out.println(res.decompileCompleted() ? res.getDecompiledFunction().getC() : "// decompile failed");
        }
        out.close();
    }
}
