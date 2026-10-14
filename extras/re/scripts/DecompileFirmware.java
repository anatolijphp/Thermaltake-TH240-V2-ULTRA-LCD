// Ghidra headless post-script: finish the analysis of the LX800 LCD firmware and write it out as C.
// Args: <outDir> <namesFile> <codeEnd>
//   outDir     firmware/re-src: code/*.c (decompiled functions by address range), functions.csv, strings.txt
//   namesFile  "<address> <name> [# comment]" lines; functions are renamed, other addresses labelled
//   codeEnd    first address after the code (the data, fonts and images, follow it)
// The program is imported raw (BinaryLoader, ARM:LE:32:v5t, base 0xFFFE0) by extras/re/decompile-fw.sh.
//@category ttlcd
import ghidra.app.cmd.disassemble.DisassembleCommand;
import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSet;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.SourceType;
import java.io.File;
import java.io.PrintWriter;
import java.nio.file.Files;
import java.util.*;

public class DecompileFirmware extends GhidraScript {
    /** Address ranges the code uses outside the image: the rest of SDRAM and the peripheral registers. */
    private void addMemory() throws Exception {
        Memory mem = currentProgram.getMemory();
        long imgStart = mem.getMinAddress().getOffset(), imgEnd = mem.getMaxAddress().getOffset() + 1;
        long[][] blocks = { { 0x0, imgStart }, { imgEnd, 0x02000000 }, { 0xB0000000L, 0xB9000000L } };
        String[] names = { "sdram_low", "sdram", "io" };
        for (int i = 0; i < blocks.length; i++) {
            Address a = toAddr(blocks[i][0]);
            if (mem.getBlock(a) != null) continue;
            MemoryBlock b = mem.createUninitializedBlock(names[i], a, blocks[i][1] - blocks[i][0], false);
            b.setWrite(true);
            if (names[i].equals("io")) b.setVolatile(true);
        }
    }

    /** The code (with its literal pools) is constant: read-only, so the decompiler resolves "load the address
     *  of a global from the pool" to that global instead of showing the pool entry. The rest of the image (fonts,
     *  initialised data, zeroed variables) stays writable. */
    private void codeReadOnly(long codeEnd) throws Exception {
        Memory mem = currentProgram.getMemory();
        MemoryBlock b = mem.getBlock(toAddr(codeEnd));
        if (b != null && !b.getStart().equals(toAddr(codeEnd))) mem.split(b, toAddr(codeEnd));
        MemoryBlock code = mem.getBlock(toAddr(codeEnd - 1));
        code.setWrite(false);
        code.setName("code");
    }

    /** NUL-terminated texts in the code area (debug messages, paths, formats) as strings, so calls show them. */
    private int defineStrings(long start, long end) throws Exception {
        Memory mem = currentProgram.getMemory();
        int n = 0;
        long a = start;
        while (a < end) {
            long b = a;
            while (b < end) {
                int c = mem.getByte(toAddr(b)) & 0xff;
                if (c == 0 || !(c >= 0x20 && c < 0x7f || c == '\n' || c == '\r' || c == '\t' || c >= 0xc2)) break;
                b++;
            }
            boolean nul = b < end && mem.getByte(toAddr(b)) == 0;
            // short ones too ("%d", "C ", "°") where they fill a 4-byte-aligned literal-pool word
            boolean shortOk = b - a >= 1 && a % 4 == 0 && (b + 1 + 3) / 4 * 4 - (b + 1) < 4 && allZero(mem, b, (b + 4) / 4 * 4);
            if (nul && (b - a >= 4 || shortOk) && looksLikeText(mem, a, b) && getInstructionContaining(toAddr(a)) == null && getDataContaining(toAddr(a)) == null) {
                try {
                    createAsciiString(toAddr(a));
                    n++;
                } catch (Exception e) {
                    // overlaps something already defined
                }
            }
            a = Math.max(b + 1, a + 1);
        }
        return n;
    }

    /** Valid UTF-8 and mostly printable ASCII (binary data passes the byte test now and then). */
    private boolean looksLikeText(Memory mem, long a, long b) throws Exception {
        byte[] bytes = new byte[(int) (b - a)];
        mem.getBytes(toAddr(a), bytes);
        try {
            java.nio.charset.StandardCharsets.UTF_8.newDecoder()
                .onMalformedInput(java.nio.charset.CodingErrorAction.REPORT)
                .decode(java.nio.ByteBuffer.wrap(bytes));
        } catch (java.nio.charset.CharacterCodingException e) {
            return false;
        }
        int ascii = 0, letters = 0;
        for (byte x : bytes) {
            if (x >= 0x20 && x < 0x7f || x == '\n' || x == '\r' || x == '\t') ascii++;
            if (Character.isLetterOrDigit(x) || x == '%') letters++;
        }
        return bytes.length <= 2 || ascii * 10 >= bytes.length * 9 && letters * 2 >= bytes.length;
    }

