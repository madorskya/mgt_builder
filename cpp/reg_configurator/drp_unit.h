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


#ifndef DRP_UNIT_H
#define DRP_UNIT_H

#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/multiprecision/cpp_int.hpp>

using namespace std;

#ifdef MTF7 // MTF7 hardware access via PCIE
    #define mwrite(a,b,c,d) if (pwrite (a,b,c,d) != c) printf ("pwrite error: w: %d file: %s line: %d\n", c, __FILE__, __LINE__);
    #define mread(a,b,c,d) if (pread (a,b,c,d) != c) printf ("pread error: w: %d file: %s line: %d\n", c, __FILE__, __LINE__);
    #define mopen(d) ::open(d, O_RDWR)
#endif

#ifdef AXI // AXI hardware, memory mapping access
    #include <sys/mman.h>
    // two parameters below should come from config file
    #define DRP_SIZE 0x10000 // 13 bits of DRP address x 32 bit words = 15 bits of address
    #define DRP_BASE 0x54000000 // DRP area base address
    // AXI versions of mwrite and mread support only 8-byte transfers
    #define mwrite(a,b,c,d) {*((uint64_t*)(sys_vptr + d)) = *b;}
    #define mread(a,b,c,d)  {*b = *((uint64_t*)(sys_vptr + d));}
// debug versions of the macros
//	#define mwrite(a,b,c,d) {printf ("mwrite: a: %08x d: %016llx\n", d, *b); fflush(stdout); *((uint64_t*)(sys_vptr + d)) = *b;}
//	#define mread(a,b,c,d)  {printf ("mread : a: %08x\n", d); fflush(stdout); *b = *((uint64_t*)(sys_vptr + d));}
    #define mopen(d) open_dev_mem()

#endif


#define PORT_MARK   0x40000000 // port marker for register offsets
#define PORT_UNMARK 0x3FFFFFFF // mask for removing PORT_MARK from offsets

class bit_range
{
public:
    int low, high;
    boost::multiprecision::uint128_t mask;
    bit_range(int l, int h){low = l; high = h; make_mask();}
    bit_range(string s);
    bit_range(){low = high = -1; mask = 0;}
    bit_range(const bit_range &br){low = br.low; high = br.high; make_mask();}
    void expand (bit_range r) {if (r.high > high) high = r.high; if (r.low < low) low = r.low;}
    string print();
    int width(){return high-low+1;}
    void make_mask();
};
// a single attribute can be scattered over multiple registers
// this class represents one part of such attribute
class part_att
{
public:
    int offset;
    bit_range reg_rng; // bits in register representing this part
    bit_range att_rng; // bits in attribute corresponding
    part_att (int off, bit_range reg, bit_range att){offset = off; reg_rng = reg; att_rng = att;}
    part_att (){offset = -1;}
};

class attribute
{
public:
    string name;
    bit_range brange; // full range of this attribute
    map <string,int> att_enc; // contains all conversions from text attribute values to binary
    vector <part_att> p_reg; // register parts that together form this attribute
    boost::multiprecision::uint128_t value; // value of the attribute
    bool valid_value;
    bool drp_reg; // this is drp register, not a port
    bool read_only;
    int xfer_bytes; // how wide the transfer should be
    string unit; // which unit inside MGT does this attribute/port belongs to (RX, TX, CPLL)
    attribute(string nm, const bit_range& ar){name = nm; brange = ar; valid_value = false;}
    // add new attribute part to this attribute, expand range
    void add_part (part_att pa) {p_reg.push_back(pa); brange.expand(pa.att_rng);}
};

class register_prop
{
public:
    uint32_t value;
    bool drp_reg;
    bool read_only;
    register_prop()
    {
        value = 0;
        drp_reg = false;
        read_only = false;
    };
};

class drp_unit
{
public:
    bool common;
    bool common_is_used;
    int base_addr;
    string rx_group_name;
    int    rx_group_index;
    string tx_group_name;
    int    tx_group_index;

    bool rx_active, tx_active;
    int x;
    int y;
    string tx_protocol_path;
    string tx_pll;
    double tx_refclk_f;

    string rx_protocol_path;
    string rx_pll;
    double rx_refclk_f;
    int quad_address;

    int quad_drp_addr1 ;
    int quad_drp_addr0 ;
    int quad_port_addr1;
    int quad_port_addr0;

    map <string, attribute> atts; // contains all attributes in the unit
    drp_unit *common_unit;
    map <int, register_prop> registers; // map of all registers in the unit, including DRP and ports
    uint32_t MEM_BASE; // core fpga base addr, in 64-bit words
    vector<drp_unit> tx_mmcm_slaves; // list of txoutclk clock-sharing slaves

    drp_unit    (int base_a);
    drp_unit (){}
    int  read_config (std::string fname, bool drp_reg);
    int  read_config_emtf (std::string fname, bool drp_reg);
    void read_params ();
    int  read_params_rx_tx (string param_fname, string unit);
    int  read_units  (string fname);
    void fill_registers ();
    void write_registers (int fd);
    void read_registers  (int fd);
    void check_registers (int fd);
    void read_print_atts (int fd);
    void check_atts ();
    void reset_tx (int fd);
    void reset_rx (int fd);
    void tx_phase_align (int fd);
    void reset_cpll (int fd);
    void reset_qpll (int fd);
    void read_registers_prn (int fd);
    void att_write (int fd, string name, boost::multiprecision::uint128_t value);
    void wait_for (int fd, string name, int t);
    boost::multiprecision::uint128_t att_read  (int fd, string name, string &svalue);
    boost::multiprecision::uint128_t att_read_prn  (int fd, string name);
    boost::multiprecision::uint128_t verilog_value (string s);

    uint64_t reg_read  (int fd, int addr);
    void     reg_write (int fd, int addr, uint64_t data);

    string print();

};


#endif // DRP_UNIT_H
