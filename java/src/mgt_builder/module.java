
//    FPGA MGT builder quickly builds complex configurations with multiple MGTs
//    2020 Alex Madorsky, University of Florida/Physics
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

package mgt_builder;

//import static com.mycompany.fpga_quad_instantiator.quad_config.*;
//import static com.mycompany.fpga_quad_instantiator.fpga.qc;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.*;

/**
 *
 * @author madorsky
 */
public class module
{
    
    quad_config qc;
    module (quad_config qcp)
    {
        qc = qcp;
    };
    
    static String logic_1 = 
"    reg         int_reg_rdy;\n" +
"    reg  [31:0] drpdo_reg;\n" +
"    wire        drprdy_mgt;\n" +
"    wire [31:0] drpdo_mgt;\n" +
"    reg         drprdy_reg;\n" +
"    assign ###.drprdy = drprdy_reg;\n" +
"    assign ###.drpdo  = drpdo_reg;\n" +
"    always @(posedge drpclk)\n" +
"    begin\n" +
"        drprdy_reg = 1'b0;\n" +
"		drpdo_reg = 32'h0;\n" +
"        if (###.int_reg == 1'b1)\n" +
"        begin\n" +
"            if (###.drpwe == 1'b1)\n" +
"            begin\n" +
"                case (###.drpaddr)\n";

    
    static String logic_2 = 
"                endcase\n" +
"                drprdy_reg = 1'b1;\n" +
"            end\n" +
"            else\n" +
"            begin\n" +
"                case (###.drpaddr)\n";
    
    static String logic_3 = 
"                    9'h1ff: drpdo_reg = XY; // each MGT reports its XY\n" +
"                endcase\n" +
"                drprdy_reg = 1'b1;\n" +
"            end\n" +
"        end\n" +
"\n" +
"        if (drprdy_mgt == 1'b1)\n" +
"        begin\n" +
"            drpdo_reg = drpdo_mgt;\n" +
"            drprdy_reg = 1'b1;\n" +
"        end\n" +
"    end\n";

    String name;
    String prefix;
    int reg_base;
    int reg_last;
    int wire_base;
    int wire_last;
    int offset = 0;
    int base_addr = 0;
    List<port> port_lst, mgt_rx_lst, mgt_tx_lst, drp_lst; //, phalg_tx_lst;
    
    List<String> kill_last_comma (List<String> s)
    {
        if (s.size() > 0)
        {
            String last_s = s.get(s.size()-1).replace(",", "");
            s.set(s.size()-1, last_s);
        }
        return s;
    };
    
    
    void print ()
    {
        System.out.printf("reg base: %08x wire base: %08x");
        System.out.println();
    };

    void fill_ports (String[] lst)
    {
        port_lst = new ArrayList<>();
        mgt_rx_lst = new ArrayList<>();
        mgt_tx_lst = new ArrayList<>();
//        phalg_tx_lst = new ArrayList<>();
        drp_lst = new ArrayList<>();
        for (String s : lst)
        {
            if (s.startsWith("\t")) s = s.replaceFirst("\t", ""); // remove tabs only at the start
            String[] sf = s.split("\t", -1); // split into fields
            
            if (!sf[0].contains("//")) // skip commented out line
            {
                port p = new port(qc);
                p.address = new reg_addr();
                p.name = sf[0].trim();
                if (p.name.isEmpty()) continue; // at least the name must be present
                p.dir = sf[1].trim();
                p.type = sf[2].trim();
                p.clock = sf[3].trim();
                try {p.bits = Integer.parseInt(sf[4].trim());} catch (Exception e) 
                {
                    System.out.println (String.format("fill_ports illegal bits: %s %s %s %s %s", sf[0], sf[1], sf[2], sf[3], sf[4]));
                    continue; // cannot convert number of bits
                    
                }
                p.dflt = sf[5].trim();
                p.iface = sf[6].trim();
                p.indexed = !sf[7].trim().isEmpty();
                p.buffer = sf[8];
                p.quad_int = false; // not internal by default
                
                // add to port list and one of the interfaces
                if      (p.iface.contentEquals(qc.mgt_rx_iface))   mgt_rx_lst.add(p);
                else if (p.iface.contentEquals(qc.mgt_tx_iface))   mgt_tx_lst.add(p);
//                else if (p.iface.contentEquals(qc.phalg_tx_iface)) phalg_tx_lst.add(p);
                else if (p.iface.contentEquals(qc.drp_iface))      drp_lst.add(p);
                port_lst.add(p);
            }
        }
        
        // create an extra int_reg port for drp interface
        port irp = new port(qc);
        irp.address = new reg_addr();
        irp.name = "int_reg";
        irp.dir = "in";
        irp.type = "drp";
        irp.clock = "DRPCLK";
        irp.bits = 1;
        irp.dflt = "";
        irp.iface = "drp";
        irp.indexed = false;
        irp.quad_int = false; 
        drp_lst.add(irp);
        
    };