    private boolean allZero(Memory mem, long from, long to) throws Exception {
        for (long x = from; x < to; x++) if (mem.getByte(toAddr(x)) != 0) return false;
        return true;
    }

    /** The settings record (0x114 bytes, docs/protocol.md "Config record") as a type on g_config. */
    private void configType(Map<Address, String[]> names) throws Exception {
        Address at = null;
        for (Map.Entry<Address, String[]> e : names.entrySet()) if (e.getValue()[0].equals("g_config")) at = e.getKey();
        if (at == null) return;
        StructureDataType t = new StructureDataType("lx_config", 0x114);
        DataType u8 = ByteDataType.dataType, u32 = UnsignedIntegerDataType.dataType, i32 = IntegerDataType.dataType;
        t.replaceAtOffset(0x000, u8, 1, "brightness", "0-100; bit 0x80 = backlight off (cmds 0x12, 0x10)");
        t.replaceAtOffset(0x001, u8, 1, "rotation", "code 0-3 (cmd 0x11, query 0x81)");
        t.replaceAtOffset(0x002, u8, 1, "boot_frame_interval", "cmd 0x14");
        t.replaceAtOffset(0x003, new ArrayDataType(CharDataType.dataType, 0x78, 1), 0x78, "boot_path", "c:\\syst\\boot");
        t.replaceAtOffset(0x07b, new ArrayDataType(CharDataType.dataType, 0x7d, 1), 0x7d, "idle_path", "c:\\syst\\wapper.jpg");
        t.replaceAtOffset(0x0f8, u32, 4, "text_colour", "built-in screens' text colour (cmd 0x16); default 0xCCCCCCCC");
        t.replaceAtOffset(0x0fc, i32, 4, "idle_text_x", "x offset of the idle temperature (default 0)");
        t.replaceAtOffset(0x100, i32, 4, "idle_text_y", "y of the idle temperature (default 100)");
        t.replaceAtOffset(0x104, u32, 4, "idle_hide", "bit 0: no picture, bit 1: no temperature (idle_screen_proc)");
        t.replaceAtOffset(0x108, u32, 4, "unknown_108", "default 5000");
        t.replaceAtOffset(0x10c, u32, 4, "unknown_10c", "default 5000");
        t.replaceAtOffset(0x110, u8, 1, "temp_scale", "0 = °C, else °F (cmd 0x1A, query 0x83)");
        t.replaceAtOffset(0x111, u8, 1, "idle_state", "0x02 = dim after idle (sleep), 0x00 = stay on (cmd 0x1E)");
        DataType dt = currentProgram.getDataTypeManager().addDataType(t, DataTypeConflictHandler.REPLACE_HANDLER);
        clearListing(at, at.add(0x113));
        createData(at, dt);
    }

    /** Every ARM "push {..., lr}" in the code becomes a function (the analyser misses those only reached
     *  through tables and pointers). */
    private int seedFunctions(long start, long end) throws Exception {
        Memory mem = currentProgram.getMemory();
        int n = 0;
        for (long a = start; a + 4 <= end; a += 4) {
            if (monitor.isCancelled()) break;
            Address ad = toAddr(a);
            int w = mem.getInt(ad);
            if ((w & 0xFFFF4000) != 0xE92D4000) continue;
            if (getFunctionContaining(ad) != null && getFunctionAt(ad) == null) continue; // inside another one
            if (getInstructionAt(ad) == null) {
                new DisassembleCommand(ad, null, true).applyTo(currentProgram, monitor);
            }
            if (getInstructionAt(ad) != null && getFunctionAt(ad) == null && createFunction(ad, null) != null) n++;
        }
        return n;
    }

    private Map<Address, String[]> readNames(String file) throws Exception {
        Map<Address, String[]> m = new LinkedHashMap<>();
        for (String line : Files.readAllLines(new File(file).toPath())) {
            String l = line.trim();
            if (l.isEmpty() || l.startsWith("#")) continue;
            String comment = "";
            int h = l.indexOf('#');
            if (h >= 0) { comment = l.substring(h + 1).trim(); l = l.substring(0, h).trim(); }
            String[] p = l.split("\\s+");
            m.put(toAddr(Long.decode(p[0])), new String[] { p[1], comment });
        }
        return m;
    }

    private void applyNames(Map<Address, String[]> names) throws Exception {
        for (Map.Entry<Address, String[]> e : names.entrySet()) {
            Address a = e.getKey();
            String name = e.getValue()[0], comment = e.getValue()[1];
            Function f = getFunctionAt(a);
            if (f == null && getInstructionAt(a) != null) f = createFunction(a, null);
            if (f != null) f.setName(name, SourceType.USER_DEFINED);
            else createLabel(a, name, true, SourceType.USER_DEFINED);
            if (!comment.isEmpty()) setPlateComment(a, comment);
        }
    }

