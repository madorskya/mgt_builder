//    FPGA MGT builder quickly builds complex configurations with multiple MGTs
//    Original work 2020 Alex Madorsky, University of Florida/Physics
//    Modified work 2021 Aleksei Greshilov, University of Florida/Physics
//    - eyescan option
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
#include <cstdlib>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <boost/algorithm/string.hpp>
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/system/error_code.hpp>

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/map.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/version.hpp>
#include <boost/multiprecision/cpp_int/serialize.hpp>

using namespace std;

#ifdef MTF7 // MTF7 hardware access via PCIE
#define mwrite(a,b,c,d) if (pwrite (a,b,c,d) != c) printf ("pwrite error: w: %d file: %s line: %d\n", c, __FILE__, __LINE__);
// read twice because of DRP interface peculiarity in MTF7
#define mread(a,b,c,d) if (pread (a,b,c,d) != c) printf ("pread error: w: %d file: %s line: %d\n", c, __FILE__, __LINE__); pread (a,b,c,d);

#define mopen(d) ::open(d, O_RDWR)

#endif
#ifdef AXI // AXI hardware, memory mapping access

	#include <sys/mman.h>
	// two parameters below should come from config file
	#define DRP_SIZE 0x10000 // 13 bits of DRP address x 32 bit words = 15 bits of address
	#define DRP_BASE 0x54000000 // DRP area base address
	// AXI versions of mwrite and mread support only 8-byte transfers
	#define mwrite(a,b,c,d) {*((uint64_t*)(sys_vptr + d)) = *b;}
	#define mread(a,b,c,d)  {volatile uint64_t dum = *((uint64_t*)(sys_vptr + d)); usleep(1); *b = *((uint64_t*)(sys_vptr + d));}
// debug versions of the macros
//	#define mwrite(a,b,c,d) {printf ("mwrite: a: %08x d: %016llx\n", d, *b); fflush(stdout); *((uint64_t*)(sys_vptr + d)) = *b;}
//	#define mread(a,b,c,d)  {printf ("mread : a: %08x\n", d); fflush(stdout); *b = *((uint64_t*)(sys_vptr + d));}
	#define mopen(d) open_dev_mem()

#endif

#define PORT_MARK   0x40000000 // port marker for register offsets
#define PORT_UNMARK 0x3FFFFFFF // mask for removing PORT_MARK from offsets

#define MGT_TYPE    0
#define GTH	    0
#define GTX         1

class bit_range
{
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
	ar& low;
	ar& high;
	ar& mask;
    }

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
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
	ar& offset;
	ar& reg_rng;
	ar& att_rng;
    }

public:
    int offset;
    bit_range reg_rng; // bits in register representing this part
    bit_range att_rng; // bits in attribute corresponding
    part_att (int off, bit_range reg, bit_range att){offset = off; reg_rng = reg; att_rng = att;}
    part_att (){offset = -1;}
};

class attribute
{
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
	ar& name;
	ar& brange;
	ar& att_enc;
	ar& p_reg;
	ar& value;
	ar& valid_value;
	ar& drp_reg;
	ar& read_only;
	ar& unit;
    }

public:
    string name;
    bit_range brange; // full range of this attribute
    map <string,int> att_enc; // contains all conversions from text attribute values to binary
    vector <part_att> p_reg; // register parts that together form this attribute
    boost::multiprecision::uint128_t value; // value of the attribute
    bool valid_value;
    bool drp_reg; // this is drp register, not a port
    bool read_only;
    string unit; // which unit inside MGT does this attribute/port belongs to (RX, TX, CPLL)
    attribute(){}
    attribute(string nm, const bit_range& ar){name = nm; brange = ar; valid_value = false;}
    // add new attribute part to this attribute, expand range
    void add_part (part_att pa) {p_reg.push_back(pa); brange.expand(pa.att_rng);}
};

