
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
import java.io.*;
import java.sql.Timestamp;

/**
 *
 * @author madorsky
 */

public class quad_config 
{
     int mgts_per_quad;
     int commons_per_quad;
     int max_drp_addr_width; // maximum DRP address width for all modules
     int int_reg_offset; // offset of internal registers. Depends on address bus in the top level design
     int max_mgt_x;
     int max_mgt_y;
     int max_common_x;
     int max_common_y;
     int reg_width_bits; // register width
    
    String mgt_ports_fn;
    String common_ports_fn;
    String quad_connections_fn;
    String fpga_mgts_fn;
    String refclk_buf_inst_fn;
    String reference_clocks_fn;
    String links_fn;
    String mgt_atts_fn;
    String common_atts_fn;
    String xymap_fn;
    String top_module_name;
    
    String[] mgt_ports;
    String[] common_ports;
    String[] quad_connections;
    String[] reference_clocks;
    String[] links;
    String[] fpga_mgts;
    String[] refclk_buf_inst;

    String mgt_refclk_name;
    String common_refclk_name;    
    String mgt_channel_name;
    String mgt_common_name;
    String mgt_quad_name;
    String mgt_txoutclk_name;
    String mgt_txusrclk_name;
    String mgt_rxoutclk_name;
    String mgt_rxusrclk_name;
    String mgt_xoutclk_min_period;
    
    
    String mgt_rx_iface;
    String mgt_tx_iface;
//  String phalg_tx_iface;
    String drp_iface;
    String interfaces_fname;
    String mgt_module_fn;
    String common_module_fn;
    String quad_module_fn;
    String mgt_port_offsets_fn;
    String common_port_offsets_fn;
    String mgt_drp_offsets_fn;
    String common_drp_offsets_fn;
    String mgt_path;
    String board_path, board_full_path;
    String root_path;
    String mgt_config_fn;
    String sv_path;
    String interfaces_fn;
    String generated_header;

    HashMap<String,protocol_config> protocol_map;
    
    public static String[] read_tab_file (String fname) 
    {
        // create token1
        String token1 = "";
        
        Scanner inFile1 = null;
        try 
        {
        	inFile1 = new Scanner(new File (fname));
        	inFile1.useDelimiter("\\n");
            List<String> lines = new ArrayList<String>();

            while (inFile1.hasNext()) 
            {
                // find next line
                token1 = inFile1.next();
                lines.add(token1);
            }
            inFile1.close();

            String[] linesArray = lines.toArray(new String[0]);

            return linesArray;
        }
        catch (Exception e)
        {
            System.out.println ("Cannot read input file: " + fname);
            if (inFile1 != null) inFile1.close();
            System.exit(1);
        }
        finally
        {
        	if (inFile1 != null) inFile1.close();
        }
        String[] dum = {""};
        return dum;
    };
    

