
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
import java.util.*;
import java.nio.charset.StandardCharsets; 
import java.nio.file.*; 

/**
 *
 * @author madorsky
 */


public class quad 
{
    String name;
    quad_config qc;
    
    quad (quad_config qcp)
    {
        qc = qcp;
        drp_addr_inc = 1 << qc.max_drp_addr_width;
    }
    
    // drp address increment from module to module
    final int drp_addr_inc;  
    
    module[] mgt_inst;
    module[] common_inst;
    List<port> port_lst;
    int aw_quad_bits = 0; 

    int get_aw_quad_bits(){return aw_quad_bits;}
    
    List<String> kill_last_comma (List<String> s)
    {
        String last_s = s.get(s.size()-1).replace(",", "");
        s.set(s.size()-1, last_s);
        return s;
    };

    void init_module()
    {
        mgt_inst = new module[qc.mgts_per_quad];
        // initialize modules
        for (int i = 0; i < qc.mgts_per_quad; i++)
        {
            mgt_inst[i] = new module(qc);
            mgt_inst[i].reg_base = 0;
            mgt_inst[i].wire_base = 0;
            mgt_inst[i].name = qc.mgt_channel_name;
            mgt_inst[i].prefix = "mgt_";
        }
        
        common_inst = new module[qc.commons_per_quad];
        for (int i = 0; i < qc.commons_per_quad; i++)
        {
            common_inst[i] = new module(qc);
            common_inst[i].reg_base = 0;
            common_inst[i].wire_base = 0;
            common_inst[i].name = qc.mgt_common_name;
            common_inst[i].prefix = "com_";
        }
    };
    
    
    void print()
    {
        for (port p: common_inst[0].port_lst)
        {
            if (p.type.contentEquals("s") && p.dir.contentEquals("in")) p.print();
        }
        System.out.println();
        for (port p: common_inst[0].port_lst)
        {
            if (p.type.contentEquals("s") && p.dir.contentEquals("out")) p.print();
        }

        System.out.println();
        System.out.println();

        for (port p: mgt_inst[3].port_lst)
        {
            if (p.type.contentEquals("s") && p.dir.contentEquals("in")) p.print();
        }
        System.out.println();
        for (port p: mgt_inst[3].port_lst)
        {
            if (p.type.contentEquals("s") && p.dir.contentEquals("out")) p.print();
        }
        
        for (module m : mgt_inst)
        {
            System.out.printf ("MGT base : %03x %03x\n", m.reg_base, m.wire_base);
        }
        System.out.printf ("Common base : %03x %03x\n", common_inst[0].reg_base, common_inst[0].wire_base);
        
    };
    
    List<String> write_inst (String gen_ind, String gen_ind_drp)
    {
        List<String> inst_ios = new ArrayList<>();
        List<String> lines = new ArrayList<>();
        String indent = "\t\t";
        
        inst_ios.add (String.format("%s\t\t.%s_qif (%s_qif [%s]),", indent, qc.drp_iface, qc.drp_iface, gen_ind_drp));
        inst_ios.add (String.format("%s\t\t.drpclk (drpclk),", indent));
        inst_ios.add (String.format("%s\t\t.%s_if (%s_if [%s]),", indent, qc.mgt_tx_iface, qc.mgt_tx_iface, gen_ind));
        inst_ios.add (String.format("%s\t\t.%s_if (%s_if [%s]),", indent, qc.mgt_rx_iface, qc.mgt_rx_iface, gen_ind));
//        inst_ios.add (String.format("%s\t\t.%s_if (%s_if [%s]),", indent, qc.phalg_tx_iface, qc.phalg_tx_iface, gen_ind));

        
        for (port p: port_lst)
        {
            if (!p.quad_int && p.iface.isEmpty())
            {
                String gi = gen_ind;
                if (p.type.contentEquals("d") || // dynamic
                    //p.type.contentEquals("p") || // pulse
                    p.type.contentEquals("c"))   // clock
                {
                    if (p.indexed && !gen_ind.isEmpty())
                    {
                        inst_ios.add (String.format("%s\t\t.%-26s (%-26s [%s]),", 
                                indent, p.name.toLowerCase(), p.name.toLowerCase(), gi));
                    }
                    else
                    {
                        inst_ios.add (String.format("%s\t\t.%-26s (%-26s),", 
                                indent, p.name.toLowerCase(), p.name.toLowerCase()));
                    }
                }
            }
        }

        // remove last comma
        inst_ios = kill_last_comma (inst_ios);
        
        lines.add    (indent + "\t" + name + " #(.ind(gi)) " + name + "_inst\n" + indent + "\t(");
        lines.addAll (inst_ios);
        lines.add    (indent + "\t);");
        return lines;
    };
    
