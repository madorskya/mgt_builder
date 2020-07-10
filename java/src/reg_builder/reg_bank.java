package reg_builder;

import java.io.File;
import java.nio.charset.StandardCharsets;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.sql.Timestamp;
import java.util.*;

class bit_range
{
    int low, high;
    long mask;
    bit_range(int l, int h){low = l; high = h; make_mask();};
    bit_range(String s) // string format = "H:L"
    {
        String[] sf = s.split(":");
        high = Integer.parseInt(sf[0]);
        if (sf.length < 2) low = high; // single bit range
        else low  = Integer.parseInt(sf[1]);
        make_mask();
    };

    bit_range() {low = high = -1; mask = 0;};
    bit_range(bit_range br){low = br.low; high = br.high; make_mask();};
    void expand (bit_range r) {if (r.high > high) high = r.high; if (r.low < low) low = r.low;};
    String print() {return String.format("%d:%d", high, low);};
    int width(){return high-low+1;};
    void make_mask()
    {
        mask = 1;
        mask = (mask << width()) - 1; // mask is now in lower bits
        mask <<= low; // now in proper position
    };
    
};

class attribute
{
// Name	att_bits	att_enc	bin_enc	rw	type	default	interface	dimensions
	String name;
	bit_range att_range;
	bit_range reg_range;
	int reg_offset;
	boolean read_only;
	String type;
	String default_val;
	String iface;
	String indexes;
	List<bit_range> dims;
	boolean new_reg;
	// constructor for attribute array
	attribute (String iname, bit_range iatt_range, boolean ireadonly, String itype, 
			   String idefault, String iiface, String[] idims, boolean inew_reg)
	{
		name        = iname;
		att_range   = iatt_range;
		read_only   = ireadonly;
		type        = itype;
		default_val = idefault;
		iface       = iiface;
		indexes     = "";
		new_reg     = inew_reg;
		dims = new ArrayList<>();
		for (String d : idims)
		{
			d = d.trim();
			if (!d.isEmpty())
			{
				dims.add (new bit_range(d)); // construct new bit range
			}
		}
	};
	// constructor for unwrapped attribute
	attribute (String iname, bit_range iatt_range, boolean ireadonly, String itype, 
			   String idefault, String iiface, String iindexes)
	{
		name        = iname;
		att_range   = iatt_range;
		read_only   = ireadonly;
		type        = itype;
		default_val = idefault;
		iface       = iiface;
		indexes     = iindexes;
		new_reg     = false; // never create new register for unwrapped attribute
		dims = new ArrayList<>(); // keep empty
	};

	// constructor for unwrapped attribute
	attribute (attribute a)
	{
		name        = a.name;
		att_range   = a.att_range;
		read_only   = a.read_only;
		type        = a.type;
		default_val = a.default_val;
		iface       = a.iface;
		indexes     = a.indexes;
		dims        = a.dims;
		new_reg     = false; // never create new register for copied attribute
		
	};

	void print ()
	{
		System.out.print(String.format("name: %s%s rng: %s %s off: %x ro: %b type: %s df: %s iface: %s new_r: %b dims: ", 
				name, indexes, att_range.print(), reg_range.print(), reg_offset, read_only, type, default_val, iface, new_reg));
		if (dims.size() > 0) 
		{
			for (bit_range r: dims)
				System.out.print("[" + r.print() + "] ");
		}
		System.out.println();
	};
	
	// generate verilog global for this att
	String verilog_global()
	{
		String s = String.format("logic%s %s", 
				(att_range.width() > 1) ? (" [" + att_range.print() + "]") : "", 
				name);
		if (dims.size() > 0) 
		{
			for (bit_range r: dims)
				s += " [" + r.print() + "]";
		}
		s += ";";
		return s;
	};

	// generate modport in
	String verilog_modport_in()
	{
		String s = String.format("%s %s,", (read_only ? "output":"input"), name);
		return s;
	};

	// generate modport in
	String verilog_modport_out()
	{
		String s = String.format("%s %s,", (read_only ? "input":"output"), name);
		return s;
	};

	// generate verilog logic for this att
	String verilog_logic()
	{
		String s = "";
		if (read_only)
			s = String.format("assign reg_[%d][%s] = rbld.%s%s;", reg_offset, reg_range.print(), name, indexes);
		else
			s = String.format("assign rbld.%s%s = reg_[%d][%s];", name, indexes, reg_offset, reg_range.print());
		
		return s;
	};
	//Address       bits    rw      Name    att_bits        att_enc bin_enc
	//00000   0:0     R/W     CPLLRESET       0:0     x-x     x-x
	String register_map()
	{
		String s = String.format("%05x\t%s\t%s\t%s%s\t%s\tx-x\tx-x", 
				reg_offset, reg_range.print(), read_only ? "R":"R/W", name, indexes, att_range.print());
		return s;
	};
};

