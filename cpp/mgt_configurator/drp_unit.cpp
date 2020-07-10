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


#include "drp_unit.h"


// constructor from verilog-style bit range
bit_range::bit_range(string s)
{
    int n = sscanf(s.c_str(), "%d:%d", &high, &low);
    if (n < 2) low = high; // single bit range
    make_mask();
}

string bit_range::print()
{
    ostringstream res;
    res << high << ":" << low;
    return res.str();
}

drp_unit::drp_unit(int base_a, int full_drp_addr_width)
{
    // convert to actual base address, taking device base into account
    // base_addr is in 64-bit words

    // split out quad address
    quad_address = base_a >> full_drp_addr_width;
    int base_a_noq = base_a & ((1<<full_drp_addr_width)-1); // leave only addres inside quad

    MEM_BASE = 0xC0000/8;
    base_addr = base_a_noq + MEM_BASE; // true base address without quad number
//    printf("new drp_unit: %05x %05x %02x %05x\n", base_a, base_a_noq, quad_address, base_addr);

    rx_active = tx_active = common = false;
    x = y = -1;
    tx_group_name = ""; tx_group_index = -1;
    rx_group_name = ""; rx_group_index = -1;
    common_is_used = false;
}

int drp_unit::read_config (std::string fname, bool drp_reg)
{
    ifstream file(fname.c_str(), ifstream::in);
    string str;
    string att_name, att_name_old;
    int line_count = 0;
    while (getline(file, str)) // read line by line
    {

        if (str.find("//") != string::npos) continue; // skip commented lines

        vector <string> fld;

        boost::split(fld, str, boost::is_any_of("\t")); // split on tabs
        if (fld.size() < 7) continue; // malformed line

        for (int bi = 0; bi < 7; bi++) boost::trim(fld[bi]);

        // check if first field has contents, for new register
        att_name_old = att_name; // remember name for the entries with just encoded attributes
        att_name = fld.at(3);
        string att_str  = fld.at(5); // attribute encoded as text
        string bit_enc  = fld.at(6); // corresponding attribute binary value

        if (!att_name.empty()) // this line contains attribute name
        {

            string reg_addr_fld = string("0x") + fld.at(0); // add prefix to register address
            int reg_addr = strtol (reg_addr_fld.c_str(), NULL, 16); // register address converted
            bit_range reg_range (fld.at(1)); // register bit range
            bit_range att_range (fld.at(4)); // corresponding attribute bit range
            bool read_only = fld.at(2).compare("R") == 0; // read-only attribute

            if (atts.find(att_name) == atts.end()) // no record of this attribute, make one
            {
//                if (reg_addr == 0 && drp_reg)
//                    cout << "creating new attribute: " << att_name << "   " << att_range.high << ":" << att_range.low
//                         << " off: " << reg_addr << endl;

                attribute na(att_name, att_range);
                na.drp_reg = drp_reg;
                na.read_only = read_only;
                atts.insert(make_pair(att_name, na));
            }

            attribute ea = atts.at(att_name); // get existing attribute (which may or may not have been just created above)
            part_att pa (reg_addr, reg_range, att_range); // new attribute part
            ea.add_part(pa); // add this part into attribute's list of parts, expand bit range

            atts.erase  (att_name);
            atts.insert (make_pair(att_name, ea)); // re-insert the attribute
        }

        if (!att_str.empty()) // encoded attribute line
        {
            if (att_name.empty()) att_name = att_name_old; // get the name in case it's not in this line

            attribute ea = atts.at(att_name); // get existing attribute with old name

            if (att_str.find('-') == string::npos && bit_enc.find('-') == string::npos) // if valid encoding
            {
                // create encoded field
                int bit_val = strtol (bit_enc.c_str(), NULL, 10);
                ea.att_enc.insert(make_pair(att_str, bit_val));
                //cout << "encoded attribute: " << att_str << " " << bit_val << " map size: " << ea.att_enc.size() << " " << ea.name << endl;
            }
            atts.erase  (att_name);
            atts.insert (make_pair(att_name, ea)); // re-insert the attribute
        }
        line_count++;
    }
    return line_count;
}

boost::multiprecision::uint128_t drp_unit::verilog_value (string s)
{
    int bn;
    char base;
    char sval[200];
    boost::multiprecision::uint128_t val = 0;

    int n = sscanf (s.c_str(), "%d'%c%s", &bn, &base, sval);
    if (n == 3)
    {

        string ssval(sval);
        int slng = ssval.length();

        int based = 0;
        switch (base)
        {
            case 'b': based = 2;  break;
            case 'd': based = 10; break;
            case 'h': based = 16; break;
        }

        if (based == 16 && slng > 16) // string is too long for stoull conversion
        {
            ssval = string("0x") + ssval;
            boost::multiprecision::uint128_t val2 (ssval);
            val = val2;
        }
        else
        {
            val = strtoull(ssval.c_str(), 0, based);
        }
        //cout << dec << bn << " " << base << " " << sval << " " << hex << val << endl;
    }

    return val;
}

