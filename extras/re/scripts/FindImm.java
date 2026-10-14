// Ghidra headless script: decompile functions that contain an instruction using a given immediate
// at least <minHits> times. Args: <outFile> <minHits> <hexImm> [hexImm...] (all immediates must occur)
//@category ttlcd
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import java.io.PrintWriter;
import java.util.*;

public class FindImm extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        PrintWriter out = new PrintWriter(a[0], "UTF-8");
        int minHits = Integer.parseInt(a[1]);
        List<Long> imms = new ArrayList<>();
        for (int i = 2; i < a.length; i++) imms.add(Long.parseLong(a[i], 16));
        Map<Function, Map<Long, Integer>> hits = new LinkedHashMap<>();
        Listing listing = currentProgram.getListing();
        for (Instruction ins : listing.getInstructions(true)) {
            Function f = getFunctionContaining(ins.getAddress());
            if (f == null) continue;
            for (int op = 0; op < ins.getNumOperands(); op++)
                for (Object o : ins.getOpObjects(op))
                    if (o instanceof Scalar) {
                        long v = ((Scalar) o).getUnsignedValue();
                        if (imms.contains(v)) hits.computeIfAbsent(f, k -> new HashMap<>()).merge(v, 1, Integer::sum);
                    }
        }
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        int n = 0;
        for (Map.Entry<Function, Map<Long, Integer>> e : hits.entrySet()) {
            Map<Long, Integer> m = e.getValue();
            boolean all = true; int total = 0;
            for (Long v : imms) { Integer c = m.get(v); if (c == null) { all = false; break; } total += c; }
            if (!all || total < minHits) continue;
            Function f = e.getKey();
            DecompileResults r = dec.decompileFunction(f, 120, monitor);
            out.println("//==== " + f.getName() + " @ " + f.getEntryPoint() + " hits=" + m);
            out.println(r.decompileCompleted() ? r.getDecompiledFunction().getC() : "// decompile failed");
            n++;
        }
        out.close();
        println("matched " + n + " functions");
    }
}
