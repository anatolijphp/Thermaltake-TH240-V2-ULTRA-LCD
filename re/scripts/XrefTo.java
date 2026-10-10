import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.symbol.*;
import ghidra.program.model.listing.*;
public class XrefTo extends GhidraScript {
  public void run() throws Exception {
    String[] a=getScriptArgs();
    for(int i=1;i<a.length;i++){
      long off=Long.parseLong(a[i],16);
      Address t=toAddr(off);
      println("== refs to "+a[i]);
      ReferenceIterator it=currentProgram.getReferenceManager().getReferencesTo(t);
      while(it.hasNext()){ Reference r=it.next(); Function f=getFunctionContaining(r.getFromAddress());
        println("  "+r.getFromAddress()+" "+(f==null?"?":f.getName())+" "+r.getReferenceType()); }
    }
  }
}