    void fill_ports ()
    {
        port_lst = new ArrayList<>();

        // assignments inside quad
        for (String s: qc.quad_connections)
        {
            if (s.startsWith("\t")) s = s.replaceFirst("\t", ""); // remove tabs only at the start
            
            if (!s.contains("//")) // skip comments
            {

                String[] sf = s.trim().split("\t");

                // make sure that internally connected signals are not included into IO
                for (int i = 0; i < mgt_inst[0].port_lst.size(); i++)
                {

                    port p = mgt_inst[0].port_lst.get(i); // port we're checking
                    String full_name = mgt_inst[0].prefix + p.name.toLowerCase();
                    // one of the internally-connected signals matches?
                    if (full_name.contentEquals(sf[0]) && p.dir.contentEquals("in"))
                    {
                        mgt_inst[0].port_lst.get(i).quad_int = true; // make signal internal to quad
//                        System.out.println(String.format("make internal signal (input): %s", mgt_inst[0].port_lst.get(i).name));
                    }
                    if (full_name.contentEquals(sf[2]) && p.dir.contentEquals("out"))
                    {
                        mgt_inst[0].port_lst.get(i).quad_int = true; // make signal internal to quad
//                        System.out.println(String.format("make internal signal (output): %s", mgt_inst[0].port_lst.get(i).name));
                    }
                }

                for (int i = 0; i < common_inst[0].port_lst.size(); i++)
                {
                    port p = common_inst[0].port_lst.get(i); // port we're checking
                    String full_name = common_inst[0].prefix + p.name.toLowerCase();
                    // one of the internally-connected signals matches?
                    if (full_name.contentEquals(sf[0]) && p.dir.contentEquals("in"))
                        common_inst[0].port_lst.get(i).quad_int = true; // make signal internal to quad

                    if (full_name.contentEquals(sf[2]) && p.dir.contentEquals("out"))
                        common_inst[0].port_lst.get(i).quad_int = true; // make signal internal to quad
                }
            }
        }

        // create IO ports for quad module
        for (port p : mgt_inst[0].port_lst) // scan MGT first
        {
            port qp = new port(p);
            // prepare bit fields
            if (p.bits > 1)
            {
                if (p.indexed) qp.dim0 = qc.mgts_per_quad;
                else           qp.dim0 = 0;
            }
            else
            {
                if (p.indexed) 
                {
                    qp.bits = qc.mgts_per_quad;
                    qp.dim0 = 0;
                }
                else           qp.bits = 0;
            }
                
            String pref = mgt_inst[0].prefix;
            // module IO list
            if (p.type.contentEquals("d") || // dynamic
                //p.type.contentEquals("p") || // pulse
                p.type.contentEquals("c"))   // clock
            {
                qp.name = pref + p.name.toLowerCase();
                port_lst.add(qp);
            }
        }        
        
        for (port p : common_inst[0].port_lst) // scan COMMON
        {
            port qp = new port(p);
            // prepare bit fields
            if (p.bits > 1)
            {
                if (p.indexed) qp.dim0 = qc.commons_per_quad;
                else           qp.dim0 = 0;
            }
            else
            {
                if (p.indexed) 
                {
                    qp.bits = qc.commons_per_quad;
                    qp.dim0 = 0;
                }
                else           qp.bits = 1;
            }
                
            String pref = common_inst[0].prefix;
            // module IO list
            if (p.type.contentEquals("d") || // dynamic
                //p.type.contentEquals("p") || // pulse
                p.type.contentEquals("c"))   // clock
            {
                qp.name = pref + p.name.toLowerCase();
                port_lst.add(qp);
            }
        }        
    };
    