int drp_unit::read_units (string fname)
{
    //cout << "reading units file: " << x << " " << y << " " << fname << endl;

    ifstream file(fname.c_str(), ifstream::in);
    string str;
    string att_name;
    int n = 0;
    int rx_count = 0, tx_count = 0, cpll_count = 0, no_count = 0;

    while (getline(file, str)) // read line by line
    {

        if (str.find("//") != string::npos) continue; // skip commented lines

        vector <string> fld;

        boost::split(fld, str, boost::is_any_of("\t")); // split on tabs

        if (fld.size() < 2 || fld[0].size() == 0) continue; // malformed line

        for (int bi = 0; bi < 2; bi++) boost::trim(fld[bi]);
        att_name = fld[0];
        string att_unit = fld[1];


        // find attribute with this name and assign value
        if (atts.count(att_name) == 1) // attribute in DRP map
        {
            attribute attr = atts.at(att_name);

            attr.unit = att_unit;

            // reinsert attribute
            atts.erase(att_name);
            atts.insert(make_pair(att_name, attr));
            if (att_unit.compare("rx") == 0) rx_count++; else
            if (att_unit.compare("tx") == 0) tx_count++; else
            if (att_unit.compare("cpll") == 0) cpll_count++; else
                no_count++;
        }
        n++;
    }
    file.close ();

//    cout << "XY: " << x << " " << y << " unit counts: rx: " << rx_count << " tx: " << tx_count
//         << " cpll: " << cpll_count << " no: " << no_count << endl;

    return n;
}


// read both params files TX and RX
void drp_unit::read_params ()
{
    // read parameters for both rx and tx protocols, but only corresponding parts (marked in "units" file)
    // make sure there are no conflicts in parameters for tx and rx in common atts
    int lines = 0;
    if (tx_active)
    {
        string tx_param_fname = tx_protocol_path + "/mgt_attributes.tab";
        lines = read_params_rx_tx (tx_param_fname, "tx");
        if (lines < 10) cout << "problem reading file: " << tx_param_fname;

        if (!(common_unit->common_is_used)) // qpll is not used yet
        {
            // tell COMMON unit to read its parameters
            tx_param_fname = tx_protocol_path + "/common_attributes.tab";
            lines = common_unit->read_params_rx_tx (tx_param_fname, "common");
            if (lines < 10) cout << "problem reading file: " << tx_param_fname;

            if (tx_pll.compare("Q") == 0)
                common_unit->common_is_used = true; // mark as used
        }
    }

    if (rx_active)
    {
        string rx_param_fname = rx_protocol_path + "/mgt_attributes.tab";
        lines = read_params_rx_tx (rx_param_fname, "rx");
        if (lines < 10) cout << "problem reading file: " << rx_param_fname;

        if (!(common_unit->common_is_used)) // qpll is not used yet
        {
            // tell COMMON unit to read its parameters
            rx_param_fname = rx_protocol_path + "/common_attributes.tab";
            lines = common_unit->read_params_rx_tx (rx_param_fname, "common");
            if (lines < 10) cout << "problem reading file: " << rx_param_fname;

            if (rx_pll.compare("Q") == 0)
                common_unit->common_is_used = true; // mark as used
        }
    }

}

