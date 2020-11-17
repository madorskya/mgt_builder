
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

class refclk_prop
{
    String name;
    double mhz; // frequency in MHz
    String pin; // positive pin name
    boolean used;
}

class mgt_prop
{
    static final int max_refclk_per_mgt = 10; // max count of refclk sources for each MGT
    boolean tx_active;
    boolean rx_active;
    int x;
    int y;
    int x_common;
    int y_common;
    String tx_pll;
    String rx_pll;
    String tx_protocol;
    String rx_protocol;
    double tx_ref_f;
    double rx_ref_f;
    String tx_refclk; // selected tx refclk
    String rx_refclk; // selected rx refclk
    int tx_refclk_ind; // selected tx refclk index
    int rx_refclk_ind; // selected rx refclk index
    String [] refclk_src = new String[max_refclk_per_mgt];
    String tx_group_name;
    int tx_group_index;
    String rx_group_name;
    int rx_group_index;
    int base_addr;
    
    mgt_prop()
    {
        tx_active = false;
        rx_active = false;
        x = -1;
        y = -1;
        x_common = -1;
        y_common = -1;
        tx_pll = "";
        rx_pll = "";
        tx_protocol = "";
        rx_protocol = "";
        tx_ref_f = 0;
        rx_ref_f = 0;
        tx_refclk = "";
        rx_refclk = "";
        tx_refclk_ind = -1;
        rx_refclk_ind = -1;
        for (int i = 0; i < max_refclk_per_mgt; i++)
            refclk_src[i] = new String("");
        tx_group_name = "";
        tx_group_index = -1;
        rx_group_name = "";
        rx_group_index = -1;
//        int base_addr = -1;
    };
    
    @Override
    public boolean equals(Object o) 
    { 
        if (this == o) { 
            return true; 
        } 
        if (o == null) { 
            return false; 
        } 
        if (this.getClass() != o.getClass()) { 
            return false; 
        } 
        mgt_prop other = (mgt_prop)o; 
        if (this.x != other.x || this.y != other.y) 
        { 
            return false; 
        } 
        return true; 
    } 
    
    void print()
    {
        System.out.print("x: " + x);
        System.out.print(" y: " + y);
        System.out.print(" x_common: " + x_common);
        System.out.print(" y_common: " + y_common);
        System.out.print(" tx_protocol: " + tx_protocol);
        System.out.print(" rx_protocol: " + rx_protocol);
        System.out.print(" tx_pll: " + tx_pll);
        System.out.print(" rx_pll: " + rx_pll);
        System.out.print(" tx_refclk_ind: " + tx_refclk_ind);
        System.out.print(" rx_refclk_ind: " + rx_refclk_ind);
        System.out.print(" tx_ref_f: " + tx_ref_f);
        System.out.print(" rx_ref_f: " + rx_ref_f);
        System.out.print(" tx_group_name: " + tx_group_name);
        System.out.print(" tx_group_ind: " + tx_group_index);
        System.out.print(" rx_group_name: " + rx_group_name);
        System.out.print(" rx_group_ind: " + rx_group_index);
        System.out.print(" clocks:");
        for (int i = 0; i < refclk_src.length; i++)
        {
            System.out.print (" " + refclk_src[i]);
        }
        System.out.println();
    };
}

class common_prop
{
    int x;
    int y;
    boolean active;
    int inst_index; // instantiation index
    int refclk_ind; // ref clock index
    double refclk_f; // ref clock frequency
    int base_addr = 0;
    common_prop()
    {
        x = -1;
        y = -1;
        active = false;
        inst_index = -1;
        refclk_ind = -1;
        refclk_f = 0;
    };
};
    

public class fpga
{
	static quad_config qc;
    static mgt_prop mgti[][]; // array for MGT instances
    static common_prop commoni[][]; // array for COMMON instances
    static List<refclk_prop> refclks; // list of refclocks
    static quad quad_inst;
    static HashMap<String, HashMap<Integer,mgt_prop>> rx_group_map, tx_group_map;
    static HashMap<String, List<mgt_prop>> tx_mmcm_map; // tx_mmcm name --> list of MGTs connected to that MMCM

    static int aw_fpga_bits = 0;
    
    static List<String> kill_last_comma (List<String> s)
    {
        String last_s = s.get(s.size()-1).replace(",", "");
        s.set(s.size()-1, last_s);
        return s;
    };

    
    public static void init_mgts ()
    {
       // create all MGTs
       for (int x = 0; x < qc.max_mgt_x; x++)
       {
           for (int y = 0; y < qc.max_mgt_y; y++)
           {
               mgti[x][y] = new mgt_prop();
           }
       }

       // create all commons
       for (int x = 0; x < qc.max_common_x; x++)
       {
           for (int y = 0; y < qc.max_common_y; y++)
           {
               commoni[x][y] = new common_prop();
           }
       }
       refclks = new ArrayList<>();
       
       System.out.println("MGT and COMMON arrays initialized");
    };
    