    void write_quad_file (String fname)
    {
        // create MGT instances
        List<String> lines = new ArrayList<>();
        
        List<String> mgt_inst_ln = mgt_inst[0].write_inst
        (
            (qc.mgts_per_quad > 1) ? "gi" : "", // only give index if more than one MGT
            "gi"
        );
        
        List<String> common_inst_ln = common_inst[0].write_inst
        (
            (qc.commons_per_quad > 1) ? "gi" : "", // only give index if more than one COMMON
            "gi+"+Integer.toString(qc.mgts_per_quad)
        );
        
        String bitfld = "";
        String arrfld = "";
        List<String> connections = new ArrayList<>();
        List<String> wires = new ArrayList<>();
        List<String> ios = new ArrayList<>();

        // add necessary interfaces
        ios.add (String.format("\tdrp.in %s_qif,", qc.drp_iface));
        ios.add ("\tinput drpclk,");
        ios.add (String.format("\t%s.in %s_if [%d:0],", qc.mgt_tx_iface, qc.mgt_tx_iface, qc.mgts_per_quad-1));
        ios.add (String.format("\t%s.in %s_if [%d:0],", qc.mgt_rx_iface, qc.mgt_rx_iface, qc.mgts_per_quad-1));
//        ios.add (String.format("\t%s.in %s_if [%d:0],", qc.phalg_tx_iface, qc.phalg_tx_iface, qc.mgts_per_quad-1));

        // assignments inside quad
        for (String s: qc.quad_connections)
        {
            if (!s.contains("//")) // skip comments
            {
                if (s.startsWith("\t")) s = s.replaceFirst("\t", ""); // remove tabs only at the start
                String[] sf = s.split("\t", -1); // split into fields
                if (sf[4].isEmpty()) // simple assignment
                    connections.add (String.format ("\tassign %s%s = %s%s;", 
                            sf[0], sf[1], sf[2], sf[3]));
                else // via buffer
                    connections.add (String.format ("\t%s (.O(%s%s), .I(%s%s));", 
                            sf[4], sf[0], sf[1], sf[2], sf[3]));
            }                
        }

        for (port p: port_lst)
        {
            if (p.bits > 1)
            {
                bitfld = String.format("[%2d:0]", p.bits-1);
                if (p.dim0 > 0) 
                    arrfld = String.format(" [%2d:0]", p.dim0-1);
                else
                    arrfld = "";
            }
            else
            {
                if (p.dim0 > 0) 
                    bitfld = String.format("[%2d:0]", p.dim0-1);
                else
                    bitfld = "      ";
                arrfld = "";
            }

            
            if (!p.quad_int) // propagate as IO
            {
                if (p.iface.isEmpty()) // only if not part of interface
                    ios.add(String.format("\t%-6s %s %s%s,",
                        p.dir+"put", bitfld, p.name, arrfld));
            }
            else
                wires.add(String.format("\twire %s %s%s;",
                    bitfld, p.name, arrfld));
            
        }
        
        ios = kill_last_comma (ios);
        
        // how many bits is needed for addressing quads and commons
        aw_quad_bits = (int)Math.ceil(Math.log(qc.mgts_per_quad + qc.commons_per_quad)/Math.log(2)) + qc.max_drp_addr_width;
        
        String drp_mux = String.format(
"	drp #(.AW(%d)) %s_if [drpn-1:0]();\n" +
"	drp_mux #(.N(drpn), .AW_FPGA(%d), .AW_QUAD(%d)) drpm (%s_if, %s_qif, drpclk);\n",
                qc.max_drp_addr_width, qc.drp_iface, 
                aw_quad_bits, qc.max_drp_addr_width, qc.drp_iface, qc.drp_iface);

        
        lines.add    (qc.generated_header);
        lines.add    ("`include \"drp_interface.sv\"");
        lines.add    ("`include \"" + qc.interfaces_fname + "\"");
        lines.add    ("module " + name + "\n#(parameter ind = 0)\n("); // module header
        lines.addAll (ios);
        lines.add    (");");
        lines.add    ("\tlocalparam drpn = " + Integer.toString(qc.mgts_per_quad + qc.commons_per_quad) + ";");
        lines.add    (drp_mux);
        lines.addAll (wires);
        lines.add    ("\tgenvar gi;");
        lines.add    ("\tgenerate");
        lines.add    ("\t\tfor (gi = 0; gi < " + qc.mgts_per_quad + "; gi++)");
        lines.add    ("\t\tbegin: mgt_loop");
        lines.addAll (mgt_inst_ln);
        lines.add    ("\t\tend");
        lines.add    ("\t\tfor (gi = 0; gi < " + qc.commons_per_quad + "; gi++)");
        lines.add    ("\t\tbegin: common_loop");
        lines.addAll (common_inst_ln);
        lines.add    ("\t\tend");
        lines.add    ("\tendgenerate");
        lines.addAll (connections);
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
    
    
    public void write_quad () 
    {
        int reg_addr = 0;
        int base_addr = 0;
        
        name = qc.mgt_quad_name;
        
        init_module();
        
        for (int i = 0; i < qc.mgts_per_quad; i++)
        {
            reg_addr = 0;
            mgt_inst[i].fill_ports (qc.mgt_ports);
            reg_addr = mgt_inst[i].assign_addresses ("in", reg_addr);
            reg_addr = mgt_inst[i].assign_addresses ("out", reg_addr);
            // assign base addr within quad
            mgt_inst[i].base_addr = base_addr;
            base_addr += (1 << qc.max_drp_addr_width);
        }
        
        for (int i = 0; i < qc.commons_per_quad; i++)
        {
            reg_addr = 0;
            common_inst[i].fill_ports (qc.common_ports);
            reg_addr = common_inst[i].assign_addresses ("in", reg_addr);
            reg_addr = common_inst[i].assign_addresses ("out", reg_addr);
            common_inst[i].base_addr = base_addr;
            base_addr += (1 << qc.max_drp_addr_width);
        }
        
        fill_ports ();
        //print ();
        
        mgt_inst[0].write_ifaces (qc.interfaces_fn);
        mgt_inst[0].write_module (qc.mgt_module_fn);
        mgt_inst[0].write_iface_assignment (mgt_inst[0].mgt_rx_lst, qc.mgt_rx_iface);
        mgt_inst[0].write_iface_assignment (mgt_inst[0].mgt_tx_lst, qc.mgt_tx_iface);
//        mgt_inst[0].write_iface_assignment (mgt_inst[0].phalg_tx_lst, qc.phalg_tx_iface);
        common_inst[0].write_module(qc.common_module_fn);
        write_quad_file (qc.quad_module_fn);
        mgt_inst[0].write_offset_table    (qc.mgt_port_offsets_fn);
        common_inst[0].write_offset_table (qc.common_port_offsets_fn);
    }
}