// read one of the params files (RX or TX)
int drp_unit::read_params_rx_tx(string param_fname, string unit)
{


    ifstream file(param_fname.c_str(), ifstream::in);
    string str;
    string att_name;
    //bool drp_reg = true; // file starts with DRP registers
    boost::multiprecision::uint128_t att_value;
    //bool filename_reported = false;

    // select PLL marker to use in the code below
    string pll;
    if (unit.compare("tx") == 0) pll = tx_pll;
    else pll = rx_pll;

    int i = 0;
    while (getline(file, str)) // read line by line
    {

        //if (str.find("// PORTS") != string::npos) drp_reg = false; // port list starts

        if (str.find("//") != string::npos) continue; // skip commented lines

        vector <string> fld;

        boost::split(fld, str, boost::is_any_of("\t")); // split on tabs

        if (fld.size() < 2 || fld[0].size() == 0) continue; // malformed line

        for (int bi = 0; bi < 2; bi++) boost::trim(fld[bi]);
        att_name = fld[0];
        string att_val_str = fld[1];

        // find attribute with this name and assign value
        if (atts.count(att_name) == 1) // attribute in DRP map
        {
            attribute attr = atts.at(att_name);

            // special cases of tied_to_XXX
            int bit_num = attr.brange.high - attr.brange.low + 1;
            ostringstream avs;
            if (att_val_str.find("tied_to_ground") != string::npos)
            {
                avs << bit_num << "'b0";
                att_val_str = avs.str();
            }
            if (att_val_str.find("tied_to_vcc") != string::npos)
            {
                avs << bit_num << "'b";
                for (int i = 0; i < bit_num; i++) avs << "1";
                att_val_str = avs.str();
            }

            if (att_val_str.find('\'') != string::npos) // verilog value
            {
                att_value = verilog_value (att_val_str); // decode
            }
            else if (attr.att_enc.size() > 0) // encoder map is not empty
            {
                if (attr.att_enc.count(att_val_str) == 1) // this encoded value exists
                {
                    att_value = attr.att_enc.at(att_val_str); // extract value
                }
                else
                {
                    cerr << "unknown encoding: attr: " << att_name << " value: " << att_val_str << endl;
                }
            }
            else
            {
                // not verilog and not encoded value, must be just a decimal
                try
                {
                    att_value = strtoull(att_val_str.c_str(), 0, 10);
                }
                catch (const std::invalid_argument& ia)
                {
                    //cout << "unconvertable value: attr: " << att_name << " " << att_val_str << '\n';
                }
            }

            if (attr.unit.compare(unit) == 0) // this attribute intended for current unit (TX or RX)
            {
                // just store attribute
                attr.value = att_value; // assign value
                attr.valid_value = true; // mark attribute value as valid
            }
            else if (attr.unit.compare("cpll") == 0 && pll.compare("C") == 0) // cpll attribute and the unit is using cpll
            {
                // check for conflicts
                if (attr.valid_value && attr.value != att_value)
                {
                    cout << "CPLL att/port conflict: XY: "<< x << " " << y << " unit: " << unit
                         << " att/port: " << attr.name << " value: " << attr.value << " overwrite: " << att_value << endl;
                }

                // store attribute
                attr.value = att_value; // assign value
                attr.valid_value = true; // mark attribute value as valid

            }
            else
            {
                if (attr.unit.compare("") == 0) // un-united attribute/port
                {
                    // check for conflicts
                    if (!common || common_is_used) // only check if 1. MGT unit or 2. COMMON is already in use
                    {
                        if (attr.valid_value && attr.value != att_value)
                        {
                            cout << "un-united att/port conflict: XY: "<< x << " " << y << " unit: " << unit
                                 << " att/port: " << attr.name << " value: " << attr.value << " overwrite: " << att_value << endl;
                        }
                    }

                    // store attribute
                    attr.value = att_value; // assign value
                    attr.valid_value = true; // mark attribute value as valid
                }
            }


            // reinsert attribute
            atts.erase(att_name);
            atts.insert(make_pair(att_name, attr));
            i++;
        }
        else
        {
            // secret attribute not in DRP map
            // a number of ports may not be in the map, so ignore missing ports
//            if (!filename_reported)
//                cout << "reading parameter file: " << x << " " << y << " " << param_fname << endl;
//            filename_reported = true;
//            cout << "WARNING: attribute not in DRP map: " << att_name << endl;
        }
    }
    file.close ();
    return i;
}