    private static String hex(Address a) { return String.format("%08x", a.getOffset()); }

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        File out = new File(args[0]);
        Map<Address, String[]> names = readNames(args[1]);
        long codeEnd = Long.decode(args[2]);
        long imgStart = currentProgram.getMemory().getMinAddress().getOffset();

        addMemory();
        codeReadOnly(codeEnd);
        Address reset = toAddr(0x00100000);
        currentProgram.getSymbolTable().addExternalEntryPoint(reset);
        new DisassembleCommand(reset, null, true).applyTo(currentProgram, monitor);
        int seeded = seedFunctions(0x00100000, codeEnd);
        analyzeAll(currentProgram);
        seeded += seedFunctions(0x00100000, codeEnd); // what the analysis left
        int strings = defineStrings(imgStart, codeEnd);
        analyzeChanges(currentProgram);
        applyNames(names);
        configType(names);
        println("functions seeded: " + seeded + ", strings defined: " + strings);

        File codeDir = new File(out, "code");
        codeDir.mkdirs();
        for (File old : Objects.requireNonNull(codeDir.listFiles())) if (old.getName().endsWith(".c")) old.delete();

        DecompInterface dec = new DecompInterface();
        DecompileOptions opt = new DecompileOptions();
        dec.setOptions(opt);
        dec.toggleCCode(true);
        dec.toggleSyntaxTree(false);
        dec.openProgram(currentProgram);

        List<Function> funcs = new ArrayList<>();
        for (Function f : currentProgram.getFunctionManager().getFunctions(true)) funcs.add(f);

        PrintWriter csv = new PrintWriter(new File(out, "functions.csv"), "UTF-8");
        csv.println("address,name,size,called_by,calls,file");
        final long CHUNK = 0x4000;
        PrintWriter cur = null;
        long curChunk = -1;
        int ok = 0, failed = 0;
        for (Function f : funcs) {
            if (monitor.isCancelled()) break;
            long off = f.getEntryPoint().getOffset();
            long chunk = off / CHUNK * CHUNK;
            String fileName = String.format("%08x.c", chunk);
            if (chunk != curChunk) {
                if (cur != null) cur.close();
                cur = new PrintWriter(new File(codeDir, fileName), "UTF-8");
                cur.printf("// LX800 LCD firmware (firmware/lx800_tt021panel.bin), functions at 0x%08x-0x%08x.%n", chunk, chunk + CHUNK - 1);
                cur.println("// Decompiled by Ghidra (extras/re/decompile-fw.sh); names from firmware/re-src/names.txt.");
                cur.println("// Pseudo-C for reading: it does not compile, and FUN_/DAT_ names are Ghidra's.");
                cur.println();
                curChunk = chunk;
            }
            Set<Function> callers = f.getCallingFunctions(monitor), callees = f.getCalledFunctions(monitor);
            StringBuilder cb = new StringBuilder();
            for (Function c : callers) cb.append(cb.length() > 0 ? ", " : "").append(c.getName());
            cur.println("// ==== " + f.getName() + " @ " + hex(f.getEntryPoint()) + ", " + f.getBody().getNumAddresses() + " bytes");
            if (cb.length() > 0) cur.println("// called by: " + cb);
            String plate = getPlateComment(f.getEntryPoint());
            if (plate != null) cur.println("// " + plate.replace("\n", "\n// "));
            DecompileResults r = dec.decompileFunction(f, 90, monitor);
            if (r.decompileCompleted()) {
                cur.println(r.getDecompiledFunction().getC().trim());
                ok++;
            } else {
                cur.println("// decompile failed: " + r.getErrorMessage());
                failed++;
            }
            cur.println();
            csv.printf("0x%08x,%s,%d,%d,%d,code/%s%n", off, f.getName(), f.getBody().getNumAddresses(), callers.size(), callees.size(), fileName);
        }
        if (cur != null) cur.close();
        csv.close();

        PrintWriter strs = new PrintWriter(new File(out, "strings.txt"), "UTF-8");
        strs.println("# address  file-offset  text   (defined strings in firmware/lx800_tt021panel.bin)");
        for (Data d : currentProgram.getListing().getDefinedData(true)) {
            StringDataInstance s = StringDataInstance.getStringDataInstance(d);
            if (s == StringDataInstance.NULL_INSTANCE) continue;
            String v = s.getStringValue();
            if (v == null || v.trim().length() < 3) continue;
            strs.printf("%s  0x%05x  %s%n", hex(d.getAddress()), d.getAddress().getOffset() - imgStart,
                v.replace("\n", "\\n").replace("\r", "\\r"));
        }
        strs.close();
        println("decompiled " + ok + " functions, " + failed + " failed, " + funcs.size() + " in all");
    }
}
