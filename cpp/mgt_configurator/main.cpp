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

#include <iostream>
#include <fcntl.h>
#include "drp_unit.h"
#include "fpga.h"
#include <readline/readline.h>
#include <readline/history.h>
#include "../linenoise_cpp/linenoise.h"

// max count of devices in the system
// the actual device count is set in the top-level config
#define MAX_DEVICE_COUNT 100

// section for AXI devices only
#ifdef AXI
	// sys_vptr is byte pointer
	uint8_t *sys_vptr;
	int sys_fd;
	int open_dev_mem ()
	{
		sys_fd = ::open("/dev/mem", O_RDWR | O_SYNC);
		if (sys_fd != -1)
		sys_vptr = (uint8_t *)mmap(NULL, DRP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, sys_fd, DRP_BASE);
		else sys_vptr = NULL;
		return sys_fd;
	}
#endif
// end section for AXI devices

int fd[MAX_DEVICE_COUNT];
string device_name[MAX_DEVICE_COUNT];
bool device_selected[MAX_DEVICE_COUNT];
int device_count;
fpga chip;

void eject (string cmd)
{
	exit (0);
};

void device_select (string cmd)
{
	vector <string> fld;
	boost::split(fld, cmd, boost::is_any_of("\t ,")); // split on tabs,spaces,commas

	if (fld[1].compare("list") == 0)
	{
		// do nothing here, devices are listed at the end
	}
	else
	if (fld[2].compare("all") == 0) // user wants all devices selected
	{
		memset(device_selected, 0, sizeof(device_selected)); // unselect all devices initially
		for (int i = 0; i < device_count; i++)
		{
			if (fd[i] >= 0) // enable all devices that were opened
				device_selected[i] = true;
		}
	}
	else
	{
		memset(device_selected, 0, sizeof(device_selected)); // unselect all devices initially
		for (unsigned i = 2; i < fld.size(); i++) // scan remaining fields, these should be device indexes
		{
			int dev_ind = strtol (fld[i].c_str(), NULL, 10); // convert into integer
			if (dev_ind < 0 || dev_ind >= device_count || fd[dev_ind] < 0)
				cout << "invalid device index: " << fld[i] << endl;
			else
				device_selected[dev_ind] = true;
		}
	}

	cout << "selected devices:" << endl;
	for (int i = 0; i < device_count; i++)
		if (device_selected[i]) printf ("index: %d name: %s\n", i, device_name[i].c_str());
}

void register_write (string cmd)
{
	for (int i = 0; i < device_count; i++)
	{
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			printf ("index: %d name: %s writing registers\n", i, device_name[i].c_str());
			chip.read_registers  (fd[i]);
			// fill actual 32-bit register images with data according to parameters, not touching bits that were there before
			chip.fill_registers  ();
			chip.write_registers (fd[i]); // write register contents into device
			chip.check_registers (fd[i]); // check all registers
			if (chip.unlock_board (i) < 0) exit(-1);
		}
	}
}

void register_wr1 (string cmd)
{
    vector <string> argv;
	boost::split(argv, cmd, boost::is_any_of("\t ")); // split on tabs,spaces

	int x        = strtol (argv[2].c_str(), NULL, 10);
	int y        = strtol (argv[3].c_str(), NULL, 10);
	int common   = strtol (argv[4].c_str(), NULL, 10);
	string rname = argv[5];
	int wr_data  = strtol (argv[6].c_str(), NULL, 16);


	for (int i = 0; i < device_count; i++)
	{
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			// find MGT
			drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));
			if (common) uit = *(uit.common_unit);

			uit.att_write(fd[i], rname, wr_data);
			if (chip.unlock_board (i) < 0) exit(-1);
		}
	}
}

void register_read (string cmd)
{
    vector <string> argv;
	boost::split(argv, cmd, boost::is_any_of("\t ")); // split on tabs,spaces

	int x      = strtol (argv[2].c_str(), NULL, 10);
	int y      = strtol (argv[3].c_str(), NULL, 10);
	int common = strtol (argv[4].c_str(), NULL, 10);
	string rname = argv[5];

	for (int i = 0; i < device_count; i++)
	{
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			// find MGT
			drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));
			if (common) uit = *(uit.common_unit);

			uit.att_read_prn(fd[i], rname);
			if (chip.unlock_board (i) < 0) exit(-1);
		}
	}
}

void eyescan (string cmd)
{
  vector <string> argv;
	boost::split(argv, cmd, boost::is_any_of("\t ")); // split on tabs,spaces

	int x      = strtol (argv[1].c_str(), NULL, 10);
	int y      = strtol (argv[2].c_str(), NULL, 10);
	int scale  = strtol (argv[3].c_str(), NULL, 10);
        int mode   = strtol (argv[4].c_str(), NULL, 10);

	for (int i = 0; i < device_count; i++)
	{
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			// find MGT
			drp_unit uit = chip.mgt_map.at(chip.mkxy(x,y));
			//if (common) uit = *(uit.common_unit);

			uit.eyescan_complete(fd[i], x, y, scale, i, mode);
			if (chip.unlock_board (i) < 0) exit(-1);
		}
	}
}