void drp_unit::fill_registers()
{
    for (map<string,attribute>::iterator it = atts.begin(); it != atts.end(); ++it) // scan all attributes
    {
        string mname = it->first;
        attribute ea = it->second;
        string aname = ea.name;
        //bit_range br = ea.brange;
        bool valid_value = ea.valid_value;
        bool read_only = ea.read_only;
        bool drp_reg = ea.drp_reg;

        boost::multiprecision::uint128_t value = ea.value;

        if (valid_value || read_only) // only fill registers that need setting (valid_value == true) or are read_only
        {
            for (vector <part_att>::iterator rit = ea.p_reg.begin(); rit != ea.p_reg.end(); ++rit) // scan register parts inside attribute
            {
                int offset = rit->offset;
                if (!drp_reg) offset += PORT_MARK; // mark as port
                bit_range reg_rng = rit->reg_rng;
                bit_range att_rng = rit->att_rng;

                if (att_rng.width() != reg_rng.width())
                    cout << "attribute and register widths don't match: " << aname << " " << att_rng.print() << " " << reg_rng.print() << endl;

                int bit_cnt = att_rng.width(); // count of bits in this part
                // prepare part of the register
                boost::multiprecision::uint128_t part_val = value >> att_rng.low;
                boost::multiprecision::uint128_t part_msk = (1 << bit_cnt) - 1;
                part_val &= part_msk; // shifted and masked

                // prepare to put into register
                part_val <<= reg_rng.low;
                part_msk <<= reg_rng.low;
                uint32_t part_val_trim = part_val.convert_to<uint32_t>();
                uint32_t part_msk_trim = part_msk.convert_to<uint32_t>();

                if (registers.count(offset) == 0) // no such register yet
                {
                    register_prop *reg = new register_prop();
                    reg->drp_reg = drp_reg; // if one attribute is DRP, the entire register is DRP
                    reg->read_only = read_only; // if one att is read-only, the entire register is
                    registers.insert(make_pair(offset, *reg)); // make new register with zero contents
                }
                if (!read_only) // do not fill read-only registers
                {
                    // need here to extract current reg. value and update with part_val
                    register_prop reg = registers.at(offset);
                    uint32_t reg_bits = reg.value; // find register for this offset
                    // check if these bits are not zeros and not equal to what we're about to write, give warning
                    if ((reg_bits & part_msk_trim) != 0 && (reg_bits & part_msk_trim) != part_val_trim)
                        cout << "overwriting existing bits in register: " << hex << aname << " bits: " << reg_rng.print()
                             << " old: " << (reg_bits & part_msk_trim) << " new: " << part_val_trim << endl;

                    reg_bits &= ~part_msk_trim;
                    reg_bits |= part_val_trim; // update bits with new value
                    reg.value = reg_bits;
                    registers.erase(offset); // insert back
                    registers.insert(make_pair(offset, reg));
                }
    //            cout << aname << " " << rit->offset << " " << reg_rng.print() << " " << att_rng.print() << endl;
            }
        }
    }
}

void drp_unit::write_registers (int fd)
{
    // write all registers
    for (map<int,register_prop>::iterator it = registers.begin(); it != registers.end(); ++it)
    {
        int offset = (it->first) & PORT_UNMARK;

        register_prop reg = it->second;
        if (!reg.read_only) // read/write register
        {

//            if (reg.drp_reg) continue; // skip DRP for now DEBUG

            // write quad selector
            reg_write (fd, reg.drp_reg ? quad_drp_addr0 : quad_port_addr0, 1 << quad_address);

            uint64_t value = reg.value;
            int saddr = (base_addr + offset)*8; // full register address, converted to bytes

            mwrite (fd, &value, 8, saddr);
            //if (reg.drp_reg && offset == 0)  printf("write_registers DRP  0: %08lx\n", value);
            //if (!reg.drp_reg && offset == 0) printf("write_registers port 0: %08lx\n", value);
        }
    }

}

void drp_unit::read_registers (int fd)
{
    // read registers into map
    map<int, register_prop> reg_rd;
    for (map<int,register_prop>::iterator it = registers.begin(); it != registers.end(); ++it)
    {
        int offset = (it->first) & PORT_UNMARK;
        register_prop reg = it->second;
        uint64_t rb = 0;
        int saddr = (base_addr + offset)*8; // full register address, converted to bytes

        // write quad selector
        reg_write (fd, reg.drp_reg ? quad_drp_addr0 : quad_port_addr0, 1 << quad_address);

        mread (fd, &rb, 8, saddr);

        reg.value = rb;
        reg_rd.insert(make_pair(it->first, reg)); // insert into new map, keep original offset for ports
    }
    registers = reg_rd; // replace map with the new one
//    cout << "XY: " << x << " " << y << " register map size: " << registers.size() << endl;
}

void drp_unit::check_registers (int fd)
{
    // read back and compare
    cout << "       checking registers: " << dec << x << " "<< y << endl;
    for (map<int,register_prop>::iterator it = registers.begin(); it != registers.end(); ++it)
    {
        int offset = (it->first) & PORT_UNMARK;
        register_prop reg = it->second;
        if (!reg.read_only) // only R/W registers
        {
            uint64_t value = reg.value & 0xffffULL;
            uint64_t rb = 0;
            int saddr = (base_addr + offset)*8; // full register address, converted to bytes

            // write quad selector
            reg_write (fd, reg.drp_reg ? quad_drp_addr0 : quad_port_addr0, 1 << quad_address);

            mread (fd, &rb, 8, saddr);

            rb &= 0xffffULL; // only 16 lower bits matter
            if (value != rb)
                printf ("ERROR: drp: %d off: %04x w: %04lx r: %04lx e: %04lx\n",
                       reg.drp_reg, offset, value, rb, (rb ^ value));
        }
    }
}

