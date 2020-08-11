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

#include <iostream>
#include <fcntl.h>
#include "drp_unit.h"
#include "fpga.h"
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

int main(int argc, char *argv[])
{

    ostringstream dev_name;

    if (argc > 1)
    {
        // argv[1] is top config file name
        fpga chip;
        chip.read_top_config (argv[1]); // top configuration file
        chip.read_mgt_config (); // read MGT configuration
        chip.read_mgt_list   (); // fpga link base addresses
        chip.read_links      (); // link config file, same as what java script reads
        chip.read_params     (); // DRP registers' and port settings for each link, according to protocol. File names are created automatically
        chip.read_inversions (); // read inversion flags for each link
        chip.fill_registers  (); // using this to only detect which registers are there, and fill map with offsets
        chip.read_tx_mmcm_map(); // read map of TX clock-sharing MMCMs

		// open devices
		string device_prefix = chip.device_prefix;
		int device_count = chip.device_count;
		int device_d = -1;
		int fd[device_count];
		string device_name[device_count];
		bool device_selected[device_count];
		memset(device_selected, 0, sizeof(device_selected)); // unselect all devices initially

		for (int i = 0; i < device_count; i++)
		{
 			dev_name.str("");
 			dev_name.clear();
 			dev_name << device_prefix << i;
			device_name[i] = dev_name.str().c_str();
            // open device
            fd[i] = ::open(device_name[i].c_str(), O_RDWR);
            if (fd[i] < 0)
            {
                printf("ERROR: Can not open device file: %s\n", device_name[i].c_str());
            }
		}

		char * line;
		string sline;
		while(1)
		{   
			line = readline("mgtc> ");
			sline = (string)line;
			boost::trim (sline);
			if(*line) add_history(line);

			vector <string> fld;
			boost::split(fld, sline, boost::is_any_of("\t ,")); // split on tabs,spaces,commas
			if (fld.size() == 0) // empty line
				continue;

			if (sline.compare("exit") == 0) exit(0);
			if (sline.compare("quit") == 0) exit(0);

			// list available devices
			if (sline.compare("list") == 0)
			{
				cout << "available devices:" << endl;
				for (int i = 0; i < device_count; i++)
					if (fd[i] >= 0) printf ("index: %d name: %s\n", i, device_name[i].c_str());
			}

			// select devices or list selected devices
			if (sline.find("select") != string::npos)
			{

				if (fld.size() > 1) // command includes device indexes
				{
					if (fld[1].compare("all") == 0) // user wants all devices selected
					{
						memset(device_selected, 0xff, sizeof(device_selected)); // select all devices
					}
					else
					{
						memset(device_selected, 0, sizeof(device_selected)); // unselect all devices initially
						for (unsigned i = 1; i < fld.size(); i++) // scan remaining fields, these should be device indexes
						{
							int dev_ind = strtol (fld[i].c_str(), NULL, 10); // convert into integer
							if (dev_ind < 0 || dev_ind >= device_count)
							{
								cout << "invalid device index: " << fld[i] << endl;
							}
							else
							{
								device_selected[dev_ind] = true;
							}
						}
					}
				}

				cout << "selected devices:" << endl;
				for (int i = 0; i < device_count; i++)
					if (device_selected[i]) printf ("index: %d name: %s\n", i, device_name[i].c_str());

			}

			if (sline.compare("write_registers") == 0)
			{
				for (int i = 0; i < device_count; i++)
				{
					if (fd[i] >= 0 && device_selected[i]) 
					{
						printf ("index: %d name: %s write_registers\n", i, device_name[i].c_str());
                        chip.read_registers  (fd[i]);
                        // fill actual 32-bit register images with data according to parameters, not touching bits that were there before
                        chip.fill_registers  ();
                        chip.write_registers (fd[i]); // write register contents into device
                        chip.check_registers (fd[i]); // check all registers
					}
				}
			}

			if (sline.compare("reset") == 0)
			{
				for (int i = 0; i < device_count; i++)
				{
					if (fd[i] >= 0 && device_selected[i]) 
					{
						printf ("index: %d name: %s reset\n", i, device_name[i].c_str());
						chip.reset (fd[i]);
					}
				}
			}
		}

        if (argc > 2)
        {

            {
//                printf("opened device: %s\n", dev_name.str().c_str());

                if (string(argv[3]).compare("write_registers") == 0)
                {
                    // arguments: write_registers [x y common(0,1)]
                    int x = -1,y = -1,common = -1;
                    if (argc >= 7)
                    {
                        x      = strtol (argv[4], NULL, 10);
                        y      = strtol (argv[5], NULL, 10);
                        common = strtol (argv[6], NULL, 10);
                        drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));
                        if (common) uit = *(uit.common_unit);

                        uit.read_registers  (device_d);
                        // fill actual 32-bit register images with data according to parameters, not touching bits that were there before
                        chip.fill_registers  ();
                        uit.write_registers (device_d); // write register contents into device
                        uit.check_registers (device_d); // check all registers
                    }
                    else if (argc >= 5)
                    {

                        chip.fill_registers  ();
                        for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
                        {
                            //string uname = it->first;
                            drp_unit du = it->second;
//                            if (common) uit = *(uit.common_unit);

                            du.read_registers  (device_d);
                            // fill actual 32-bit register images with data according to parameters, not touching bits that were there before
                            du.write_registers (device_d); // write register contents into device
                            du.check_registers (device_d); // check all registers
                            du.common_unit->write_registers (device_d); // write register contents into device
                            du.common_unit->check_registers (device_d); // check all registers
                        }
                    }
                    else
                    {
						// this section ported
                        chip.read_registers  (device_d);
                        // fill actual 32-bit register images with data according to parameters, not touching bits that were there before
                        chip.fill_registers  ();
                        chip.write_registers (device_d); // write register contents into device
                        chip.check_registers (device_d); // check all registers
                    }
                }
                else if (string(argv[3]).compare("check_registers") == 0)
                {

                    // fill actual 32-bit register images with data according to parameters, not touching bits that were there before
                    chip.fill_registers  ();
                    chip.check_registers (device_d); // check all registers
                }
                else if (string(argv[3]).compare("check_atts") == 0)
                {

                    cout << "checking attributes" << endl;
                    // fill actual 32-bit register images with data according to parameters, not touching bits that were there before
                    chip.fill_registers ();
                    chip.read_registers (device_d);
                    chip.check_atts (); // check all attributes
                }
                else if (string(argv[3]).compare("reset") == 0)
                {
					// this section ported
                    chip.reset (device_d);
                }
                else if (string(argv[3]).compare("tx_phase_align") == 0)
                {
                    chip.tx_phase_align (device_d);
                }
                else if (string(argv[3]).compare("print") == 0)
                {
                    for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
                    {
                        //string uname = it->first;
                        drp_unit du = it->second;
                        printf ("x: %d y: %02d %-7s%02d txpll: %1s txa: %d   %-7s%02d rxpll: %1s rxa: %d"
                                " base: %05x com_base: %05x quad_adr: %08x pll_quad_adr: %08x\n",
                                du.x, du.y,
                                du.tx_group_name.c_str(), du.tx_group_index, du.tx_pll.c_str(), du.tx_active,
                                du.rx_group_name.c_str(), du.rx_group_index, du.rx_pll.c_str(), du.rx_active,
                                du.base_addr, du.common_unit->base_addr,
                                du.quad_address,
                                du.common_unit->quad_address);
                    }

                }
                else if (string(argv[3]).compare("test") == 0)
                {
                    //int i = 0x123456;
                    for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
                    {
                        //string uname = it->first;
                        drp_unit du = it->second;
                        cout << "          links: " << du.tx_group_name << du.tx_group_index << " "
                             << du.rx_group_name << du.rx_group_index << endl;
                        du.att_read_prn(device_d, "CPLLPD");
                        du.common_unit->att_read_prn(device_d, "QPLLPD");
                        du.att_read_prn(device_d, "TXPD");
                        du.att_read_prn(device_d, "RXPD");

//                        if (du.tx_group_index >= 0)
//                        {
//                            cout << "TX link: " << du.tx_group_name << du.tx_group_index << endl;
//                            i += 0x111111;
//                        }
                    }

                }
                else if (string(argv[3]).compare("read") == 0)
                {
                    // arguments: read x y common(0,1) register_name
                    int x      = strtol (argv[4], NULL, 10);
                    int y      = strtol (argv[5], NULL, 10);
                    int common = strtol (argv[6], NULL, 10);
                    string rname = argv[7];
                    // find MGT
                    drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));
                    if (common) uit = *(uit.common_unit);

                    uit.att_read_prn(device_d, rname);
                }
                else if (string(argv[3]).compare("read_registers") == 0)
                {
                    // arguments: read_registers x y common(0,1)
                    int x      = strtol (argv[4], NULL, 10);
                    int y      = strtol (argv[5], NULL, 10);
                    int common = strtol (argv[6], NULL, 10);
                    // find MGT
                    drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));
                    if (common) uit = *(uit.common_unit);

                    uit.read_registers_prn(device_d);
                }
                else if (string(argv[3]).compare("write") == 0)
                {
                    // arguments: write x y common(0,1) register_name value
                    int x      = strtol (argv[4], NULL, 10);
                    int y      = strtol (argv[5], NULL, 10);
                    int common = strtol (argv[6], NULL, 10);
                    string rname = argv[7];
                    boost::multiprecision::uint128_t value  = strtol (argv[8], NULL, 16);
                    // find MGT
                    drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));
                    if (common) uit = *(uit.common_unit);

                    uit.att_write(device_d, rname, value);
                }
                else if (string(argv[3]).compare("prbs_program") == 0)
                {
                    // program trasmitters first
                    // 1 = PRBS-7
                    // 4 = PRBS-31
                    int prbs_type = atoi(argv[4]);
                    for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
                    {
                        drp_unit du = it->second;
                        if (du.tx_group_index >= 0)
                        {
                            cout << "programming PRBS " << dec << du.tx_group_name << du.tx_group_index << endl;
                            du.att_write(device_d, "TXPRBSSEL", prbs_type);
                        }
                    }
                    // now receivers
                    for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
                    {
                        drp_unit du = it->second;
                        if (du.rx_group_index >= 0)
                        {
                            cout << "programming PRBS " << dec << du.rx_group_name << du.rx_group_index << endl;
                            du.att_write(device_d, "RXSLIDE_MODE", 0); // have to set RXSLIDE_MODE=OFF for PRBS test
                            du.att_write(device_d, "RXPRBSSEL", prbs_type);
                            usleep (10000); // give DFE time to train

                            // tell DFE to hold settings
//                            du.att_write(device_d, "RXDFEAGCHOLD", 1);
//                            du.att_write(device_d, "RXDFELFHOLD", 1);

                            // reset error counter
                            du.att_write(device_d, "RXPRBSCNTRESET", 1);
                            du.att_write(device_d, "RXPRBSCNTRESET", 0);
                        }
                    }
                }
                else if (string(argv[3]).compare("prbs_read") == 0)
                {
                    for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
                    {
                        //string uname = it->first;
                        drp_unit du = it->second;
                        if (du.rx_group_index >= 0)
                        {
                            printf ("%7s %02d ",du.rx_group_name.c_str(), du.rx_group_index);
                            du.att_read_prn(device_d, "RX_PRBS_ERR_CNT");
                        }
                    }
                }
                else if (string(argv[3]).compare("print_atts") == 0)
                {
                    // arguments: print_atts x y common(0,1)
                    int x      = strtol (argv[4], NULL, 10);
                    int y      = strtol (argv[5], NULL, 10);
                    int common = strtol (argv[6], NULL, 10);
                    // find MGT
                    drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));
                    if (common) uit = *(uit.common_unit);

                    uit.read_print_atts(device_d);
                }
                else if (string(argv[3]).compare("print_registers") == 0)
                {
                    // arguments: print_registers x y common(0,1)
                    int x      = strtol (argv[4], NULL, 10);
                    int y      = strtol (argv[5], NULL, 10);
                    int common = strtol (argv[6], NULL, 10);
                    // find MGT
                    drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));
                    if (common) uit = *(uit.common_unit);

                    printf ("printing register values for XY: %d %d\n", x, y);

                    for (map<int,register_prop>::iterator it = uit.registers.begin(); it != uit.registers.end(); ++it)
                    {
                        int offset = (it->first) & PORT_UNMARK;
                        register_prop reg = it->second;
                        uint32_t val = reg.value;
                        printf ("off: %04x value: %04x\n", offset, val);
                    }

                }
                else if (string(argv[3]).compare("drp") == 0)
                {
                    // arguments: drp x y
                    cout << "max_drp_addr_width: " << chip.max_drp_addr_width << endl;
                    cout << "mgts_per_quad: " << chip.mgts_per_quad << endl;
                    cout << "quad_port_addr0: " << hex << chip.quad_port_addr0 << endl;
                    cout << "xy_reg_addr: " << hex << chip.xy_reg_addr << endl;


                    int x      = strtol (argv[4], NULL, 10);
                    int y      = strtol (argv[5], NULL, 10);
                    // find MGT
                    drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));

                    for (int quad = 0; quad < 8; quad++)
                    {
                        // write quad selector
                        uit.reg_write(device_d, chip.quad_drp_addr0, 1 << quad);

                        for (int mgt = 0; mgt < 4; mgt++)
                        {
                            int xy_addr = (mgt << 9) + 0; // ACJTAG register
                            uint64_t rb = uit.reg_read (device_d, xy_addr);
                            //cout << hex << "quad: " << quad << " mgt: " << mgt << " acjtag: " << rb << endl;
                            printf ("quad: %d mgt: %d drp: %08lx\n", quad, mgt, rb);

                        }
                    }
                    for (int quad = 0; quad < 8; quad++)
                    {
                        // write quad selector
                        uit.reg_write(device_d, chip.quad_port_addr0, 1 << quad);

                        for (int mgt = 0; mgt < 4; mgt++)
                        {
                            int xy_addr = (mgt << 9) + 0; // ACJTAG register
                            uint64_t rb = uit.reg_read (device_d, xy_addr);
                            //cout << hex << "quad: " << quad << " mgt: " << mgt << " acjtag: " << rb << endl;
                            printf ("quad: %d mgt: %d port: %08lx\n", quad, mgt, rb);

                        }
                    }

                }
            }
        }
        else
        {
            cout << "chip.mgt_map size = " << chip.mgt_map.size() << endl;

        }

    }
    else
    {
        cout << "Use: " << argv[0] << " top_config_file" << endl;
    }

    return 0;
}
