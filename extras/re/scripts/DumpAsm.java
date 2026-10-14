// Ghidra headless script: print the instruction listing of functions at the given addresses.
// Args: <outFile> <hexAddr> [hexAddr...]
//@category ttlcd
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.*;
import java.io.PrintWriter;

public class DumpAsm extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] a = getScriptArgs();
        PrintWriter out = new PrintWriter(a[0], "UTF-8");
        for (int i = 1; i < a.length; i++) {
            Function f = getFunctionAt(toAddr(Long.parseLong(a[i], 16)));
            if (f == null) { out.println("; no function at " + a[i]); continue; }
            out.println("; ==== " + f.getName() + " @ " + f.getEntryPoint());
            InstructionIterator it = currentProgram.getListing().getInstructions(f.getBody(), true);
            while (it.hasNext()) {
                Instruction ins = it.next();
                String comment = "";
                for (ghidra.program.model.symbol.Reference r : ins.getReferencesFrom()) {
                    Data d = getDataAt(r.getToAddress());
                    if (d != null && d.hasStringValue()) comment = "  ; " + d.getDefaultValueRepresentation();
                }
                out.println(ins.getAddress() + "  " + ins + comment);
            }
        }
        out.close();
    }
}