    int assign_addresses (String dir, int start_addr)
    {
        int ca = start_addr; // current address
        int cb = 0; // current bit
        int start_offset = offset;
        
        for (port p : port_lst)
        {
            if (p.dir.contentEquals(dir) && (p.type.contentEquals("s") || p.type.contentEquals("p"))) // inputs
            {
                int low_bit = cb;
                int high_bit = cb + p.bits - 1;
                if (high_bit >= qc.reg_width_bits) // out of bits in this word, move to next word
                {
                    ca += 1; // take next register
                    offset += 1; // take next register
                    cb = 0; // start with bit 0
                    low_bit = cb;
                    high_bit = cb + p.bits - 1; // should be no ports wider than 32 bits
                }
                cb += p.bits; // update current bit
                // create bit mask
                int mask = 0;
                for (int i = 0; i < p.bits; i++) mask = (mask << 1) | 1;
                mask <<= low_bit;
                p.address.offset = offset;
                p.address.reg  = ca;
                p.address.mask = mask;
                p.address.low_bit = low_bit;
                p.address.high_bit = high_bit;
                
            }
        }

        if (dir.contentEquals("in"))
        {
            reg_base = start_offset;
            reg_last = offset;
        }
        else
        {
            wire_base = start_offset;
            wire_last = offset;
        }
        ca += 1;
        offset += 1;
        return ca;
    };
    void write_module (String fname)
    {
        List<String> lines = new ArrayList<>();
        List<String> module_ios = new ArrayList<>(); // module IO ports
        List<String> inst_ios = new ArrayList<>(); // instance ports
        List<String> wires = new ArrayList<>(); // wire instantiations
        List<String> regs  = new ArrayList<>(); // reg instantiations
        List<String> case_wr = new ArrayList<>(); // write case items
        List<String> case_rd = new ArrayList<>(); // read case items
        List<String> inst_buffers = new ArrayList<>(); // buffers
        
        String dirc, bitfld, ifp_name; // signal
        
        // add mandatory interfaces to module_ios
        module_ios.add (String.format("\t%s.in %s_if,", qc.drp_iface, qc.drp_iface));
        if (mgt_tx_lst.size() > 0) module_ios.add (String.format("\t%s.in %s_if,", qc.mgt_tx_iface, qc.mgt_tx_iface));
        if (mgt_rx_lst.size() > 0) module_ios.add (String.format("\t%s.in %s_if,", qc.mgt_rx_iface, qc.mgt_rx_iface));
//        if (phalg_tx_lst.size() > 0) module_ios.add (String.format("\t%s.in %s_if,", qc.phalg_tx_iface, qc.phalg_tx_iface));
        
        // scan ports, generate lists
        for (port p : port_lst)
        {
            // name with interface, if any
            if (p.iface.isEmpty()) ifp_name = p.name;
            else ifp_name = p.iface + "_if." + p.name;
            
            // prepare declarations
            if (p.dir.contentEquals("in"))
            {
                //signal = "reg ";
                dirc = "input ";
            }
            else
            {
                //signal = "wire";
                dirc = "output";
            }
            // prepare bit fields
            if (p.bits > 1) bitfld = String.format("[%2d:0]", p.bits-1);
            else bitfld = "      ";
            
            // module IO list
            if (p.type.contentEquals("d") || // dynamic
                p.type.contentEquals("c") || // clock
//                p.type.contentEquals("p") || // pulse counter, temporary
                p.type.contentEquals("drp")) // DRP
            {
                if (p.iface.isEmpty()) // only add port to module IO if it's not part of an interface
                    module_ios.add(String.format("\t%s %s %s,",
                        dirc, bitfld, p.name.toLowerCase()));
            }
            
            // instance IO list
            
            if (p.type.contentEquals("0")) // hardcoded inputs, unconnected outputs
            {
                if (p.dir.contentEquals("in"))
                {
                    if (p.dflt.trim().contentEquals("")) // no default indicated
                        inst_ios.add (String.format("\t\t.%-26s (%d'b0),", 
                            p.name, p.bits)); // put 0 default
                    else
                        inst_ios.add (String.format("\t\t.%-26s (%s),", 
                            p.name, p.dflt)); // put indicated default
                }   
                else
                {
                    // unconnected output
                    inst_ios.add (String.format("\t\t.%-26s (),", 
                            p.name));
                }
            }
            else
            if (p.type.contentEquals("s")) // static connection to register
            {
                if (p.dir.contentEquals("in"))
                    inst_ios.add (String.format("\t\t.%-26s ( reg_[%d][%2d:%2d]),", 
                        p.name, p.address.offset, p.address.high_bit, p.address.low_bit));
                else
                    inst_ios.add (String.format("\t\t.%-26s (wire_[%d][%2d:%2d]),", 
                        p.name, p.address.offset, p.address.high_bit, p.address.low_bit));
                    
            }
            else
            if (p.name.contentEquals("DRPRDY")) // special case of drprdy signal
            {
                inst_ios.add (String.format("\t\t.%-26s (%s),", 
                        p.name, p.name.toLowerCase()+"_mgt"));
            }
            else
            if (p.name.startsWith("DRPDI")) // special case of drpd[io] signals
            {
                inst_ios.add (String.format("\t\t.%-26s (%s [15:0]),", 
                        p.name, ifp_name.toLowerCase()));
            }
            else // dynamic, clock, pulse, or drp
            if (p.name.startsWith("DRPDO")) // special case of drpd[io] signals
            {
                inst_ios.add (String.format("\t\t.%-26s (%s [15:0]),", 
                        p.name, p.name.toLowerCase()+"_mgt"));
            }
            else if (!p.buffer.isEmpty())
            {
                // buffered connection
                // add just the name to the IO list
                inst_ios.add (String.format("\t\t.%-26s (%-26s),", 
                        p.name, (p.name.toLowerCase()) + "_loc"));

                if (p.dir.contentEquals("out"))
                {
                    // add buffer instance 
                    inst_buffers.add (String.format("\t%s %s_buf (.O(%s), .I(%s));", 
                            p.buffer, p.name.toLowerCase(), ifp_name.toLowerCase(), (p.name.toLowerCase() + "_loc")));
                }
                else
                {
                    // add buffer instance 
                    inst_buffers.add (String.format("\t%s %s_buf (.O(%s), .I(%s));", 
                            p.buffer, p.name, (p.name.toLowerCase() + "_loc"), ifp_name.toLowerCase()));
                }
            }
            else // dynamic, clock, pulse, or drp
            {
                inst_ios.add (String.format("\t\t.%-26s (%-26s),", 
                        p.name, ifp_name.toLowerCase()));
            }
        }
        
        // remove comma after last IO declaration
        module_ios = kill_last_comma (module_ios);
        inst_ios   = kill_last_comma (inst_ios);

        String module_params = "#(parameter [7:0] qind = 0, parameter [7:0] mind = 0)";

        // wires and regs declarations
        regs.add  (String.format("\treg  [31:0] reg_  [%2d:%2d];", reg_last,  reg_base));
        wires.add (String.format("\twire [31:0] wire_ [%2d:%2d];", wire_last, wire_base));
        
        // registers added to both write and read logic
        for (int i = reg_base; i <= reg_last; i++)
        {
            case_wr.add (String.format("                    9'd%d: reg_[%d] = %s.drpdi;", i, i, qc.drp_iface + "_if"));
            case_rd.add (String.format("                    9'd%d: drpdo_reg = reg_[%d];", i, i));
        }
        
        // wires added only to read logic
        for (int i = wire_base; i <= wire_last; i++)
        {
            case_rd.add (String.format("                    9'd%d: drpdo_reg = wire_[%d];", i, i));
        }
        
        lines.add    (qc.generated_header);
        lines.add    ("`include \"drp_interface.sv\"");
        lines.add    ("`include \"" + qc.interfaces_fname + "\"");
        lines.add    ("module " + name + "_tux"); // module header
        lines.add    (module_params);
        lines.add    ("(");
        lines.addAll (module_ios);
        lines.add    (");");
        lines.add    ("`include \"" + qc.xymap_fn + "\"");
        lines.add    ("\twire [15:0] XY = mgtxy[qind][mind];");
        lines.addAll (regs);
        lines.addAll (wires);
        lines.add    (logic_1.replaceAll("###", qc.drp_iface + "_if"));
        lines.addAll (case_wr);
        lines.add    (logic_2.replaceAll("###", qc.drp_iface + "_if"));
        lines.addAll (case_rd);
        lines.add    (logic_3.replaceAll("###", qc.drp_iface + "_if"));
        lines.addAll (inst_buffers);
        lines.add    (String.format("\t%s %s_inst\n\t(", name, name));
        lines.addAll (inst_ios);
        lines.add    ("\t);");
        lines.add    ("endmodule");
        
        Path file = Paths.get(fname);
        try
        {
            Files.write(file, lines, StandardCharsets.UTF_8);
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    };
    
    List<String> write_inst (String gen_ind, String gen_ind_drp)
    {
        List<String> lines = new ArrayList<>();
        List<String> inst_ios = new ArrayList<>(); // module IO ports
        String indent = "";
        if (!gen_ind.isEmpty()) indent = "\t\t";

        // add mandatory interfaces to module_ios
        inst_ios.add (String.format("%s\t\t.%s_if (%s_if[%s]),", indent, qc.drp_iface, qc.drp_iface, gen_ind_drp));
        inst_ios.add (String.format("%s\t\t.drpclk (drpclk),", indent));
        if (mgt_tx_lst.size() > 0) inst_ios.add (String.format("%s\t\t.%s_if (%s_if[%s]),", indent, qc.mgt_tx_iface, qc.mgt_tx_iface, gen_ind));
        if (mgt_rx_lst.size() > 0) inst_ios.add (String.format("%s\t\t.%s_if (%s_if[%s]),", indent, qc.mgt_rx_iface, qc.mgt_rx_iface, gen_ind));
//        if (phalg_tx_lst.size() > 0) inst_ios.add (String.format("%s\t\t.%s_if (%s_if[%s]),", indent, qc.phalg_tx_iface, qc.phalg_tx_iface, gen_ind));

        
        // scan ports, generate list
        for (port p : port_lst)
        {
            String pref = prefix;
            String gi = gen_ind;
            if (p.type.contentEquals("d") || // dynamic
//                p.type.contentEquals("p") || // pulse
                p.type.contentEquals("c"))   // clock
            {
                
                if (p.iface.isEmpty()) // only if not part of an interface
                {
                    if (p.indexed && !gi.isEmpty())
                    {
                        inst_ios.add (String.format("%s\t\t.%-26s (%-26s [%s]),", 
                                indent, p.name.toLowerCase(), pref+p.name.toLowerCase(), gi));
                    }
                    else
                    {
                        inst_ios.add (String.format("%s\t\t.%-26s (%-26s),", 
                                indent, p.name.toLowerCase(), pref+p.name.toLowerCase()));
                    }
                }
            }
        }

        // remove last comma
        inst_ios   = kill_last_comma (inst_ios);
        
        lines.add    (indent + "\t" + name + "_tux  #(.qind(ind), .mind(gi)) " + name + "_tux_inst\n" + indent + "\t(");
        lines.addAll (inst_ios);
        lines.add    (indent + "\t);");
        
        return lines;
        
    };

    List<String> write_iface (List<port> ports, String name)
    {
        List<String> lines = new ArrayList<>();
        
        List<String> logic = new ArrayList<>(); 
        List<String> modport_in  = new ArrayList<>(); 
        List<String> modport_out = new ArrayList<>(); 

        String bitfld = "";
        
        for (port p: ports)
        {
            if (p.bits > 1) bitfld = String.format("[%2d:0]", p.bits-1);
            else bitfld = "      ";
            logic.add (String.format("\tlogic %s %s;", bitfld, p.name.toLowerCase()));
            modport_in.add  (String.format("\t\t%-6s %s,", p.dir + "put", p.name.toLowerCase()));
           
            String diropp = "input"; // reverse port directions for modport out
            if (p.dir.contentEquals("in")) diropp = "output";
           
            modport_out.add (String.format("\t\t%-6s %s,", diropp, p.name.toLowerCase()));
        }

        modport_in = kill_last_comma (modport_in);
        modport_out = kill_last_comma (modport_out);
        
        lines.add    ("interface " + name + ";");
        lines.addAll (logic);
        lines.add    ("\tmodport in\n\t(");
        lines.addAll (modport_in);
        lines.add    ("\t);");
        lines.add    ("\tmodport out\n\t(");
        lines.addAll (modport_out);
        lines.add    ("\t);\nendinterface\n");
        
        return lines;

    };

    void write_ifaces (String fname)
    {
        List<String> lines = new ArrayList<>();
        
        lines.add    (qc.generated_header);
        lines.add    ("`ifndef MGT_INTERFACES_SV");
        lines.add    ("`define MGT_INTERFACES_SV");
        lines.addAll (write_iface (mgt_rx_lst, qc.mgt_rx_iface));
        lines.addAll (write_iface (mgt_tx_lst, qc.mgt_tx_iface));
//        lines.addAll (write_iface (phalg_tx_lst, "phalg_tx"));
        // drp interface is constructed manually for now
        //lines.addAll (write_iface (drp_lst,    "drp"));

        lines.add    ("`endif");
        
        Path file = Paths.get(fname);
        try
        {
            Files.write(file, lines, StandardCharsets.UTF_8);
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    };

    void write_iface_assignment (List<port> ports, String iface_name)
    {
        List<String> lines = new ArrayList<>();
        List<String> assignments = new ArrayList<>();
        
        for (port p: ports)
        {
            if (p.dir.contentEquals("in"))
                assignments.add (String.format("\t assign dest.%s = src.%s;", p.name.toLowerCase(), p.name.toLowerCase()));
            else
                assignments.add (String.format("\t assign src.%s = dest.%s;", p.name.toLowerCase(), p.name.toLowerCase()));
        }
        
        lines.add    (qc.generated_header);
        lines.add    ("`include \"" + qc.interfaces_fname + "\"");
        lines.add    ("module assign_" + iface_name + "\n(");
        lines.add    ("\t" + iface_name + ".out dest,");
        lines.add    ("\t" + iface_name + ".in src");
        lines.add    (");");
        lines.addAll (assignments);
        lines.add    ("endmodule");

        Path file = Paths.get(qc.sv_path + "/assign_" + iface_name + ".sv");
        try
        {
            Files.write(file, lines, StandardCharsets.UTF_8);
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
        
    };
    
    void write_offset_table (String fname)
    {
        List<String> lines = new ArrayList<>();
        List<String> rw = new ArrayList<>(); // read/write
        List<String> ro = new ArrayList<>(); // read only
        

        for (port p: port_lst)
        {
            if (p.type.contentEquals("s")) 
            {
                String te = p.offset_tab_entry();

                if (p.dir.contentEquals("in")) rw.add(te);
                else                           ro.add(te);
            }
        }

        //lines.add   ("//offset\tlsb\tmsb\trw\tname");
        lines.add   ("//Address       bits    rw      Name    att_bits        att_enc bin_enc");
        lines.addAll(rw);
        lines.addAll(ro);
        lines.add   ("001ff\t15:0\tR\tXY_COORD\t15:0\tx-x\tx-x"); // XY port, created in firmware
        
        Path file = Paths.get (fname);
        try
        {
            Files.write(file, lines, StandardCharsets.UTF_8);
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    }
}