void drp_unit::read_registers_prn (int fd)
{
    cout << "reading registers: " << x << " "<< y << endl;
    for (map<int,register_prop>::iterator it = registers.begin(); it != registers.end(); ++it)
    {
        int offset = (it->first) & PORT_UNMARK;
        register_prop reg = it->second;
        if (!reg.read_only) // only R/W registers
        {
            //uint64_t value = reg.value & 0xffffULL;
            uint64_t rb = 0;
            int saddr = (base_addr + offset)*8; // full register address, converted to bytes

            // write quad selector
            reg_write (fd, reg.drp_reg ? quad_drp_addr0 : quad_port_addr0, 1 << quad_address);

            mread (fd, &rb, 8, saddr);

            uint16_t rb16 = rb & 0xffffULL; // only 16 lower bits matter
            printf ("drp: %d off: %03x rb: %04x\n", reg.drp_reg, offset, rb16);
        }
    }

}

void drp_unit::read_print_atts (int fd)
{

    for (map<string,attribute>::iterator it = atts.begin(); it != atts.end(); ++it) // scan all attributes
    {
        string mname = it->first;
        string svalue;
        att_read  (fd, mname, svalue);
        cout << mname << "\t" << svalue << endl;
    }
}

void drp_unit::check_atts ()
{
    int i = 0, j = 0;
    for (map<string,attribute>::iterator it = atts.begin(); it != atts.end(); ++it) // scan all attributes
    {
        string mname = it->first;
        attribute ea = it->second;
        string aname = ea.name;
        //bit_range br = ea.brange;
        bool valid_value = ea.valid_value;
        boost::multiprecision::uint128_t value = ea.value;
        bool drp_reg = ea.drp_reg;

        i++;
        if (valid_value)
        {
            for (vector <part_att>::iterator rit = ea.p_reg.begin(); rit != ea.p_reg.end(); ++rit) // scan register parts inside attribute
            {
                j++;
                int offset = rit->offset;
                if (drp_reg) offset += PORT_MARK;
                bit_range reg_rng = rit->reg_rng;
                bit_range att_rng = rit->att_rng;

                if (att_rng.width() != reg_rng.width())
                    cout << "attribute and register widths don't match: " << aname << " " << att_rng.print() << " " << reg_rng.print() << endl;

                int bit_cnt = att_rng.width(); // count of bits in this part
                // prepare part of the register
                boost::multiprecision::uint128_t part_val = value >> att_rng.low;
                boost::multiprecision::uint128_t part_msk = (1 << bit_cnt) - 1;
                part_val &= part_msk; // shifted and masked

                // prepare to put into register
                part_val <<= reg_rng.low;
                part_msk <<= reg_rng.low;
                uint32_t part_val_trim = part_val.convert_to<uint32_t>();
                uint32_t part_msk_trim = part_msk.convert_to<uint32_t>();

                if (registers.count(offset) > 0) // check that register is in the map
                {
                    register_prop reg = registers.at(offset); // find register for this offset
                    uint32_t reg_bits = reg.value;
                    // check if these bits are not zeros and not equal to what we're about to write, give warning
                    uint32_t part_reg = reg_bits & part_msk_trim; // part of the register carrying the value
                    if (part_reg != part_val_trim)
                    {
                        cout << hex << "att: " << aname << " reg: " << offset << " w: " << part_val_trim << " r: " << part_reg << endl;
                    }
                }
            }
        }
    }
    cout << "checked " << i << " attributes and " << j << " parts" << endl;
}

string drp_unit::print()
{
    ostringstream res;
    for (map<string,attribute>::iterator it = atts.begin(); it != atts.end(); ++it) // scan all attributes
    {
        string mname = it->first;
        attribute ea = it->second;
        string aname = ea.name;
        bit_range br = ea.brange;

        res << aname << " " << br.print() << endl;

        for (map <string,int>::iterator ae = ea.att_enc.begin(); ae != ea.att_enc.end(); ++ae)
        {
            res << "encoding: " << ae->first << " " << ae->second << endl;
        }

        for (vector <part_att>::iterator pr = ea.p_reg.begin(); pr != ea.p_reg.end(); ++pr)
        {
            part_att pa = *pr;
            res << "register part: " << hex << pa.offset << " " << pa.reg_rng.print() << " " << pa.att_rng.print() <<  endl;
        }

    }

    return res.str();
}

// return mask corresponding to this bitrange
void bit_range::make_mask()
{
    mask = 1;
    mask = (mask << width()) - 1; // mask is now in lower bits
    mask <<= low; // now in proper position
}