    public void decode_top_config (String[] tc)
    {
       xymap_fn = "xymap.sv"; // to support legacy configs
       for (String s : tc) 
       {
           String[] sf = s.split("\t");
//           if (sf.length > 1)
//                System.out.println(sf[0] + " + " + sf[1]);

           if (sf.length > 1 && !sf[0].contains("//")) // ignore commented lines
           {
           // can't find an easy way of creating a list of parameter names and corresponding parameters by pointer
           // using brute force approach
               if (sf[0].trim().contentEquals("int_reg_offset"        ))  int_reg_offset        = Integer.parseInt(sf[1].trim(), 10); else
               if (sf[0].trim().contentEquals("reg_width_bits"        ))  reg_width_bits        = Integer.parseInt(sf[1].trim(), 10); else
               if (sf[0].trim().contentEquals("mgt_rx_iface"          ))  mgt_rx_iface          = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_tx_iface"          ))  mgt_tx_iface          = sf[1].trim(); else
//               if (sf[0].trim().contentEquals("phalg_tx_iface"        ))  phalg_tx_iface        = sf[1].trim(); else
               if (sf[0].trim().contentEquals("drp_iface"             ))  drp_iface             = sf[1].trim(); else
               if (sf[0].trim().contentEquals("interfaces_fname"      ))  interfaces_fname      = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_module_fn"         ))  mgt_module_fn         = sf[1].trim(); else
               if (sf[0].trim().contentEquals("common_module_fn"      ))  common_module_fn      = sf[1].trim(); else
               if (sf[0].trim().contentEquals("quad_module_fn"        ))  quad_module_fn        = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_port_offsets_fn"   ))  mgt_port_offsets_fn   = sf[1].trim(); else
               if (sf[0].trim().contentEquals("common_port_offsets_fn"))  common_port_offsets_fn= sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_drp_offsets_fn"    ))  mgt_drp_offsets_fn    = sf[1].trim(); else
               if (sf[0].trim().contentEquals("common_drp_offsets_fn" ))  common_drp_offsets_fn = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_atts_fn"           ))  mgt_atts_fn           = sf[1].trim(); else
               if (sf[0].trim().contentEquals("common_atts_fn"        ))  common_atts_fn        = sf[1].trim(); else
               if (sf[0].trim().contentEquals("xymap_fn"              ))  xymap_fn              = sf[1].trim(); else
               if (sf[0].trim().contentEquals("MGT"                   ))  mgt_path              = sf[1].trim(); else
               if (sf[0].trim().contentEquals("board"                 ))  board_path            = sf[1].trim(); else
               if (sf[0].trim().contentEquals("root_config"           ))  root_path             = sf[1].trim(); else
               if (sf[0].trim().contentEquals("top_module_name"       ))  top_module_name       = sf[1].trim(); else
            	   
               {
            	   
                   System.out.print ("Unsupported parameter in top config file: ");
                   if (sf.length > 0) System.out.print (sf[0] + " = ");
                   if (sf.length > 1) System.out.print (sf[1]);
                   System.out.println();
               }
           }
       }
       
       // create file names from MGT and board paths
       String mgt_full_path = root_path + "/chips/" + mgt_path + "/";

       mgt_config_fn           = mgt_full_path + "mgt_config.tab";
       mgt_ports_fn            = mgt_full_path + "mgt_ports.tab"; 
       common_ports_fn         = mgt_full_path + "common_ports.tab";
       quad_connections_fn     = mgt_full_path + "quad_connections.tab";
       fpga_mgts_fn	           = mgt_full_path + "mgt_placement.tab";
       refclk_buf_inst_fn      = mgt_full_path + "refclk_buf_inst.sv";
       mgt_port_offsets_fn     = mgt_full_path + "mgt_port_offsets.tab";
       common_port_offsets_fn  = mgt_full_path + "common_port_offsets.tab";

       board_full_path = root_path + "/boards/" + board_path + "/";

       reference_clocks_fn  = board_full_path + "reference_clocks.tab";
       links_fn             = board_full_path + "links.tab";
       
       // add path for generated sources
       sv_path = root_path + "/sv/";
       
       quad_module_fn   = sv_path + quad_module_fn;
       common_module_fn = sv_path + common_module_fn;
       mgt_module_fn    = sv_path + mgt_module_fn;
       interfaces_fn    = sv_path + interfaces_fname; // need to keep name without path for include statements
       

    };