    static void fill_clocks (String[] clocks)
    {
        
        
        for (String s : clocks)
        {
            s = s.trim();
            String[] sf = s.split("\t"); // split into fields
            if (!sf[0].contains("//")) // skip commented out line
            {
                refclk_prop p = new refclk_prop();
                p.name = sf[1];
                p.mhz = Double.parseDouble(sf[2]);
                p.pin = sf[3];
                p.used = true; // count all clocks as used for now
                refclks.add(p);
                
                //System.out.println("found refclk: " + p.name + " " + p.mhz + " MHz");
            }
        }
    };
    
    static void fill_mgts (String[] mgts_all)
    {
        mgt_prop m;
        common_prop c;
        
        for (String s : mgts_all)
        {
            s = s.trim();
            String[] sf = s.split("\t");
            if (!sf[0].contains("//")) // skip commented out line
            {
                int x = Integer.parseInt(sf[0]);
                int y = Integer.parseInt(sf[1]);
                m = mgti[x][y]; // get MGT for modification
                m.x = x;
                m.y = y;
                m.x_common = Integer.parseInt(sf[2]);
                m.y_common = Integer.parseInt(sf[3]);
                // fill clocks here
                for (int i = 0; i < m.refclk_src.length; i++)
                {
                    if (i+4 >= sf.length)
                    {
                        break; // assigned all ref clks
                    }
                    m.refclk_src[i] = sf[i+4];
                }
                mgti[x][y] = m; // store modified MGT
                
                c = commoni[m.x_common][m.y_common]; // get common for modification
                c.x = m.x_common;
                c.y = m.y_common;
                c.active = false;
                commoni[m.x_common][m.y_common] = c; // store common
            }
        }
        
    }
    
    static mgt_prop find_refclk(mgt_prop m)
    {
        // finds suitable refclk for an MGT
        for (int i = 0; i < refclks.size(); i++)
        {
            refclk_prop clk_av = refclks.get(i);
            for (String clk_src : m.refclk_src) // check all clk sources for this MGT
            {
                if (clk_src.contentEquals(clk_av.name) && clk_av.mhz == m.tx_ref_f)
                {
                    m.tx_refclk = clk_av.name;
                    m.tx_refclk_ind = i;
                }
                if (clk_src.contentEquals(clk_av.name) && clk_av.mhz == m.rx_ref_f)
                {
                    m.rx_refclk = clk_av.name;
                    m.rx_refclk_ind = i;
                }
            }
            refclks.set(i, clk_av);
        }
        
        return m;
    };
    
    static void fill_mgti (String[] links)
    {
        mgt_prop m;
        rx_group_map = new HashMap<String,HashMap<Integer,mgt_prop>>();
        tx_group_map = new HashMap<String,HashMap<Integer,mgt_prop>>();
        
        for (String s : links)
        {
            s = s.trim();
            String[] sf = s.split("\t");
            if (!sf[0].contains("//")) // skip commented out line
            {
                String dir = sf[0]; // direction 
                int x = Integer.parseInt(sf[1]);
                int y = Integer.parseInt(sf[2]);
                m = mgti[x][y]; // get MGT for modification
                if (dir.startsWith("rx"))
                {
                    if (m.rx_active)
                    {
                        System.out.println (String.format ("ERROR: RX %d %d already assigned", m.x, m.y));
                    }
                    else
                    {
                        m.rx_active = true;
                        m.rx_protocol    = sf[3];
                        m.rx_pll         = sf[4];
                        m.rx_ref_f       = Double.parseDouble(sf[5]);
                        m.rx_group_name  = sf[6];
                        m.rx_group_index = Integer.parseInt(sf[7]);
                        
                        if (!m.rx_group_name.isEmpty())
                        {
                            // try to obtain map for that group
                            if (!rx_group_map.containsKey(m.rx_group_name))
                            {
                                // no key for that group yet, make new map
                                HashMap<Integer, mgt_prop> nm = new HashMap<Integer, mgt_prop>();
                                rx_group_map.put(m.rx_group_name, nm);
                            }
                            HashMap<Integer, mgt_prop> gm = rx_group_map.get(m.rx_group_name);
                            // insert new MGT into the map
                            gm.put(m.rx_group_index, m);
                            // put the modified map back
                            rx_group_map.put(m.rx_group_name, gm);
                        }
                    }
                }
                else
                {
                    if (m.tx_active)
                    {
                        System.out.println (String.format ("ERROR: TX %d %d already assigned", m.x, m.y));
                    }
                    else
                    {
                        m.tx_active = true;
                        m.tx_protocol    = sf[3];
                        m.tx_pll         = sf[4];
                        m.tx_ref_f       = Double.parseDouble(sf[5]);
                        m.tx_group_name  = sf[6];
                        m.tx_group_index = Integer.parseInt(sf[7]);

                        if (!m.tx_group_name.isEmpty())
                        {
                            // try to obtain map for that group
                            if (!tx_group_map.containsKey(m.tx_group_name))
                            {
                                // no key for that group yet, make new map
                                HashMap<Integer, mgt_prop> nm = new HashMap<Integer, mgt_prop>();
                                tx_group_map.put(m.tx_group_name, nm);
                            }
                            HashMap<Integer, mgt_prop> gm = tx_group_map.get(m.tx_group_name);
                            // insert new MGT into the map
                            gm.put(m.tx_group_index, m);
                            // put the modified map back
                            tx_group_map.put(m.tx_group_name, gm);
                        }
                    }
                }
                m = find_refclk(m);
                mgti[x][y] = m; // store modified MGT
                
                // activate corresponding COMMON
                commoni[m.x_common][m.y_common].active = m.rx_active | m.tx_active;
            }
        }
    };
    