void drp_unit::att_write (int fd, string name, boost::multiprecision::uint128_t value)
{
    uint64_t rb64;
    boost::multiprecision::uint128_t rb, avalue = 0;
    // find attribute with that name
    if (atts.count(name) == 1)
    {
        attribute a = atts.at(name);

        // scan all register parts
        for (vector<part_att>::iterator it = a.p_reg.begin(); it != a.p_reg.end(); ++it)
        {
            part_att pa = *it;
            bit_range att_rng = pa.att_rng; // range of this part in attribute
            bit_range reg_rng = pa.reg_rng; // range of this part in register

            // find that register
            register_prop reg = registers.at(pa.offset + (a.drp_reg ? 0 : PORT_MARK));
            // write quad selector
            reg_write (fd, reg.drp_reg ? quad_drp_addr0 : quad_port_addr0, 1 << quad_address);

            // read this part
            int saddr = (base_addr + pa.offset)*8; // full register address, converted to bytes
            mread (fd, &rb64, 8, saddr);

            rb = (boost::multiprecision::uint128_t) rb64;

            rb &= ~reg_rng.mask; // clean out the bits to change

            avalue = value & att_rng.mask; // select only bits for this register
            avalue >>= att_rng.low; // bits that we need to place are now in low bits
            avalue <<= reg_rng.low; // now they are in proper position for register

            rb |= avalue; // merge new bits
            rb64 = rb.convert_to<uint64_t>();
            mwrite (fd, &rb64, 8, saddr); // write register back
        }
    }
    else
    {
        cout << "WRITE ERROR: attribute or port not found: " << name << endl;
    }
}

uint64_t drp_unit::reg_read (int fd, int addr)
{
    // must select quad before using this
    uint64_t rb64;
    int saddr = (MEM_BASE + addr)*8; // full register address, converted to bytes
    mread (fd, &rb64, 8, saddr);
    return rb64;
}

void drp_unit::reg_write (int fd, int addr, uint64_t data)
{
    // must select quad before using this
    int saddr = (MEM_BASE + addr)*8; // full register address, converted to bytes
    mwrite (fd, &data, 8, saddr);
}


boost::multiprecision::uint128_t drp_unit::att_read  (int fd, string name, string &svalue)
{
    uint64_t rb64;
    boost::multiprecision::uint128_t rb, avalue = 0;
    // find attribute with that name
    if (atts.count(name) == 1)
    {
        attribute a = atts.at(name);

        // scan all register parts
        for (vector<part_att>::iterator it = a.p_reg.begin(); it != a.p_reg.end(); ++it)
        {
            part_att pa = *it;
            bit_range att_rng = pa.att_rng; // range of this part in attribute
            bit_range reg_rng = pa.reg_rng; // range of this part in register
            int offset_reg = pa.offset + (a.drp_reg ? 0 : PORT_MARK);


            // find that register
            if (registers.count(offset_reg) > 0)
            {
                register_prop reg = registers.at(offset_reg);
                // write quad selector
                reg_write (fd, reg.drp_reg ? quad_drp_addr0 : quad_port_addr0, 1 << quad_address);
                // read this part
                int saddr = (base_addr + pa.offset)*8; // full register address, converted to bytes
                mread (fd, &rb64, 8, saddr);
                //cout << "offset: " << hex << pa.offset << dec << " reg_rng: " << pa.reg_rng.print()
                //     << " att_rng: " << pa.att_rng.print() << " data: " << hex << rb64 << endl;

                rb = (boost::multiprecision::uint128_t) rb64;
            }
            else
                rb = -1; // invalid value if the attribute/port was not in registers


            rb &= reg_rng.mask; // leave only relevant bits
            rb >>= reg_rng.low; // now the relevant portion is in low bits
            rb <<= att_rng.low; // now the relevant portion is in proper position for attribute

            avalue |= rb; // place into final result
//            cout << hex << "link: " << group_name << group_index
//                 << " port: " << name << " base: " << base_addr << " off: " << pa.offset << " reg_rng " << reg_rng.print()
//                 << " att_rng: " << att_rng.print() << endl;
        }

        // find the string representation if any
        svalue = "NOT_FOUND";
        bool v_found = false;
        for (map <string,int>::iterator it = a.att_enc.begin(); it != a.att_enc.end(); ++it)
        {
            string sval = it->first; // string representation
            int val = it->second; // corresponding integer value
            if (val == avalue)
            {
                // found the value
                svalue = sval;
                v_found = true;
                break;
            }
        }
        if (!v_found)
        {
            // attribute does not have encoded values, print hex in verilog syntax
            ostringstream osvalue;
            if (a.brange.width() == 1)
                osvalue << dec << a.brange.width() << "'b" << avalue;
            else
                osvalue << dec << a.brange.width() << "'h" << hex << avalue;
            svalue = osvalue.str();
        }
    }
    else
    {
        cout << "READ ERROR: attribute or port not found: " << name << endl;
        return -1;
    }
    return avalue;
}


boost::multiprecision::uint128_t drp_unit::att_read_prn(int fd, string name)
{
    string svalue;
    boost::multiprecision::uint128_t v  = att_read(fd, name, svalue);
    cout << name << " = " << svalue << endl;
    return v;
}