    public void decode_mgt_config (String[] tc)
    {
       for (String s : tc) 
       {
           String[] sf = s.split("\t");
//           if (sf.length > 1)
//                System.out.println(sf[0] + " + " + sf[1]);

           if (sf.length > 1 && !sf[0].contains("//")) // ignore commented lines
           {
           // can't find an easy way of creating a list of parameter names and corresponding parameters by pointer
           // using brute force approach
               if (sf[0].trim().contentEquals("mgts_per_quad"         ))  mgts_per_quad          = Integer.parseInt(sf[1].trim(), 10); else
               if (sf[0].trim().contentEquals("commons_per_quad"      ))  commons_per_quad       = Integer.parseInt(sf[1].trim(), 10); else
               if (sf[0].trim().contentEquals("max_drp_addr_width"    ))  max_drp_addr_width     = Integer.parseInt(sf[1].trim(), 10); else
               if (sf[0].trim().contentEquals("max_mgt_x"             ))  max_mgt_x              = Integer.parseInt(sf[1].trim(), 10); else
               if (sf[0].trim().contentEquals("max_mgt_y"             ))  max_mgt_y              = Integer.parseInt(sf[1].trim(), 10); else
               if (sf[0].trim().contentEquals("max_common_x"          ))  max_common_x           = Integer.parseInt(sf[1].trim(), 10); else
               if (sf[0].trim().contentEquals("max_common_y"          ))  max_common_y           = Integer.parseInt(sf[1].trim(), 10); else
               if (sf[0].trim().contentEquals("mgt_refclk_name"       ))  mgt_refclk_name        = sf[1].trim(); else
               if (sf[0].trim().contentEquals("common_refclk_name"    ))  common_refclk_name     = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_channel_name"      ))  mgt_channel_name       = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_common_name"       ))  mgt_common_name        = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_quad_name"         ))  mgt_quad_name          = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_txoutclk_name"     ))  mgt_txoutclk_name      = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_txusrclk_name"     ))  mgt_txusrclk_name      = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_rxoutclk_name"     ))  mgt_rxoutclk_name      = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_rxusrclk_name"     ))  mgt_rxusrclk_name      = sf[1].trim(); else
               if (sf[0].trim().contentEquals("mgt_xoutclk_min_period"))  mgt_xoutclk_min_period = sf[1].trim(); else
               {
                   System.out.println ("Unsupported parameter in MGT config file: " + sf[0] + " = " + sf[1]);
               }
           }
       }
    };
    
    public protocol_config decode_protocol_config (String[] tc)
    {
       protocol_config pcfg = new protocol_config();
       for (String s : tc) 
       {
           String[] sf = s.split("\t");
//           if (sf.length > 1)
//                System.out.println(sf[0] + " + " + sf[1]);

           if (sf.length > 1 && !sf[0].contains("//")) // ignore commented lines
           {
           // can't find an easy way of creating a list of parameter names and corresponding parameters by pointer
           // using brute force approach
               if (sf[0].trim().contentEquals("rxoutclk_constr"))  pcfg.rxoutclk_constr = Double.parseDouble (sf[1].trim()); else
               if (sf[0].trim().contentEquals("txoutclk_constr"))  pcfg.txoutclk_constr = Double.parseDouble (sf[1].trim()); else
               {
                   System.out.println ("Unsupported parameter in protocol config file: " + sf[0] + " = " + sf[1]);
               }
           }
       }
//       System.out.println(String.format("returning protocol: rx: %f tx: %f", pcfg.rxoutclk_constr, pcfg.txoutclk_constr));
       return pcfg;
    };
    
    public quad_config (String top_config_name)
    {
       // read top config file
       String[] top_config = read_tab_file (top_config_name);
       decode_top_config (top_config);
       
       String[] mgt_config = read_tab_file (mgt_config_fn);
       decode_mgt_config (mgt_config);
       
       reference_clocks = read_tab_file (reference_clocks_fn);
       fpga_mgts        = read_tab_file (fpga_mgts_fn);
       links            = read_tab_file (links_fn); 
       mgt_ports        = read_tab_file (mgt_ports_fn);
       common_ports     = read_tab_file (common_ports_fn);
       quad_connections = read_tab_file (quad_connections_fn);
       refclk_buf_inst  = read_tab_file (refclk_buf_inst_fn);
       
       // read protocol configs
        protocol_map = new HashMap<>();
        String[] pathnames;

        File f = new File(root_path + "/protocols");
        pathnames = f.list(); // list all directories with protocols
        System.out.println("Protocols found:");
        for (String pathname : pathnames) 
        {
            System.out.println(pathname);
            String protocol_config_fn = root_path + "/protocols/" + pathname + "/protocol_config.tab";
            String[] protocol_config_s = read_tab_file (protocol_config_fn); // read protocol configuration
            protocol_config pcfg = decode_protocol_config(protocol_config_s);
            protocol_map.put(pathname, pcfg); // store protocol in map
//            System.out.println(String.format("stored protocol: %s rx: %f tx: %f", pathname, pcfg.rxoutclk_constr, pcfg.txoutclk_constr));
        }        
//        for (String pn: protocol_map.keySet())
//        {
//            protocol_config pc = protocol_map.get(pn);
//            System.out.println(String.format("protocol: %s rx: %f tx: %f", pn, pc.rxoutclk_constr, pc.txoutclk_constr));
//        }
        
        Date date = new Date ();
        long time = date.getTime();
        Timestamp ts = new Timestamp (time);

        generated_header = String.format
        (
        "// This code was automatically generated by FPGA MGT Builder\n" + 
        "// Timestamp: %s\n" + 
        "// User: %s\n" + 
        "// FPGA MGT builder homepage: https://github.com/madorskya/mgt_builder\n",  
        ts, System.getProperty("user.name")); 
      
        
    }
}