void device_reset (string cmd)
{
    vector <string> argv;
	boost::split(argv, cmd, boost::is_any_of("\t ")); // split on tabs,spaces

	if (argv.size() < 2)
	{
		cout << "please specify device family" << endl;
		return;
	}

	bool v7_gth = false, usplus_gth = false;
	if (argv[1].compare("v7_gth") == 0) v7_gth = true;
	if (argv[1].compare("usplus") == 0) usplus_gth = true;

	if (!v7_gth && !usplus_gth)
	{
		cout << "please specify supported device family" << endl;
		return;
	}

	for (int i = 0; i < device_count; i++)
	{
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			printf ("index: %d name: %s reset\n", i, device_name[i].c_str());
			if (v7_gth)     chip.reset_v7_gth     (fd[i]);
			if (usplus_gth) chip.reset_usplus_gth (fd[i]);
			if (chip.unlock_board (i) < 0) exit(-1);
		}
	}
}

void prbs_pattern (string cmd)
{
	vector <string> fld;
	boost::split(fld, cmd, boost::is_any_of("\t ,")); // split on tabs,spaces,commas
	int prbs_type = -1;
	// program trasmitters first
	if (fld[1].compare("reset") == 0)
	{
		prbs_type = -1;
	}
	else
	{
		prbs_type = (strtol(fld[1].c_str(), NULL, 10) + 1)/8;
	}
	for (int i = 0; i < device_count; i++)
	{
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			cout << "device: " << i << endl;
			if (prbs_type >= 0) // not reset, need to program transmitters
			{
			for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
			{
				drp_unit du = it->second;
				if (du.tx_group_index >= 0)
				{
					cout << "programming TX PRBS " << dec << prbs_type << " " << du.tx_group_name << du.tx_group_index << endl;
					du.att_write(fd[i], "TXPRBSSEL", prbs_type);
				}
			}
			}
			// now receivers
			for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
			{
				drp_unit du = it->second;
				if (du.rx_group_index >= 0)
				{
					if (prbs_type >= 0) // not reset
					{
					cout << "programming RX PRBS " << dec << prbs_type << " " << du.rx_group_name << du.rx_group_index << endl;
					du.att_write(fd[i], "RXSLIDE_MODE", 0); // have to set RXSLIDE_MODE=OFF for PRBS test
					du.att_write(fd[i], "RXPRBSSEL", prbs_type);
					usleep (10000); // give DFE time to train
					}
					// reset error counter
					du.att_write(fd[i], "RXPRBSCNTRESET", 1);
					du.att_write(fd[i], "RXPRBSCNTRESET", 0);
				}
			}
			if (chip.unlock_board (i) < 0) exit(-1);
		}
	}
}

void prbs_read_v7 (string cmd)
{
	for (int i = 0; i < device_count; i++)
	{
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			cout << "device: " << i << endl;
			for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
			{
				//string uname = it->first;
				drp_unit du = it->second;
				if (du.rx_group_index >= 0)
				{
					printf ("%7s %02d ",du.rx_group_name.c_str(), du.rx_group_index);
					du.att_read_prn(fd[i], "RX_PRBS_ERR_CNT");
				}
			}
			if (chip.unlock_board (i) < 0) exit(-1);
		}
	}
}

void prbs_read_us (string cmd)
{
	for (int i = 0; i < device_count; i++)
	{
		int locked_count = 0;
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			cout << "device: " << i << endl;
			for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
			{
				//string uname = it->first;
				drp_unit du = it->second;
				if (du.rx_group_index >= 0)
				{
					// check if link locked first
    					string svalue;
			    		boost::multiprecision::uint128_t v  = du.att_read(fd[i], "RXPRBSLOCKED", svalue);
					if (v == 1) // if locked
					{
			    			boost::multiprecision::uint128_t v  = du.att_read(fd[i], "RX_PRBS_ERR_CNT", svalue);
						if (v > 0) // print only if errors
						{
							printf ("%7s %02d ",du.rx_group_name.c_str(), du.rx_group_index);
							// du.att_read_prn(fd[i], "RXPRBSLOCKED");
	    						cout << svalue << endl;
						}
						locked_count ++;
					}
					else // RPBS unlocked
					{
						printf ("%7s %02d -- link NOT LOCKED\n",du.rx_group_name.c_str(), du.rx_group_index);
					}
				}
			}
			if (chip.unlock_board (i) < 0) exit(-1);
		}
		printf ("PRBS locked link count: %d\n", locked_count);
	}

}


string help_general = "Top level commands: device register reset prbs scan exit\nType one of them, then follow hints.";