    static void print()
    {
        int j = 0;
        System.out.println("USED ref clocks:");
       for (refclk_prop p : refclks)
       {
           // print only used clocks here
           if (p.used)
           {
                System.out.println("refclk: " + p.name + " " + p.mhz + " MHz ind: " + j);
                j++;
           }
       }

       for (int x = 0; x < qc.max_mgt_x; x++)
       {
           for (int y = 0; y < qc.max_mgt_y; y++)
           {
               if (mgti[x][y].tx_active || mgti[x][y].rx_active) // MGT active
               {
                   mgti[x][y].print();
               }
           }
       }
       
       // print group maps
       for (HashMap.Entry<String,HashMap<Integer, mgt_prop>> grp : rx_group_map.entrySet())
       {
           System.out.println("group: " + grp.getKey());
           HashMap<Integer, mgt_prop> gm = grp.getValue();
           
           for (HashMap.Entry<Integer,mgt_prop> mgt: gm.entrySet())
           {
            System.out.println("index = " + mgt.getKey() + 
                              ", XY = " + mgt.getValue().x + " " + mgt.getValue().y); 
           }
       }

       for (HashMap.Entry<String,HashMap<Integer, mgt_prop>> grp : tx_group_map.entrySet())
       {
           System.out.println("group: " + grp.getKey());
           HashMap<Integer, mgt_prop> gm = grp.getValue();
           
           for (HashMap.Entry<Integer,mgt_prop> mgt: gm.entrySet())
           {
            System.out.println("index = " + mgt.getKey() + 
                              ", XY = " + mgt.getValue().x + " " + mgt.getValue().y); 
           }
       }
       // scan the map of MMCMs
       for(HashMap.Entry<String,List<mgt_prop>> mle : tx_mmcm_map.entrySet())
       {
           List<mgt_prop> ml = mle.getValue(); // list of MGTs for this MMCM
           System.out.println("MMCM name: " + mle.getKey());
           for (mgt_prop mgt : ml)
           {
               mgt.print();
           }
       }
    };
    
