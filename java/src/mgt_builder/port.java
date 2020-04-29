/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package mgt_builder;
//import static com.mycompany.fpga_quad_instantiator.quad_config.*;

//import static com.mycompany.fpga_quad_instantiator.fpga.qc;



/**
 *
 * @author madorsky
 */

class reg_addr
{
    int offset;
    int reg;
    int mask;
    int low_bit;  // low bit in the combined register
    int high_bit; // high bit in the combined register
}



public class port
{
    quad_config qc;
    String name;
    String dir;
    String type;
    String clock;
    int    bits;
    int    dim0; // array dimension 0
    int    dim1; // array dimension 1
    String dflt;
    String iface;
    boolean indexed;
    boolean quad_int; // internal to quad 
    reg_addr address;
    String buffer;
    
    port(quad_config qcp)
    {
        qc = qcp;
        name = "";
        dir = "";
        type = "";
        clock = "";
        bits = 0;
        dim0 = 0;
        dim1 = 0;
        dflt = "";
        iface = "";
        indexed = false;
        quad_int = false;
        buffer = "";
        address = new reg_addr();
        address.offset = -1;
        address.reg = -1;
        address.mask = 0;
        address.low_bit = -1;
        address.high_bit = -1;
    };

    port(port p)
    {
        qc = p.qc;
        name = p.name;
        dir = p.dir;
        type = p.type;
        clock = p.clock;
        bits = p.bits;
        dim0 = p.dim0;
        dim1 = p.dim1;
        dflt = p.dflt;
        iface = p.iface;
        indexed = p.indexed;
        quad_int = p.quad_int;
        address = new reg_addr();
        address.offset = p.address.offset;
        address.reg = p.address.reg;
        address.mask = p.address.mask;
        address.low_bit = p.address.low_bit;
        address.high_bit = p.address.high_bit;
    };

    
    void print ()
    {
        System.out.printf("name: %26s ", name);
        System.out.printf("dir: %3s ", dir);
        System.out.printf("type: %1s ", type);
        System.out.printf("clock: %14s ", clock);
        System.out.printf("bits: %2d ", bits);
        System.out.printf("dflt: %9s ", dflt);
        System.out.printf("iface: %12s ", iface);
        System.out.printf("a.off: %08x ", address.offset);
        System.out.printf("a.reg: %08x ", address.reg);
        System.out.printf("a.mask: %08x ", address.mask);
        System.out.printf("a.hb: %2d ", address.high_bit);
        System.out.printf("a.lb: %2d ", address.low_bit);
        System.out.println();
    };
    
    String offset_tab_entry ()
    {
//Address       bits    rw      Name    att_bits        att_enc bin_enc
        String rw = "R/W";
        if (dir.contentEquals("out")) rw = "R";
        
        return String.format ("%05x\t%d:%d\t%s\t%s\t%d:%d\tx-x\tx-x", 
                address.offset + qc.int_reg_offset, // adding internal register offset to all addresses
                address.high_bit, address.low_bit, rw, name, 
                bits-1, 0);
    };
}