void drp_unit::wait_for (int fd, string name, int t)
{
    int i;
    string dum;
    for (i = 0; i < 20; i++)
    {
        boost::multiprecision::uint128_t v = att_read (fd, name, dum);
        if (v == t) break;
//        if (i > 10) usleep (10000);
    }
    if (i == 20) cout << dec << x << " " << y << " " << name << " == " << t << " did not assert" << endl;
//    else cout << "wait counter: " << i << endl;

}

void drp_unit::reset_cpll(int fd)
{
    string dum;
    if (tx_pll.compare("C") == 0 || rx_pll.compare("C") == 0)
    {
        if (att_read(fd, "CPLLPD", dum) != 0)
        {
            cout << "power up CPLL: " << dec << x << " " << y << endl;
            att_write(fd, "CPLLPD", 0);
            usleep (10000);
        }

        cout << "resetting CPLL in mgt: " << x << " " << y << endl;
        att_write (fd, "CPLLRESET", 1);
        att_write (fd, "CPLLRESET", 0);
        wait_for  (fd, "CPLLLOCK", 1);
    }
    else
    {
        cout << "power down CPLL in mgt: " << x << " " << y << endl;
        att_write (fd, "CPLLPD", 1);
    }
}

void drp_unit::reset_qpll(int fd)
{
    string dum;
    if (common_is_used) // only if this QPLL is used
    {
        if (att_read(fd, "QPLLPD", dum) != 0)
        {
            cout << "power up QPLL: " << dec << x << " " << y << endl;
            att_write(fd, "QPLLPD", 0);
            usleep (10000);
        }

        cout << "resetting QPLL in quad: " << quad_address << endl;
        att_write (fd, "QPLLRESET", 1);
        att_write (fd, "QPLLRESET", 0);
        wait_for  (fd, "QPLLLOCK", 1);
    }
    else
    {
        cout << "power down QPLL in quad: " << quad_address << endl;
        att_write(fd, "QPLLPD", 1);
    }
}
#define each_slave(s) for (vector<drp_unit>::iterator s = tx_mmcm_slaves.begin(); s != tx_mmcm_slaves.end(); ++s)

void drp_unit::tx_phase_align(int fd)
{

    att_write    (fd, "TXSYNC_OVRD", 1);
    att_write    (fd, "TXPHDLYRESET", 0);
    att_write    (fd, "TXDLYBYPASS", 0);
    att_write    (fd, "TXPHALIGNEN", 1);

//    att_write    (fd, "GTTXRESET", 1);
//    att_write    (fd, "GTTXRESET", 0);
//    att_write    (fd, "TXUSERRDY", 1);
//    wait_for     (fd, "TXRESETDONE", 1);

    each_slave(su)
    {

        su->att_write    (fd, "TXSYNC_OVRD", 1);
        su->att_write    (fd, "TXPHDLYRESET", 0);
        su->att_write    (fd, "TXDLYBYPASS", 0);
        su->att_write    (fd, "TXPHALIGNEN", 1);

//        su->att_write    (fd, "GTTXRESET", 1);
//        su->att_write    (fd, "GTTXRESET", 0);
//        su->att_write    (fd, "TXUSERRDY", 1);
//        su->wait_for     (fd, "TXRESETDONE", 1);
    }

    att_write    (fd, "TXDLYSRESET", 1);
    wait_for     (fd, "TXDLYSRESETDONE", 1);
    att_write    (fd, "TXDLYSRESET", 0);

    each_slave(su)
    {
        su->att_write    (fd, "TXDLYSRESET", 1);
        su->wait_for     (fd, "TXDLYSRESETDONE", 1);
        su->att_write    (fd, "TXDLYSRESET", 0);
    }

    att_write    (fd, "TXPHINIT", 1);
    wait_for     (fd, "TXPHINITDONE", 1);
    att_write    (fd, "TXPHINIT", 0);

    att_write    (fd, "TXPHALIGN", 1);
    wait_for     (fd, "TXPHALIGNDONE", 1);
    att_write    (fd, "TXPHALIGN", 0);

    att_write    (fd, "TXDLYEN", 1);
    wait_for     (fd, "TXPHALIGNDONE", 1);
    att_write    (fd, "TXDLYEN", 0);

    each_slave(su)
    {
        su->att_write    (fd, "TXPHINIT", 1);
        su->wait_for     (fd, "TXPHINITDONE", 1);
        su->att_write    (fd, "TXPHINIT", 0);
    }
    each_slave(su)
    {
        su->att_write    (fd, "TXPHALIGN", 1);
        su->wait_for     (fd, "TXPHALIGNDONE", 1);
        su->att_write    (fd, "TXPHALIGN", 0);
    }

    att_write    (fd, "TXDLYEN", 1);
    wait_for     (fd, "TXPHALIGNDONE", 1);

}

