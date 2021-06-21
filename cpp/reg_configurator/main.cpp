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
#include <readline/readline.h>
#include <readline/history.h>
#include "../../../linenoise_cpp/linenoise.h"

using namespace std;

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

#define MAX_DEVICE_COUNT 100

int fd[MAX_DEVICE_COUNT];
string device_name[MAX_DEVICE_COUNT];
bool device_selected[MAX_DEVICE_COUNT];
int device_count = 12;

int base = 0; // base address of register bank, bytes
drp_unit bank (base);

void eject (string cmd)
{
    exit (0);
}

void device_print (string cmd)
{
    for (int i = 0; i < 12; i++)
        cout << "i: " << i << " dev: " << hex << fd[i] << endl;
}

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
    vector <string> argv;
    boost::split(argv, cmd, boost::is_any_of("\t ")); // split on tabs,spaces

    string rname = argv[2];
    uint64_t wr_data  = strtoull (argv[3].c_str(), NULL, 16);
    boost::multiprecision::uint128_t value = wr_data;

    for (int i = 0; i < device_count; i++)
    {
        if (fd[i] >= 0 && device_selected[i])
        {
            bank.att_write (fd[i], rname, value);
        }
    }
}


void register_read (string cmd)
{
    vector <string> argv;
    boost::split(argv, cmd, boost::is_any_of("\t ")); // split on tabs,spaces

    string rname = argv[2];

    for (int i = 0; i < device_count; i++)
    {
        if (fd[i] >= 0 && device_selected[i])
        {
            bank.att_read_prn(fd[i], rname);
        }
    }
}

string help_general = "Top level commands: device register exit";

node_record nr[] =
{
    {0, "device",            "list|select|print", NULL,           NULL},
    {1, 	"list",          "<Enter>",         device_select,  NULL},
    {1, 	"print",         "<Enter>",         device_print,   NULL},
    {1, 	"select",        "device list|all", NULL,           NULL},
    {2,			"([0-9,]+)", "<Enter>",         device_select,  NULL},
    {2,			"all",       "<Enter>",         device_select,  NULL},
    {0, "register",          "write|read",      NULL,           NULL},
    {1,     "write",         "reg_name",        NULL,           NULL},
    {2,         "([A-Z0-9_]+)", "value",        NULL,           NULL},
    {3,         "([xXA-Fa-f0-9]+)", "<Enter>",  register_write, NULL},
    {1,     "read",          "reg_name",        NULL,           NULL},
    {2,         "([A-Z0-9_]+)", "<Enter>",      register_read,  NULL},
    {0, "exit",              "<Enter>",         eject,          NULL},
    {-1,"help",              "<Enter>",         NULL,           &help_general} // end marker
};

int main(int argc, char *argv[])
{

    // argv[1] is top config file name

    ostringstream dev_name;
    string device_prefix = "/dev/utca_sp12";

    string register_offsets = "emtf_pcie_registers.tab";
    int mgt_port_ln  = bank.read_config_emtf (register_offsets, false); // read port map
    cout << "read " << mgt_port_ln << " lines from file: " << register_offsets << endl;

//    bank.read_params     (); // registers' settings. File names are created automatically
    bank.fill_registers  (); // using this to only detect which registers are there, and fill map with offsets

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
    }

    char* buf;
    string bline;

    linenoise ln(nr, "regc_history.txt");

    while((buf = ln.prompt("regc> ")) != NULL)
    {
        bline = (string) buf;
        int ei = ln.get_enter_index();

        if (ei >= 0 && nr[ei].cb != NULL) nr[ei].cb(bline);

        if (ei < 0) printf("%s", ln.get_help_message().c_str());

        free (buf);
    }

/*
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
*/
    return 0;
}