    static void write_fpga (String fname)
    {
        List<String> lines = new ArrayList<>();
        List<String> ios = new ArrayList<>();
        List<String> wires = new ArrayList<>();
        List<String> iface_inst = new ArrayList<>();
        List<String> iface_conn = new ArrayList<>();
        List<String> mmcms = new ArrayList<>();
        int quads_per_fpga = 0;
        String bitfld, arrfld0, arrfld1;
//        int base_addr = 0;
        // find all quads that need to be instantiated
        int j = 0;
        for (int x = 0; x < qc.max_common_x; x++)
        {
           for (int y = 0; y < qc.max_common_y; y++)
           {
               if (commoni[x][y].active) // quad active
               {
                   System.out.println ("active COMMON x, y, index: " + x + " " + y + " " + j);
                   commoni[x][y].inst_index = j; // order of quad instantiations
                   j++;
                   quads_per_fpga++;
               }
           }
        }

        for (port p: quad_inst.port_lst)
        {
            if (p.bits > 1)
            {
                bitfld = String.format("[%2d:0]", p.bits-1);
                if (p.dim0 > 1) 
                    arrfld1 = String.format("[%d:0]", p.dim0-1);
                else
                    arrfld1 = "";
            }
            else
            {
                if (p.dim0 > 1) 
                    bitfld = String.format("[%2d:0]", p.dim0-1);
                else
                    bitfld = "      ";
                arrfld1 = "";
            }

            //if (p.indexed) arrfld0 = String.format("[%d:0]", quads_per_fpga-1);
            if (p.indexed) arrfld0 = "[quadn-1:0]";
            else arrfld0 = "";
            
            if (!p.quad_int && p.iface.isEmpty()) 
                wires.add(String.format("\twire %s %s %s%s;",
                    bitfld, p.name, arrfld0, arrfld1));
            
        }
        
        
        List<String> quad_io = quad_inst.write_inst("gi", "gi");
        
        // count used ref clocks
        int refclk_cnt = 0;
        for (refclk_prop p : refclks) if (p.used) refclk_cnt++;
        
        wires.add(String.format("\twire [%d:0] refclk;", refclk_cnt-1));
        
        // create refclk connections
        List<String> refclk_conns = new ArrayList<>();
        String tx_as = "", rx_as = "";
        for (int x = 0; x < qc.max_mgt_x; x++)
        {
            for (int y = 0; y < qc.max_mgt_y; y++)
            {
                mgt_prop m = mgti[x][y];
                tx_as = "";
                rx_as = "";
                if (m.tx_refclk_ind >= 0 && m.tx_pll.contentEquals("C"))
                {
                    tx_as = String.format("\tassign %s%s [%d][%d] = refclk [%d]; // %s %d", 
                            quad_inst.mgt_inst[0].prefix, qc.mgt_refclk_name.toLowerCase(), 
                            commoni[m.x_common][m.y_common].inst_index, y % qc.mgts_per_quad, m.tx_refclk_ind,
                            m.tx_group_name, m.tx_group_index); // Y coord is used to calculate MGT number within quad
                }
                else if (m.rx_refclk_ind >= 0 && m.rx_pll.contentEquals("C"))
                {
                    rx_as = String.format("\tassign %s%s [%d][%d] = refclk [%d]; // %s %d", 
                            quad_inst.mgt_inst[0].prefix, qc.mgt_refclk_name.toLowerCase(), 
                            commoni[m.x_common][m.y_common].inst_index, y % qc.mgts_per_quad, m.rx_refclk_ind,
                            m.rx_group_name, m.rx_group_index); // Y coord is used to calculate MGT number within quad
                }
                
                if (m.tx_refclk_ind >= 0 && m.tx_pll.contentEquals("Q")) 
                {
                    if (commoni[m.x_common][m.y_common].refclk_ind >= 0 && m.tx_ref_f != commoni[m.x_common][m.y_common].refclk_f)
                    {
                        System.out.println(String.format("ERROR: attempting to assign COMMON refclk that's already assigned: %s %d target F: %f assigned F: %f dir: TX cx: %d cy: %d", 
                                m.tx_group_name, m.tx_group_index, m.tx_ref_f, commoni[m.x_common][m.y_common].refclk_f, m.x_common, m.y_common));
                    }
                    else
                    {
                        // assign clock to COMMON only if it was not yet assigned
                        tx_as = String.format("\tassign %s%s [%d] = refclk [%d]; // %s %d", 
                                quad_inst.common_inst[0].prefix, qc.common_refclk_name.toLowerCase(), 
                                commoni[m.x_common][m.y_common].inst_index, m.tx_refclk_ind,
                                m.tx_group_name, m.tx_group_index);
                        commoni[m.x_common][m.y_common].refclk_ind = m.tx_refclk_ind; // mark this COMMON as having clock
                        commoni[m.x_common][m.y_common].refclk_f = m.tx_ref_f; // remember frequency
                        
                    }
                }
                else if (m.rx_refclk_ind >= 0 && m.rx_pll.contentEquals("Q"))
                {

                    if (commoni[m.x_common][m.y_common].refclk_ind >= 0 && m.rx_ref_f != commoni[m.x_common][m.y_common].refclk_f)
                    {
                        System.out.println(String.format("ERROR: attempting to assign COMMON refclk that's already assigned: %s %d target F: %f assigned F: %f dir: RX cx: %d cy: %d",
                                m.rx_group_name, m.rx_group_index, m.rx_ref_f, commoni[m.x_common][m.y_common].refclk_f, m.x_common, m.y_common));
                    }
                    else
                    {
                        // assign clock to COMMON only if it was not yet assigned
                        rx_as = String.format("\tassign %s%s [%d] = refclk [%d]; // %s %d", 
                                quad_inst.common_inst[0].prefix, qc.common_refclk_name.toLowerCase(), 
                                commoni[m.x_common][m.y_common].inst_index, m.rx_refclk_ind,
                                m.rx_group_name, m.rx_group_index);
                        commoni[m.x_common][m.y_common].refclk_ind = m.rx_refclk_ind; // mark this COMMON as having clock
                        commoni[m.x_common][m.y_common].refclk_f = m.rx_ref_f; // remember frequency
                    }
                }

                if (!tx_as.isEmpty()) refclk_conns.add(tx_as);
                if (!rx_as.isEmpty()) refclk_conns.add(rx_as);
            }
        }

        ios.add (String.format("\tdrp.in %s_fif,", qc.drp_iface));
        ios.add ("\tinput drpclk,");
        ios.add (String.format("\tinput [%d:0] refclk_p,", refclk_cnt-1));
        ios.add (String.format("\tinput [%d:0] refclk_n,", refclk_cnt-1));

        // add ports for user interfaces
        for (HashMap.Entry<String,HashMap<Integer, mgt_prop>> grp : rx_group_map.entrySet())
        {
            String grp_name = grp.getKey();
            int max_index = 0;
            HashMap<Integer, mgt_prop> gm = grp.getValue();

            for (HashMap.Entry<Integer,mgt_prop> mgt: gm.entrySet())
            {
                int link_index = mgt.getKey();
                if (link_index > max_index) max_index = link_index;
//                mgt_prop link_prop = mgt.getValue();
            }
            ios.add (String.format("\t%s.in %s [%d:0],", qc.mgt_rx_iface, grp_name, max_index));
        }

        for (HashMap.Entry<String,HashMap<Integer, mgt_prop>> grp : tx_group_map.entrySet())
        {
            String grp_name = grp.getKey();
            int max_index = 0;
            HashMap<Integer, mgt_prop> gm = grp.getValue();

            for (HashMap.Entry<Integer,mgt_prop> mgt: gm.entrySet())
            {
                int link_index = mgt.getKey();
                if (link_index > max_index) max_index = link_index;
//                mgt_prop link_prop = mgt.getValue();
            }
            ios.add (String.format("\t%s.in %s [%d:0],", qc.mgt_tx_iface, grp_name, max_index));
        }

        
        
        iface_inst.add (String.format("\t%s %s_if [quadn-1:0][%d:0]();", qc.mgt_tx_iface, qc.mgt_tx_iface, qc.mgts_per_quad-1));
        iface_inst.add (String.format("\t%s %s_if [quadn-1:0][%d:0]();", qc.mgt_rx_iface, qc.mgt_rx_iface, qc.mgts_per_quad-1));
//        iface_inst.add (String.format("\t%s %s_if [quadn-1:0][%d:0]();", qc.phalg_tx_iface, qc.phalg_tx_iface, qc.mgts_per_quad-1));
        
        // scan all links, instantiate connections
        int ai = 0;
        for (int x = 0; x < qc.max_mgt_x; x++)
        {
            for (int y = 0; y < qc.max_mgt_y; y++)
            {
                
                mgt_prop m = mgti[x][y];
                if (m.rx_active || m.tx_active)
                {
                    int qi = commoni[m.x_common][m.y_common].inst_index; // quad instantiation index
                    int mi = y % qc.mgts_per_quad; // mgt number within quad
                    if (m.rx_active)
                    {
                        iface_conn.add(String.format("\tassign_%s ari%d (%s_if [%d][%d], %s [%d]);", 
                            qc.mgt_rx_iface, ai, qc.mgt_rx_iface, qi, mi, m.rx_group_name, m.rx_group_index));
                    }

                    if (m.tx_active)
                    {
                        iface_conn.add(String.format("\tassign_%s ati%d (%s_if [%d][%d], %s [%d]);", 
                            qc.mgt_tx_iface, ai, qc.mgt_tx_iface, qi, mi, m.tx_group_name, m.tx_group_index));
//                        iface_conn.add(String.format("\tassign_%s ati%d (%s [%d], %s_if [%d][%d]);", 
//                            mgt_tx_iface, ai, mgti[x][y].tx_group_name, mgti[x][y].tx_group_index, mgt_tx_iface, qi, mi));
                    }
                    ai++;
                }
                
            }
        }
        
       // scan the map of MMCMs
       for(HashMap.Entry<String,List<mgt_prop>> mle : tx_mmcm_map.entrySet())
       {
           int i = 0; 
           String mmcm_name = mle.getKey(); // name of MMCM for this group
           List<mgt_prop> ml = mle.getValue(); // list of MGTs for this MMCM
           for (mgt_prop mgt : ml)
           {

                int qi = commoni[mgt.x_common][mgt.y_common].inst_index; // quad instantiation index
                int mi = mgt.y % qc.mgts_per_quad; // mgt number within quad
               
                if (i == 0) // generate MMCM for first MGT in each group
                {
                    ios.add (String.format("\toutput %s_mmcm_clk,", mmcm_name)); // declare generated clock as output
                    // create MMCM
                    mmcms.add (String.format("\t%s_mmcm %s_inst (.O(%s_mmcm_clk), .I(%s_if[%d][%d].%s));", 
                            mgt.tx_protocol, mmcm_name, mmcm_name,
                            qc.mgt_tx_iface, qi, mi, qc.mgt_txoutclk_name.toLowerCase()));
                    // mgt_tx_if[0][3].txoutclk
                }
                // connect txusrclk to the MMCM for all MGTs in group
                mmcms.add (String.format("\tassign %s%s[%d][%d] = %s_mmcm_clk; // %s%d",
                        quad_inst.mgt_inst[0].prefix, qc.mgt_txusrclk_name.toLowerCase(), qi, mi,
                        mmcm_name, mgt.tx_group_name, mgt.tx_group_index));
               
               i++;
           }
       }
        
        
        ios = kill_last_comma (ios);

        // how many bits is needed for addressing quads and commons
        aw_fpga_bits = (int)Math.ceil(Math.log(quads_per_fpga)/Math.log(2)) + quad_inst.get_aw_quad_bits();
        
        String drp_mux = String.format(
"	drp #(.AW(%d)) %s_qif [quadn-1:0]();\n" +
"	drp_mux_fpga #(.N(quadn), .AW_QUAD(%d)) drpm (%s_qif, %s_fif, drpclk);\n",
                quad_inst.get_aw_quad_bits(), qc.drp_iface, 
                quad_inst.get_aw_quad_bits(), qc.drp_iface, qc.drp_iface);
        
        lines.add    (qc.generated_header);
        lines.add    ("`include \"drp_interface.sv\"");
        lines.add    ("`include \"" + qc.interfaces_fname + "\"");
        lines.add    ("module " + qc.top_module_name + "\n("); // module header
        lines.addAll (ios);
        lines.add    (");");
        lines.add    ("\tlocalparam quadn = " + Integer.toString(quads_per_fpga) + ";");
        lines.add    (drp_mux);
        lines.addAll (iface_inst);
        lines.addAll (wires);
        lines.add    ("\tgenvar gi;");
        lines.add    ("\tgenerate");
        lines.add    ("\t\tfor (gi = 0; gi < quadn; gi++)");
        lines.add    ("\t\tbegin: quad_loop");
        lines.addAll (quad_io);
        lines.add    ("\t\tend");
        lines.add    ("\t\tfor (gi = 0; gi < " + refclk_cnt + "; gi++)");
        lines.add    ("\t\tbegin: refclk_loop");
        for (String rs : qc.refclk_buf_inst) lines.add (rs);
        lines.add    ("\t\tend");
        lines.add    ("\tendgenerate");
        lines.addAll (iface_conn);
        lines.addAll (refclk_conns);
        lines.addAll (mmcms);
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
    
    static void write_constr (String fname, String xymap_fn)
    {
        List<String> lines = new ArrayList<>();
        List<String> mgt_loc = new ArrayList<>();
        List<String> refclk_loc = new ArrayList<>();
        List<String> refclk_constr = new ArrayList<>();
        List<String> refclk_group = new ArrayList<>();
        List<String> genclk_constr = new ArrayList<>();
        List<String> genclk_group = new ArrayList<>();
        List<String> xylines = new ArrayList<>();
        List<String> xymap = new ArrayList<>();
    

        
        // MGT locations
        int max_qi = 0, max_mi = 0;
        for (int x = 0; x < qc.max_mgt_x; x++)
        {
            for (int y = 0; y < qc.max_mgt_y; y++)
            {
                
                mgt_prop m = mgti[x][y];
                if (m.rx_active || m.tx_active)
                {
                    int qi = commoni[m.x_common][m.y_common].inst_index; // quad instantiation index
                    int mi = y % qc.mgts_per_quad; // mgt number within quad
                    mgt_loc.add (String.format("set_property LOC %s_X%dY%-2d [get_cells */quad_loop[%2d].%s_inst/mgt_loop[%2d].%s_tux_inst/%s_inst];", 
                            qc.mgt_channel_name, x, y, qi, qc.mgt_quad_name, mi, qc.mgt_channel_name, qc.mgt_channel_name));
                
                    xymap.add(String.format("\tassign mgtxy[%d][%d] = 16'ha%d%02d;", qi, mi, x, y));
                    if (max_qi < qi) max_qi = qi;
                    if (max_mi < mi) max_mi = mi;
// example of generated clocks from IBERT
//create_clock -name Q7_RXCLK0 -period 3.2 [get_pins {u_ibert_core/inst/QUAD[0].u_q/CH[0].u_ch/u_gthe2_channel/RXOUTCLK}]
//set_clock_groups -group [get_clocks Q7_RXCLK0] -asynchronous
//create_clock -name  Q7_TX0 -period 3.2 [get_pins {u_ibert_core/inst/QUAD[0].u_q/CH[0].u_ch/u_gthe2_channel/TXOUTCLK}]
//set_clock_groups -group [get_clocks Q7_TX0] -asynchronous

// working line from my test project. Full path to TXOUTCLK pin does not work for some reason, need to use this filtering syntax
//create_clock -period 8.0 -name txclk0  [get_pins -filter {REF_PIN_NAME=~*TXOUTCLK} -of_objects [get_cells -hierarchical -filter {NAME =~ *quad_loop[0]*mgt_loop[3]*GTHE2_CHANNEL_inst*}]]
                
                    if (m.rx_active)
                    {
                        // pick up clock constraint from protocol config
                        protocol_config pcfg = qc.protocol_map.get(m.rx_protocol);
                        
                        genclk_constr.add (String.format("create_clock -period %.3f -name rxclk%d_%d [get_pins -filter {REF_PIN_NAME=~*%s} -of_objects [get_cells -hierarchical -filter {NAME =~ *quad_loop[%d]*mgt_loop[%d]*%s_inst*}]]; # %s%d",
                                pcfg.rxoutclk_constr, x, y, qc.mgt_rxoutclk_name, qi, mi, qc.mgt_channel_name, m.rx_group_name, m.rx_group_index));
                        genclk_group.add (String.format("set_clock_groups -group [get_clocks -include_generated_clocks rxclk%d_%d] -asynchronous",
                                x, y));
                    }
                    if (m.tx_active)
                    {
                        // pick up clock constraint from protocol config
                        protocol_config pcfg = qc.protocol_map.get(m.tx_protocol);

                        genclk_constr.add (String.format("create_clock -period %.3f -name txclk%d_%d [get_pins -filter {REF_PIN_NAME=~*%s} -of_objects [get_cells -hierarchical -filter {NAME =~ *quad_loop[%d]*mgt_loop[%d]*%s_inst*}]]; # %s%d",
                                pcfg.txoutclk_constr, x, y, qc.mgt_txoutclk_name, qi, mi, qc.mgt_channel_name, m.tx_group_name, m.tx_group_index));
                        genclk_group.add (String.format("set_clock_groups -group [get_clocks -include_generated_clocks txclk%d_%d] -asynchronous",
                                x, y));
                    }
                }
            }
        }
/*
set_clock_groups -name async_refclks -asynchronous \
-group [get_clocks -include_generated_clocks clk0] \
-group [get_clocks -include_generated_clocks clk1]
  */      
// example below from IBERT
// set_clock_groups -group [get_clocks -include_generated_clocks REFCLK0_0] -asynchronous

//        refclk_group.add ("set_clock_groups -name async_refclks -asynchronous \\");
        int i = 0;
        for (refclk_prop p: refclks)
        {
 //create_clock -period 1.563 -name sync_ref_clk_p7 [get_ports {sync_ref_clk_p[7]}]
            refclk_constr.add (String.format ("create_clock -period %.3f -name refclk_p%-2d [get_ports refclk_p[%2d]];", 1000./p.mhz, i, i));
            refclk_loc.add (String.format("set_property LOC %4s [get_ports refclk_p[%2d]]; # name: %s %s MHz", p.pin, i, p.name, p.mhz));
//            refclk_group.add (String.format("  -group [get_clocks -include_generated_clocks refclk_p%-2d] \\", i));
            refclk_group.add (String.format("set_clock_groups -group [get_clocks -include_generated_clocks refclk_p%-2d] -asynchronous", i));
            i++;
        }

//        refclk_group.add (";"); // to compensate for the last backslash
  


        lines.add   (qc.generated_header.replace("//", "#"));
        lines.addAll(refclk_loc);
        lines.addAll(refclk_constr);
        lines.addAll(refclk_group);
        lines.addAll(mgt_loc);
        lines.addAll(genclk_constr);
        lines.addAll(genclk_group);
        
        Path file = Paths.get(fname);
        try
        {
            Files.write(file, lines, StandardCharsets.UTF_8);
        }
        catch (Exception e)
        {
            System.out.println(e);
        }

        
        // construct XY map header file
        String xymap_header = String.format("\twire [15:0] mgtxy [0:%d][0:%d]; // [quad][mgt]", max_qi, max_mi);
        
        xylines.add    (qc.generated_header);
        xylines.add    (xymap_header);
        xylines.addAll (xymap);
        
        Path xyfile = Paths.get(xymap_fn);
        try
        {
            Files.write(xyfile, xylines, StandardCharsets.UTF_8);
        }
        catch (Exception e)
        {
            System.out.println(e);
        }
    };
    
    static void assign_base_addr()
    {
        int quad_base   = 0;
        int module_base = 0;
        int module_inc = (1 << qc.max_drp_addr_width);
        int quad_inc   = (1 << quad_inst.get_aw_quad_bits());
        
        for (int x = 0; x < qc.max_mgt_x; x++)
        {
            for (int y = 0; y < qc.max_mgt_y; y++)
            {
                
                mgt_prop m = mgti[x][y];
                if (m.x >= 0) // only use filled MGTs
                {
                    if (commoni[m.x_common][m.y_common].active) // need to assign base addresses to all MGTs if this quad is active
                    {
                        mgti[x][y].base_addr = module_base;
                        module_base += module_inc;

                        if (y % qc.mgts_per_quad == qc.mgts_per_quad-1) // last MGT in this quad
                        {
                            commoni[m.x_common][m.y_common].base_addr = module_base;
                            quad_base += quad_inc;
                            module_base = quad_base; // move address to next quad base
                        }
                    }
                }
            }
        }
    };
    
    static void write_base_addr (String fname)
    {
        List<String> lines = new ArrayList<>();
        
        for (HashMap.Entry<String,HashMap<Integer, mgt_prop>> grp : rx_group_map.entrySet())
        {
            String grp_name = grp.getKey();
            HashMap<Integer, mgt_prop> gm = grp.getValue();

            for (HashMap.Entry<Integer,mgt_prop> mgt: gm.entrySet())
            {
                 mgt_prop m = mgt.getValue();
                 lines.add (String.format ("%s\t%d\t%d\t%d\t%05x\t%05x\t%s\t%s", 
                         grp_name, mgt.getKey(), m.x, m.y, m.base_addr, 
                         commoni[m.x_common][m.y_common].base_addr, 
                         qc.mgt_path, m.rx_protocol));
            }
        }

        for (HashMap.Entry<String,HashMap<Integer, mgt_prop>> grp : tx_group_map.entrySet())
        {
            String grp_name = grp.getKey();
            HashMap<Integer, mgt_prop> gm = grp.getValue();

            for (HashMap.Entry<Integer,mgt_prop> mgt: gm.entrySet())
            {
                 mgt_prop m = mgt.getValue();
                 lines.add (String.format ("%s\t%d\t%d\t%d\t%05x\t%05x\t%s\t%s", 
                         grp_name, mgt.getKey(), m.x, m.y, m.base_addr, 
                         commoni[m.x_common][m.y_common].base_addr, 
                         qc.mgt_path, m.tx_protocol));
            }
        }
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
    
    public void fill_tx_usrclk_mmcms()
    {
       tx_mmcm_map = new HashMap<String,List<mgt_prop>>();
       
       for (int x = 0; x < qc.max_mgt_x; x++)
       {
           for (int y = 0; y < qc.max_mgt_y; y++)
           {
               if (mgti[x][y].tx_active) // TX active
               {
                   int tx_refclk_ind = mgti[x][y].tx_refclk_ind;
                   String tx_protocol = mgti[x][y].tx_protocol;
                   // MMCM group name
                   String mmcm_group = String.format("%s_%d", tx_protocol, tx_refclk_ind);
                   // find the list of MGTs for this MMCM
                   if (!tx_mmcm_map.containsKey(mmcm_group))
                   {
                       
                        // no key for that group yet, make new map
                        List<mgt_prop> ml = new ArrayList<mgt_prop>();
                        tx_mmcm_map.put(mmcm_group, ml);
                   }
                    List<mgt_prop> ml = tx_mmcm_map.get(mmcm_group);
                    // add new MGT into the list
                    ml.add(mgti[x][y]);
                    // put the modified list back
                    tx_mmcm_map.put(mmcm_group, ml);
               
               }
           }
       }
       
    };
    
    
    void write_mmcm_map (String fname)
    {
        List<String> lines = new ArrayList<>();
        
       for(HashMap.Entry<String,List<mgt_prop>> mle : tx_mmcm_map.entrySet())
       {
           List<mgt_prop> ml = mle.getValue(); // list of MGTs for this MMCM
           lines.add("//MMCM name: " + mle.getKey());
           String master = "master";
           for (mgt_prop mgt : ml)
           {
               lines.add(String.format("%d\t%d\t%s", mgt.x, mgt.y, master));
               master = "slave";
           }
       }
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
    
    public fpga (quad_config qcp) 
    {
    	qc = qcp;

        mgti = new mgt_prop[qc.max_mgt_x][qc.max_mgt_y]; 
        commoni = new common_prop[qc.max_common_x][qc.max_common_y];
        quad_inst = new quad(qc);
       
       init_mgts(); 
       fill_clocks (qc.reference_clocks); // available reference clocks
       fill_mgts (qc.fpga_mgts); // available MGTs
       fill_mgti (qc.links); // MGT instances
       fill_tx_usrclk_mmcms (); // identify necessary MMCMs for txusrclk generation for each TX protocol
       
       print();

       quad_inst = new quad(qc);
       quad_inst.write_quad();

       write_fpga (qc.sv_path + "/" + qc.top_module_name + ".sv");
       write_constr (qc.sv_path + "/" + qc.top_module_name + ".xdc", qc.sv_path + qc.xymap_fn);
       
       assign_base_addr(); // assign base addresses to all MGTs in active quads
       write_base_addr(qc.root_path + "/fpga_base_addr.tab");
       write_mmcm_map (qc.board_full_path + "/tx_mmcm_map.tab");
    }
    
}