void drp_unit::reset_tx (int fd)
{
    string dum;
    if (tx_active)
    {
        cout << "         reset TX unit: " << dec << x << " " << y
             <<" "<< tx_group_name << tx_group_index << endl;

        if (att_read(fd, "TXPD", dum) != 0)
        {
            cout << "power up TX unit: " << dec << x << " " << y << endl;
            att_write    (fd, "TXPD", 0);
            usleep (10000);
        }

        att_write    (fd, "GTTXRESET", 1);
        att_write    (fd, "GTTXRESET", 0);
        att_write    (fd, "TXUSERRDY", 1);
        wait_for     (fd, "TXRESETDONE", 1);
    }
    else
    {
        cout << "power down TX unit: " << dec << x << " " << y << endl;
        att_write    (fd, "TXPD", 3);
    }
}

void drp_unit::reset_rx (int fd)
{
    string dum;
    if (rx_active)
    {
        cout << "         reset RX unit: " << dec << x << " " << y
             <<" "<< rx_group_name << rx_group_index << endl;

        if (att_read(fd, "RXPD", dum) != 0)
        {
            cout << "power up RX unit: " << dec << x << " " << y << endl;
            att_write    (fd, "RXPD", 0);
            usleep (10000);
        }

        // tell DFE to train
//        att_write (fd, "RXDFEAGCHOLD", 0);
//        att_write (fd, "RXDFELFHOLD", 0);

        att_write (fd, "GTRXRESET", 1);
        att_write (fd, "GTRXRESET", 0);
        att_write (fd, "RXUSERRDY", 1);
        wait_for  (fd, "RXRESETDONE", 1);

        att_write (fd, "RXDLYSRESET", 1);
        att_write (fd, "RXDLYSRESET", 0);
        wait_for  (fd, "RXSYNCDONE", 1);
    }
    else
    {
        cout << "power down RX unit: " << dec << x << " " << y << endl;
        att_write    (fd, "RXPD", 3);
    }
}

/*
    if (tx)
    {
        if (tx_active)
        {
            if (tx_pll.compare("C") == 0)
            {
                // reset CPLL

                att_write    (fd, "CPLLRESET", 1);
                att_write    (fd, "CPLLRESET", 0);

//                usleep (100000);
//                att_read_prn (fd, "CPLLLOCK");
                wait_for (fd, "CPLLLOCK");

//                att_read_prn (fd, "CPLLREFCLKLOST");
//                att_read_prn (fd, "CPLLFBCLKLOST");
            }
            else if (tx_pll.compare("Q") == 0)
            {

//                att_write (fd, "CPLLPD", 1); // power down CPLL
                // reset QPLL

                common_unit->att_write    (fd, "QPLLRESET", 1);
                common_unit->att_write    (fd, "QPLLRESET", 0);

//                usleep (100000);
//                common_unit->att_read_prn (fd, "QPLLLOCK");
                common_unit->wait_for (fd, "QPLLLOCK");

//                common_unit->att_read_prn (fd, "QPLLREFCLKLOST");
//                common_unit->att_read_prn (fd, "QPLLFBCLKLOST");
            }

            // transmitter

            att_write    (fd, "TXPHALIGNEN", 1);
//            att_write    (fd, "GTTXRESET", 1);
//            usleep (100000);
//            att_write    (fd, "GTTXRESET", 0);
//            usleep (100000);
//            att_write    (fd, "TXUSERRDY", 1);
//            usleep (100000);
//            att_read_prn (fd, "TXRESETDONE");

            att_write    (fd, "GTTXRESET", 1);
            att_write    (fd, "GTTXRESET", 0);
            att_write    (fd, "TXUSERRDY", 1);
            wait_for     (fd, "TXRESETDONE");

            if (master) // master channel phase alignment
            {
                // buffer bypass reset
                att_write    (fd, "TXDLYSRESET", 1);
                wait_for     (fd, "TXDLYSRESETDONE");
                att_write    (fd, "TXDLYSRESET", 0);

                att_write    (fd, "TXPHINIT", 1);
                wait_for     (fd, "TXPHINITDONE");
                att_write    (fd, "TXPHINIT", 0);

                att_write    (fd, "TXPHALIGN", 1);
                wait_for     (fd, "TXPHALIGNDONE");
                att_write    (fd, "TXPHALIGN", 0);

                att_write    (fd, "TXDLYEN", 1);
                wait_for     (fd, "TXPHALIGNDONE");
                att_write    (fd, "TXDLYEN", 0);
                att_write    (fd, "TXDLYEN", 1);
                wait_for     (fd, "TXPHALIGNDONE");
            }
            else
            {
                // TBD
            }

        }
    }
    else

 */