using namespace std;
node_record nr[] =
{
	{0, "device",            "list|select",     NULL,           NULL},
	{1, 	"list",          "<Enter>",         device_select,  NULL},
	{1, 	"select",        "device list|all", NULL,           NULL},
	{2,			"([0-9,]+)", "<Enter>",         device_select,  NULL},
	{2,			"all",       "<Enter>",         device_select,  NULL},
	{0, "register",          "write|read",      NULL,           NULL},
	{1,     "write",         "all|linkX",       NULL,           NULL},
	{2,         "all",       "<Enter>",         register_write, NULL},
	{2,         "([0-1])",   "link Y",          NULL,           NULL},
	{3,         "([0-9]+)",  "common (1|0)",    NULL,           NULL},
	{4,         "([0-1])",   "reg name",        NULL,           NULL},
	{5,         "([A-Z0-9_]+)", "value",        NULL,           NULL},
	{6,         "([xXA-Fa-f0-9]+)", "<Enter>",  register_wr1,   NULL},
	{1,     "read",          "link X",          NULL,           NULL},
	{2,         "([0-1])",   "link Y",          NULL,           NULL},
	{3,         "([0-9]+)",  "common (1|0)",    NULL,           NULL},
	{4,         "([0-1])",   "reg name",        NULL,           NULL},
	{5,         "([A-Z0-9_]+)", "<Enter>",      register_read,  NULL},
	{0, "scan",              "link X",          NULL,           NULL},
	{1,     "([0-1])",       "link Y",          NULL,           NULL},
	{2,     "([0-9]+)",      "scale(6-15)",     NULL,           NULL},
  	{3,     "([0-9]+)",      "mode:normal-2d(0)|bathtub(1)",    NULL,       NULL},
  	{4,     "([0-9]+)",      "<Enter>",         eyescan,        NULL},
	{0, "reset",             "v7_gth | usplus", NULL,           NULL},
	{1,     "v7_gth",        "<Enter>",         device_reset,   NULL},
	{1,     "usplus",        "<Enter>",         device_reset,   NULL},
	{0, "prbs",              "pattern|read|reset",  NULL,       NULL},
	{1,     "(7|15|23|31)",  "<Enter>",         prbs_pattern,   NULL},
	{1,     "reset",         "<Enter>",         prbs_pattern,   NULL},
	{1,     "read",          "v7_gth | usplus", NULL,           NULL},
	{2,       "v7_gth",      "<Enter>",         prbs_read_v7,   NULL},
	{2,       "usplus",      "<Enter>",         prbs_read_us,   NULL},
	{0, "exit",              "<Enter>",         eject,          NULL},
	{-1,"help",              "<Enter>",         NULL,           &help_general} // end marker
};

int main(int argc, char *argv[])
{

    ostringstream dev_name;

    if (argc > 1)
    {
		if (!boost::filesystem::exists("/tmp/mgt_serialize.dat"))
		{
			// argv[1] is top config file name
			chip.read_top_config (argv[1]); // top configuration file
			chip.read_mgt_config (); // read MGT configuration
			chip.read_mgt_list   (); // fpga link base addresses
			chip.read_links      (); // link config file, same as what java script reads
			chip.read_params     (); // DRP registers' and port settings for each link, according to protocol. File names are created automatically
			chip.read_inversions (); // read inversion flags for each link
			chip.fill_registers  (); // using this to only detect which registers are there, and fill map with offsets
			chip.read_tx_mmcm_map(); // read map of TX clock-sharing MMCMs

			std::ofstream ofs("/tmp/mgt_serialize.dat", std::ios::binary);
			boost::archive::binary_oarchive oa(ofs);
			oa << chip;
			ofs.close();
			chmod("/tmp/mgt_serialize.dat", 0666);
		}
		else
		{
			std::ifstream ifs("/tmp/mgt_serialize.dat", std::ios::binary);
			boost::archive::binary_iarchive ia(ifs);
			ia >> chip;
			ifs.close();
		}

		// open devices
		string device_prefix = chip.device_prefix;
		device_count = chip.device_count;
		memset(device_selected, 0, sizeof(device_selected)); // unselect all devices initially

		for (int i = 0; i < device_count; i++)
		{
 			dev_name.str("");
 			dev_name.clear();
 			dev_name << device_prefix << i;
			device_name[i] = dev_name.str().c_str();
            		// open device
            		//fd[i] = ::open(device_name[i].c_str(), O_RDWR);
            		fd[i] = mopen(device_name[i].c_str()); // mopen macro depends on the system
            		if (fd[i] < 0)
            		{
                		printf("ERROR: Can not open device file: %s\n", device_name[i].c_str());
            		}
			else
			{
				// create semaphore for this board
				if (chip.create_semaphore(i) < 0) exit(-1);
			}
		}

		char* buf;
		string bline;

		linenoise ln(nr, "mgtb_history.txt");

		while((buf = ln.prompt("mgtc> ")) != NULL)
		{
			bline = (string) buf;
			int ei = ln.get_enter_index();

			if (ei >= 0 && nr[ei].cb != NULL) nr[ei].cb(bline);

        	if (ei < 0) printf("%s", ln.get_help_message().c_str());

			free (buf);
		}
    }
    else
    {
        cout << "Use: " << argv[0] << " top_config_file" << endl;
    }

    return 0;
}