public class reg_bank 
{
	HashMap<Integer, attribute> atts; // map of the attributes (register fractions), with arrays
	HashMap<Integer, attribute> atts_unwrap; // map of the attributes (register fractions), arrays unwrapped
    int reg_width_bits = 64; // hardcoded temporarily
	
    int reg_base;
    int reg_last;
    int wire_base;
    int wire_last;
    
    String generated_header;

    List<String> kill_last_comma (List<String> s)
    {
        if (s.size() > 0)
        {
            String last_s = s.get(s.size()-1).replace(",", "");
            s.set(s.size()-1, last_s);
        }
        return s;
    };

    public reg_bank()
	{
		atts = new HashMap<Integer, attribute>();
		atts_unwrap = new HashMap<Integer, attribute>();
//		System.out.println ("reg_bank created");
        Date date = new Date ();
        long time = date.getTime();
        Timestamp ts = new Timestamp (time);

        generated_header = String.format
        (
        "// This code was automatically generated by FPGA Register Builder\n" + 
        "// Timestamp: %s\n" + 
        "// User: %s\n" + 
        "// FPGA MGT+Register builder homepage: github.com/madorskya/mgt_builder\n",  
        ts, System.getProperty("user.name")); 
		
	};
	
    public String[] read_tab_file (String fname) 
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
        }
        finally
        {
        	if (inFile1 != null) inFile1.close();
        }
        String[] dum = {""};
        return dum;
    };
    
	int atts_unwrap_count;
	boolean unwrap_first;

	public void unwrap_index(attribute a, int ind_ind, String comb_ind)
    {
//    	System.out.println ("unwrap: " + a.name + " " + ind_ind);
		
    	bit_range r = a.dims.get(ind_ind);
    	String ci;
    	for (int i = r.low; i <= r.high; i++)
    	{
    		ci = String.format("[%d]", i);
    		
    		if (ind_ind < a.dims.size()-1)
    		{
    			// not the last index, go deeper
    			unwrap_index (a, ind_ind+1, comb_ind + ci);
    		}
    		else
    		{
    			// this is the last index level
//    			System.out.println ("recursion: " + a.name + comb_ind + ci);
    			attribute ar = new attribute(a); // make copy of original attribute
    			
    			if (unwrap_first) ar.new_reg = a.new_reg; // only first unwrapped att can create new register
    			else ar.new_reg = false;
    			
    			ar.indexes = comb_ind + ci; // add indexes
                atts_unwrap.put(atts_unwrap_count, ar); // put into unwrapped list
                atts_unwrap_count++;
                unwrap_first = false;
    		}
    	}
    };

    public int read_config (String[] tc)
    {
    	String att_name;
    	int line_count = 0;
    	int atts_count = 0;
    	atts_unwrap_count = 0;
    	
        for (String s : tc) // line by line
        {

            String[] sf = s.split("\t", -1);
//            System.out.println("name: " + sf[0] + "lng: " + sf.length);
            if (sf.length < 10 || sf[0].contains("//")) continue; // ignore commented, malformed lines

            for (int bi = 0; bi < 10; bi++) sf[bi] = sf[bi].trim();

            // check if first field has contents, for new register
            att_name = sf[0];

            if (!att_name.isEmpty()) // this line contains attribute name
            {
                boolean read_only  = sf[4].contentEquals("R"); // read-only attribute
        		String type        = sf[5];
        		String default_val = sf[6];
        		String iface       = sf[7];
        		String[] dims      = sf[8].split(",");
        		boolean new_reg    = !sf[9].isEmpty();


                bit_range att_range = new bit_range(sf[1]); // register bit range

                attribute na = new attribute (att_name, att_range, read_only, type, default_val, iface, dims, new_reg);
                atts.put(atts_count, na);
                atts_count++;

//                System.out.println(String.format("name: %s dims.length: %d na.dims.size: %d new_reg: %b", 
//        				att_name, dims.length, na.dims.size(), new_reg));
                
                // if it's an array, unwrap 
                if (na.dims.size() > 0)
                {
                	unwrap_first = true;
                	unwrap_index(na, 0, "");
                }
                else
                {
                    atts_unwrap.put(atts_unwrap_count, na);
                    atts_unwrap_count++;
                	
                }
                
                line_count++;

            }

        }
        return line_count;
    };
    
	public void print()
	{
       for(HashMap.Entry<Integer,attribute> ate : atts_unwrap.entrySet())
       {
           attribute att = ate.getValue(); // attribute from map
           att.print();
       }
       
       System.out.println(String.format("reg_base: %x, reg_last: %x wire_base: %x wire_last: %x", 
    	       reg_base, reg_last, wire_base, wire_last));
	};
    
	void verilog_global(String fname)
	{
        List<String> lines = new ArrayList<>();
        
        lines.add (generated_header);
        lines.add ("`ifndef RBLD_INTERFACE_SV");
        lines.add ("`define RBLD_INTERFACE_SV\n");
        lines.add ("interface rbld_iface;\n");
        
		for(HashMap.Entry<Integer,attribute> ate : atts.entrySet())
			lines.add ("\t" + ate.getValue().verilog_global());
		
		lines.add ("\n\tmodport in\n\t(");
		
		for(HashMap.Entry<Integer,attribute> ate : atts.entrySet())
			lines.add ("\t\t" + ate.getValue().verilog_modport_in());
		
		lines = kill_last_comma (lines);

		lines.add ("\t);\n\n\tmodport out\n\t(");

		for(HashMap.Entry<Integer,attribute> ate : atts.entrySet())
			lines.add ("\t\t" + ate.getValue().verilog_modport_out());

		lines = kill_last_comma (lines);

		lines.add ("\t);\n");
		
		lines.add ("endinterface");
        lines.add ("`endif");

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
	
	void verilog_logic(String fname)
	{
        List<String> lines = new ArrayList<>();
        
        // declaration of registers and wires
//        lines.add (String.format("reg  [%d:0] reg_  [%d:%d];", reg_width_bits-1, reg_last,  reg_base));
//        lines.add (String.format("wire [%d:0] wire_ [%d:%d];", reg_width_bits-1, wire_last, wire_base));
        lines.add (generated_header);
        lines.add (String.format("localparam REG_LAST = %d;", reg_last));
        lines.add (String.format("logic  [%d:0] reg_  [%d:%d];", reg_width_bits-1, wire_last,  reg_base));
        lines.add ("// Add the following to bus interface module IO list: rbld_iface.out rbld");
        lines.add ("// Add the following to IO list of modules that need to use any of the attributes: rbld_iface.in rbld");
        lines.add ("// Add the following to the top-level module: rbld_iface rbld();");
        lines.add ("// Inside the modules, access attributes like this: rbld.attribute_name");
        lines.add ("");
        
        // assignments of registers to atts and vv
        int i = 0;
		for(HashMap.Entry<Integer,attribute> ate : atts_unwrap.entrySet())
		{
			lines.add (ate.getValue().verilog_logic());
			i++;
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
		System.out.println ("verilog_logic wrote " + i + " lines");
		
	};
	
	void register_map (String fname)
	{
        List<String> lines = new ArrayList<>();
		int i = 0;
		for(HashMap.Entry<Integer,attribute> ate : atts_unwrap.entrySet())
		{
			lines.add (ate.getValue().register_map());
			i++;
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
		System.out.println ("register_map wrote " + i + " lines");
	};
	
    int assign_addresses (boolean read_only, int start_addr)
    {
        int ca = start_addr; // current address
        int cb = 0; // current bit
        
        for (HashMap.Entry<Integer,attribute> ate : atts_unwrap.entrySet())
        {
        	attribute p = ate.getValue(); // attribute from map
        	
        	// if att requires new register, set bit count to max so the new reg will be made
        	if (p.new_reg) cb = reg_width_bits; 
        		
            if (p.read_only == read_only) // 
            {
                int low_bit = cb;
                int high_bit = cb + p.att_range.width() - 1;
                if (high_bit >= reg_width_bits) // out of bits in this word, move to next word
                {
                    ca += 1; // take next register
                    cb = 0; // start with bit 0
                    low_bit = cb;
                    high_bit = cb + p.att_range.width() - 1; // should be no ports wider than reg_width_bits
                }
                cb += p.att_range.width(); // update current bit
                p.reg_offset = ca;
                p.reg_range = new bit_range(low_bit, high_bit);
            }
        }

        if (!read_only)
        {
            reg_base = start_addr;
            reg_last = ca;
        }
        else
        {
            wire_base = start_addr;
            wire_last = ca;
        }
        ca += 1;
        return ca;
    };
}
