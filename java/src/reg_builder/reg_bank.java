package reg_builder;

import java.io.File;
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
};

public class reg_bank 
{
	HashMap<Integer, attribute> atts; // map of the attributes (register fractions), with arrays
	HashMap<Integer, attribute> atts_unwrap; // map of the attributes (register fractions), arrays unwrapped
	
    int reg_base;
    int reg_last;
    int wire_base;
    int wire_last;

    public reg_bank()
	{
		atts = new HashMap<Integer, attribute>();
		atts_unwrap = new HashMap<Integer, attribute>();
		System.out.println ("reg_bank created");
		
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

                System.out.println(String.format("name: %s dims.length: %d na.dims.size: %d new_reg: %b", 
        				att_name, dims.length, na.dims.size(), new_reg));
                
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
    
    int assign_addresses (boolean read_only, int start_addr)
    {
        int ca = start_addr; // current address
        int cb = 0; // current bit
        int reg_width_bits = 64; // hardcoded temporarily
        
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
