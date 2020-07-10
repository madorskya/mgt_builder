package reg_builder;

public class top {
    public static void main (String[] args) 
    {
        // args[0] contains the top config file for the design
        
       System.out.println(
    		"    FPGA Register Builder 2020 Alex Madorsky, University of Florida/Physics\n" + 
       		"    This program comes with ABSOLUTELY NO WARRANTY.\n" + 
       		"    This is free software, and you are welcome to redistribute it\n" + 
       		"    under certain conditions; visit <https://www.gnu.org/licenses/gpl-3.0.txt> for details.\n"  
       		);

       	reg_bank rb = new reg_bank();
       	String[] fs = rb.read_tab_file("registers/register_list.tab");
       	rb.read_config(fs);
       	int last_addr = rb.assign_addresses (false, 0); // first R/W registers
       	rb.assign_addresses (true, last_addr); // then RO
       	rb.verilog_global("sv/registers_iface.sv"); // write verilog interface
       	rb.verilog_logic ("sv/registers_logic.sv"); // write verilog logic source
       	rb.register_map  ("registers/register_offsets.tab"); // write register address map for control software
//       	rb.print();
       
    }
}