class register_prop
{
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
	ar& value;
	ar& drp_reg;
	ar& read_only;
    }

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
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar& prescale;
	ar& rxout_div;
	ar& rx_data_width;

	ar& eq_mode;
	ar& h_max;
	ar& v_max;
	ar& h_step;
	ar& v_step;

	ar& common;
	ar& common_is_used;
	ar& base_addr;
	ar& rx_group_name;
	ar& rx_group_index;
	ar& tx_group_name;
	ar& tx_group_index;

	ar& rx_active;
	ar& tx_active;
	ar& x;
	ar& y;
	ar& tx_protocol_path;
	ar& tx_pll;
	ar& tx_refclk_f;

	ar& rx_protocol_path;
	ar& rx_pll;
	ar& rx_refclk_f;
	ar& quad_address;

	ar& quad_drp_addr1 ;
	ar& quad_drp_addr0 ;
	ar& quad_port_addr1;
	ar& quad_port_addr0;

	ar& atts;
	ar& registers;
	ar& MEM_BASE;
	ar& tx_mmcm_slaves;
    }

public:
    // Control the prescaling of the sample count to keep both sample
    // count and error count in reasonable precision within the 16-bit
    // register range.
    // Valid values: from 0 to 31.
    boost::multiprecision::uint128_t prescale;

    // QPLL/CPLL output clock divider D for the RX datapath.
    // Valid values: 1, 2, 4, 8, 16.
    boost::multiprecision::uint128_t rxout_div;

    // Defines the width of valid data on Rdata and Sdata buses.
    // Valid values: 16, 20, 32, 40.
    boost::multiprecision::uint128_t rx_data_width;

    // Equalizer mode: LPM linear eq. or DFE eq.
    // When in DFE mode (RXLPMEN=0), due to the unrolled first DFE tap,
    // two separate eye scan measurements are needed, one at +UT and
    // one at -UT, to measure the TOTAL BER at a given vertical and
    // horizontal offset.
    // Valid values = 'LPM', 'DFE'.
    string eq_mode;
    int h_max;
    int v_max;
    int h_step;
    int v_step;

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

    drp_unit    (int base_a, int mem_base, int full_drp_addr_width);
    drp_unit ()
    {
        prescale = 0;
        rxout_div = 1;
        rx_data_width = 40;
        eq_mode = "LPM";
        h_max = 32;
        v_max = 127;
        h_step = 1;
        v_step = 2;
    };

    int  read_config (std::string fname, bool drp_reg);
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
    void reset_qpll_v7_gth (int fd);
    void reset_qpll_usplus_gth (int fd);
    void read_registers_prn (int fd);
    void att_write (int fd, string name, boost::multiprecision::uint128_t value);
    void wait_for (int fd, string name, int t);
    boost::multiprecision::uint128_t att_read  (int fd, string name, string &svalue);
    boost::multiprecision::uint128_t att_read_prn  (int fd, string name);
    boost::multiprecision::uint128_t att_read_eye  (int fd, string name);
    boost::multiprecision::uint128_t verilog_value (string s);

    uint64_t reg_read  (int fd, int addr);
    void     reg_write (int fd, int addr, uint64_t data);

    void eyescan_config(int fd, int x, int y);
		void eyescan_config_gty(int fd, int x, int y);
    bool eyescan_control(int fd, int x, int y, bool err_det_en, bool run, bool arm);
    bool eyescan_offset (int fd, int x, int y, int hor_offset, int ver_offset, int ut_sign);
		bool eyescan_offset_gty (int fd, int x, int y, int hor_offset, int ver_offset, int ut_sign);
    bool eyescan_wait (int fd, int x, int y, string wait_for);
    map <string,boost::multiprecision::uint128_t> eyescan_acquisition(int fd, int x, int y, int hor_offset, int ver_offset);
		map <string,boost::multiprecision::uint128_t> eyescan_acquisition_gty(int fd, int x, int y, int hor_offset, int ver_offset);
    void eyescan_sweep (int fd, int x, int y, int scale, int i, int mode);
		void eyescan_sweep_gty (int fd, int x, int y, int scale, int i, int mode);
    void eyescan_complete (int fd, int x, int y, int scale, int i, int mode);
		void eyescan_complete_gty (int fd, int x, int y, int scale, int i, int mode);

    string print();
};

#endif // DRP_UNIT_H
