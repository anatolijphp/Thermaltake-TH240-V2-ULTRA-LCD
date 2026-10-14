// Ghidra headless script: decompile functions whose instructions use a given constant.
// Args: <outFile> <hexValue> [maxFuncs]
//@category ttlcd
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import ghidra.program.model.scalar.Scalar;
import java.io.PrintWriter;
import java.util.*;

public class DumpScalar extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        PrintWriter out = new PrintWriter(a[0], "UTF-8");
        long want = Long.parseLong(a[1], 16);
        int max = a.length > 2 ? Integer.parseInt(a[2]) : 50;
        DecompInterface dec = new DecompInterface();
        dec.openProgram(currentProgram);
        Set<Function> done = new LinkedHashSet<>();
        InstructionIterator it = currentProgram.getListing().getInstructions(true);
        while (it.hasNext() && done.size() < max && !monitor.isCancelled()) {
            Instruction ins = it.next();
            boolean hit = false;
            for (int i = 0; i < ins.getNumOperands() && !hit; i++)
                for (Object o : ins.getOpObjects(i))
                    if (o instanceof Scalar && ((Scalar) o).getUnsignedValue() == want) { hit = true; break; }
            if (!hit) continue;
            Function f = getFunctionContaining(ins.getAddress());
            if (f == null || done.contains(f)) continue;
            done.add(f);
            DecompileResults res = dec.decompileFunction(f, 120, monitor);
            out.println("//==== " + f.getName() + " @ " + f.getEntryPoint() + "  (uses 0x" + a[1] + " at " + ins.getAddress() + ")");
            out.println(res.decompileCompleted() ? res.getDecompiledFunction().getC() : "// decompile failed");
        }
        out.close();
        println("dumped " + done.size() + " functions");
    }
}
