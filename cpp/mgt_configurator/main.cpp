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
#include "../linenoise_cpp/linenoise.h"

#define MAX_DEVICE_COUNT 12

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
	for (int i = 0; i < device_count; i++)
	{
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			printf ("index: %d name: %s reset\n", i, device_name[i].c_str());
			chip.reset (fd[i]);
			if (chip.unlock_board (i) < 0) exit(-1);
		}
	}
}

void prbs_pattern (string cmd)
{
	vector <string> fld;
	boost::split(fld, cmd, boost::is_any_of("\t ,")); // split on tabs,spaces,commas
	// program trasmitters first
	int prbs_type = (strtol(fld[1].c_str(), NULL, 10) + 1)/8;
	for (int i = 0; i < device_count; i++)
	{
		if (fd[i] >= 0 && device_selected[i])
		{
			if (chip.lock_board (i) < 0) exit(-1);
			cout << "device: " << i << endl;
			for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
			{
				drp_unit du = it->second;
				if (du.tx_group_index >= 0)
				{
					cout << "programming TX PRBS " << dec << prbs_type << " " << du.tx_group_name << du.tx_group_index << endl;
					du.att_write(fd[i], "TXPRBSSEL", prbs_type);
				}
			}
			// now receivers
			for (map<int, drp_unit>::iterator it = chip.mgt_map.begin(); it != chip.mgt_map.end(); ++it)
			{
				drp_unit du = it->second;
				if (du.rx_group_index >= 0)
				{
					cout << "programming RX PRBS " << dec << prbs_type << " " << du.rx_group_name << du.rx_group_index << endl;
					du.att_write(fd[i], "RXSLIDE_MODE", 0); // have to set RXSLIDE_MODE=OFF for PRBS test
					du.att_write(fd[i], "RXPRBSSEL", prbs_type);
					usleep (10000); // give DFE time to train

					// reset error counter
					du.att_write(fd[i], "RXPRBSCNTRESET", 1);
					du.att_write(fd[i], "RXPRBSCNTRESET", 0);
				}
			}
			if (chip.unlock_board (i) < 0) exit(-1);
		}
	}
}

void prbs_read (string cmd)
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

string help_general = "commands: device register reset prbs exit";

using namespace std;
node_record nr[] =
{
	{0, "device",            "list|select",     NULL,           NULL},
	{1, 	"list",          "<Enter>",         device_select,  NULL},
	{1, 	"select",        "device list|all", NULL,           NULL},
	{2,			"([0-9,]+)", "<Enter>",         device_select,  NULL},
	{2,			"all",       "<Enter>",         device_select,  NULL},
	{0, "register",          "write|read",      NULL,           NULL},
	{1,     "write",         "all",             NULL,           NULL},
	{2,         "all",       "<Enter>",         register_write, NULL},
	{1,     "read",          "link X",          NULL,           NULL},
	{2,         "([0-1])",   "link Y",          NULL,           NULL},
	{3,         "([0-9]+)",  "common (1|0)",    NULL,           NULL},
	{4,         "([0-1])",   "reg name",        NULL,           NULL},
	{5,         "([A-Z0-9_]+)", "<Enter>",      register_read,  NULL},
	{0, "scan",              "link X",          NULL,           NULL},
	{1,     "([0-1])",       "link Y",          NULL,           NULL},
	{2,     "([0-9]+)",      "scale(6-15)",     NULL,           NULL},
        {3,     "([0-9]+)",      "mode:normal(0)|bathtub(1)",       NULL,       NULL},
        {4,     "([0-9]+)",      "<Enter>",         eyescan,        NULL},
	{0, "reset",             "<Enter>",         device_reset,   NULL},
	{0, "prbs",              "pattern|read",    NULL,           NULL},
	{1,     "(7|15|23|31)",  "<Enter>",         prbs_pattern,   NULL},
	{1,     "read",          "<Enter>",         prbs_read,      NULL},
	{0, "exit",              "<Enter>",         eject,          NULL},
	{-1,"help",              "<Enter>",         NULL,           &help_general} // end marker
};

int main(int argc, char *argv[])
{

    ostringstream dev_name;

    if (argc > 1)
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
            fd[i] = ::open(device_name[i].c_str(), O_RDWR);
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
