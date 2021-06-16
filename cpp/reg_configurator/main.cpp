//    FPGA Register builder quickly builds control register interfaces from a simple spreadsheet
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

#include <iostream>
#include <fcntl.h>
#include "drp_unit.h"

using namespace std;

int main(int argc, char *argv[])
{

    // argv[1] is top config file name
    int base = 0; // base address of register bank, bytes
    drp_unit bank (base);

    string register_offsets = "emtf_pcie_registers.tab";
    int mgt_port_ln  = bank.read_config_emtf (register_offsets, false); // read port map
    cout << "read " << mgt_port_ln << " lines from file: " << register_offsets << endl;

//    bank.read_params     (); // registers' settings. File names are created automatically
    bank.fill_registers  (); // using this to only detect which registers are there, and fill map with offsets

    if (argc > 2)
    {
        int dev_ind = 1;
        ostringstream dev_name;

        dev_name << "/dev/utca_sp12" << dev_ind;
        // open device
        int device_d = ::open(dev_name.str().c_str(),O_RDWR);
        if (device_d < 0)
        {
            printf("ERROR: Can not open device file: %s\n", dev_name.str().c_str());
        }
        else
        {
            printf("opened device: %s\n", dev_name.str().c_str());

        }

        bank.att_read_prn(device_d, "core_fpga_fw_year");
        bank.att_read_prn(device_d, "core_fpga_fw_month");
        bank.att_read_prn(device_d, "core_fpga_fw_day");
        bank.att_read_prn(device_d, "core_fpga_fw_hour");
        bank.att_read_prn(device_d, "core_fpga_fw_min");
        bank.att_read_prn(device_d, "core_fpga_fw_sec");

        bank.att_read_prn(device_d, "ctl_fpga_fw_year");
        bank.att_read_prn(device_d, "ctl_fpga_fw_month");
        bank.att_read_prn(device_d, "ctl_fpga_fw_day");
        bank.att_read_prn(device_d, "ctl_fpga_fw_hour");
        bank.att_read_prn(device_d, "ctl_fpga_fw_min");
        bank.att_read_prn(device_d, "ctl_fpga_fw_sec");

        bank.att_read_prn(device_d, "daq_l1a_del");
        boost::multiprecision::uint128_t value = 0x123;
        bank.att_write (device_d, "daq_l1a_del", value);
        bank.att_read_prn(device_d, "daq_l1a_del");
    }

    return 0;
}
