#include "fpga.h"

void fpga::read_mgt_list()
{
    ifstream file(base_addr_fn.c_str(), std::ifstream::in);
    if (!file.is_open())
    {
        cout << "cannot open file: " << base_addr_fn << endl;
        return;
    }
    string str;
    while (getline(file, str)) // read line by line
    {

        if (str.find("//") != string::npos) continue; // skip commented lines

//        cout << "fpga constructor reading line: " << str << endl;

        vector <string> fld;

        boost::split(fld, str, boost::is_any_of("\t")); // split on tabs
        if (fld.size() < 8) continue; // malformed line

//group   ind     x       y       mgt_bs  com_bs  MGT_path                protocol_path
//lnk_rx  0       1       39      03600   03800   xc7vx690tffg1927-2/GTH  MPC

        for (int bi = 0; bi < 8; bi++) boost::trim(fld[bi]);
        // create new drp_unit for MGT
        string grp_name  = fld[0];
        int    grp_index = strtol (fld[1].c_str(), NULL, 10);
        int    x         = strtol (fld[2].c_str(), NULL, 10);
        int    y         = strtol (fld[3].c_str(), NULL, 10);
        int    mgt_base  = strtol (fld[4].c_str(), NULL, 16);
        int    com_base  = strtol (fld[5].c_str(), NULL, 16);

        string mgt_path = fld[6]; // path to MGT settings
        string protocol_path = fld[7];

        // the fields below auto-generated
        string mgt_port_off           = "chips/" + mgt_path + "/mgt_port_offsets.tab";
        string com_port_off           = "chips/" + mgt_path + "/common_port_offsets.tab";
        string mgt_drp_off            = "chips/" + mgt_path + "/mgt_drp_map.tab";
        string com_drp_off            = "chips/" + mgt_path + "/common_drp_map.tab";
        string mgt_ports_atts_unit_fn = "chips/" + mgt_path + "/mgt_ports_atts_unit.tab";

        // create MGT unit
        drp_unit mgt_unit (mgt_base, full_drp_addr_width);
        mgt_unit.x = x;
        mgt_unit.y = y;

        mgt_unit.quad_drp_addr1  = quad_drp_addr1;
        mgt_unit.quad_drp_addr0  = quad_drp_addr0;
        mgt_unit.quad_port_addr1 = quad_port_addr1;
        mgt_unit.quad_port_addr0 = quad_port_addr0;

        int mgt_port_ln  = mgt_unit.read_config (mgt_port_off, false); // read port map
        int mgt_drp_ln   = mgt_unit.read_config (mgt_drp_off , true);  // read DRP map
        int mgt_unit_ln  = mgt_unit.read_units  (mgt_ports_atts_unit_fn); // apply separation of ports/registers into rx/tx units

        if (mgt_port_ln < 10) cerr << "problem reading " << mgt_port_off << endl;
        if (mgt_drp_ln  < 10) cerr << "problem reading " << mgt_drp_off  << endl;
        if (mgt_unit_ln < 10) cerr << "problem reading " << mgt_ports_atts_unit_fn  << endl;

        // add record to map
        ostringstream mgt_name;
        mgt_name << grp_name << grp_index; // create full unit name

        drp_unit *com_unit;
        // check if common for this quad already was created
        if (common_map.count(com_base) == 0)
        {
        // not yet
        // create COMMON unit as new so we can reference its pointer
            com_unit = new drp_unit(com_base, full_drp_addr_width);
            com_unit->common = true; // mark this unit as common
            com_unit->quad_drp_addr1  = quad_drp_addr1;
            com_unit->quad_drp_addr0  = quad_drp_addr0;
            com_unit->quad_port_addr1 = quad_port_addr1;
            com_unit->quad_port_addr0 = quad_port_addr0;
            int com_port_ln  = com_unit->read_config(com_port_off, false); // read port map
            int com_drp_ln   = com_unit->read_config(com_drp_off , true);  // read DRP map

            if (com_port_ln < 5) cerr << "problem reading " << com_port_off << endl;
            if (com_drp_ln  < 5) cerr << "problem reading " << com_drp_off  << endl;

            // store in map of commons
            common_map.insert(make_pair(com_base, com_unit));
//            printf("creating QPLL for: x: %d y: %d com_base: %x\n", x, y, com_base);
        }
        else
        {
            // get the existing common from map
            com_unit = common_map.at(com_base);
        }

        mgt_unit.common_unit = com_unit; // each MGT knows its own common unit

//        cout << "fpga constructor adding MGT: " << mgt_name.str() << endl;
        mgt_map.insert     (make_pair(mkxy(x,y), mgt_unit)); // store MGT unit in map after all parameters are set
        mgt_name_map.insert(make_pair(mgt_name.str(), mkxy(x,y))); // store MGT unit name and XY in a separate map
    }
//    cout << "MGT map size: " << mgt_map.size() << endl;
}

void fpga::read_links ()
{

    ifstream file(links_fn.c_str(), ifstream::in);
    string str;
    while (getline(file, str)) // read line by line
    {

        if (str.find("//") != string::npos) continue; // skip commented lines

        vector <string> fld;

        boost::split(fld, str, boost::is_any_of("\t")); // split on tabs
        if (fld.size() < 8) continue; // malformed line
// new format
//rx      1       39      MPC                     C       640     lnk_rx  0

        for (int bi = 0; bi < 8; bi++) boost::trim(fld[bi]);

        string dir = fld[0];
        int x = strtol (fld[1].c_str(), NULL, 10);
        int y = strtol (fld[2].c_str(), NULL, 10);
        string protocol = fld[3];
        string pll = fld[4];
        double refclk_f = strtod(fld[5].c_str(), NULL);
        string grp_name = fld[6];
        int    grp_index = strtol (fld[7].c_str(), NULL, 10);

//        ostringstream mgt_name;
//        mgt_name << grp_name << grp_index; // create full unit name

        // find the link in map
        drp_unit mgt_unit = mgt_map.at(mkxy(x,y)); // find unit

        // assign extra properties to link
        if (dir.compare("tx") == 0)
        {
            mgt_unit.tx_active = true;
            mgt_unit.tx_protocol_path = "protocols/" + protocol;
            mgt_unit.tx_pll      = pll;
            mgt_unit.tx_refclk_f = refclk_f;
            mgt_unit.tx_group_name = grp_name;
            mgt_unit.tx_group_index = grp_index;
        }
        if (dir.compare("rx") == 0)
        {
            mgt_unit.rx_active = true;
            mgt_unit.rx_protocol_path = "protocols/" + protocol;
            mgt_unit.rx_pll      = pll;
            mgt_unit.rx_refclk_f = refclk_f;
            mgt_unit.rx_group_name = grp_name;
            mgt_unit.rx_group_index = grp_index;
        }
        mgt_unit.x        = x;
        mgt_unit.y        = y;

        // refresh MGT unit in map after all parameters are set
        mgt_map.erase  (mkxy(x,y));
        mgt_map.insert (make_pair(mkxy(x,y), mgt_unit));
    }
}


void fpga::read_tx_mmcm_map()
{
    ifstream file(tx_mmcm_map_fn.c_str(), ifstream::in);
    string str;
    int master_xy = -1;
    drp_unit mu;
    while (getline(file, str)) // read line by line
    {

        if (str.find("//") != string::npos) continue; // skip commented lines

        vector <string> fld;

        boost::split(fld, str, boost::is_any_of("\t")); // split on tabs
        if (fld.size() < 3) continue; // malformed line
// format
//1       39      master
        for (int bi = 0; bi < 3; bi++) boost::trim(fld[bi]);

        int x = strtol (fld[0].c_str(), NULL, 10);
        int y = strtol (fld[1].c_str(), NULL, 10);
        string master = fld[2];
        if (master.compare("master") == 0)
        {
            if (master_xy >= 0)
            {
                // need to update previous master
                mgt_map.erase(master_xy);
                mgt_map.insert(make_pair(master_xy, mu));
                // add to list of masters
                tx_mmcm_masters.push_back(mu);
            }

            master_xy = mkxy(x,y); // new master unit's XY
            if (mgt_map.count(master_xy) > 0)
            {
                mu = mgt_map.at(master_xy); // extract master unit
            }
            else
                cout << "ERROR: cannot find tx clk sharing master: x: " << x << " y: " << y << endl;
        }
        else
        {
            // one of the slaves
            drp_unit su;
            int slave_xy = mkxy(x,y);
            if (mgt_map.count(slave_xy) > 0)
            {
                su = mgt_map.at(slave_xy); // extract slave unit
                mu.tx_mmcm_slaves.push_back(su); // add slave to master unit
            }
            else
                cout << "ERROR: cannot find tx clk sharing slave: x: " << x << " y: " << y << endl;


        }


//        cout << x << " " << y << " " << master << endl;
    }
    if (master_xy >= 0) // add last master unit to list
    {
        mgt_map.erase(master_xy);
        mgt_map.insert(make_pair(master_xy, mu));
        // add to list of masters
        tx_mmcm_masters.push_back(mu);
    }

    // scan masters
//    for (vector<drp_unit>::iterator it = tx_mmcm_masters.begin(); it != tx_mmcm_masters.end(); ++it )
//    {
//        drp_unit mu = *it;
//        cout << "master: " << mu.x << " " << mu.y << endl;
//        // scan slaves in each master
//        for (vector<drp_unit>::iterator sit = mu.tx_mmcm_slaves.begin(); sit != mu.tx_mmcm_slaves.end(); ++sit )
//        {
//            cout << "slave: " << sit->x << " " << sit->y << endl;
//        }
//    }
}

void fpga::tx_phase_align(int fd)
{
    // scan masters
    for (vector<drp_unit>::iterator it = tx_mmcm_masters.begin(); it != tx_mmcm_masters.end(); ++it )
    {
        drp_unit mu = *it;
        cout << "master: " << mu.x << " " << mu.y << endl;
        mu.tx_phase_align(fd);
    }
}

void fpga::read_params ()
{
    // scan all links
    map<int, drp_unit> mgt_map_r;
    for (map<int, drp_unit>::iterator it = mgt_map.begin(); it != mgt_map.end(); ++it)
    {
        int xy = it->first;
        drp_unit du = it->second;

        du.read_params(); // read parameters for MGT unit
        // each MGT also reads parameters for its COMMON unit

        mgt_map_r.insert(make_pair(xy, du)); // add elements to new map
        // cannot replace element in mgt_map, this breaks iterator

    }
    mgt_map = mgt_map_r; // store new map in the fpga
}

void fpga::fill_registers ()
{
    // scan all links
    map<int, drp_unit> mgt_map_r;
    for (map<int, drp_unit>::iterator it = mgt_map.begin(); it != mgt_map.end(); ++it)
    {
        int xy = it->first;
        drp_unit du = it->second;

//        cout << "                       fill mgt: " << dec << xy << " "
//             << du.tx_group_name << du.tx_group_index << " " << du.rx_group_name << du.rx_group_index << endl;

        du.fill_registers (); // fill registers in each MGT

        du.common_unit->fill_registers (); // and in COMMON unit
//        printf ("filling QPLL registers: %04d, reg size: %lu atts size: %lu\n",
//                xy, du.common_unit->registers.size(), du.common_unit->atts.size());

        mgt_map_r.insert(make_pair(xy, du)); // add elements to new map
        // cannot replace element in mgt_map, this breaks iterator

    }
    mgt_map = mgt_map_r; // store new map in the fpga

}

void fpga::write_registers (int fd)
{
    // scan all links
    for (map<int, drp_unit>::iterator it = mgt_map.begin(); it != mgt_map.end(); ++it)
    {
        //string uname = it->first;
        drp_unit du = it->second;

        // write quad selector
//        du.reg_write (fd, quad_drp_addr0, 1 << du.quad_address);

        du.write_registers (fd); // write registers in each MGT

        du.common_unit->write_registers (fd); // and in COMMON unit
    }
}

void fpga::read_registers (int fd)
{
    // scan all links
    map<int, drp_unit> mgt_map_r;
    for (map<int, drp_unit>::iterator it = mgt_map.begin(); it != mgt_map.end(); ++it)
    {
        int xy = it->first;
        drp_unit du = it->second;

        // write quad selector
//        du.reg_write (fd, quad_drp_addr0, 1 << du.quad_address);

        du.read_registers (fd); // check registers in each MGT

        du.common_unit->read_registers (fd); // and in COMMON unit
        mgt_map_r.insert(make_pair(xy, du)); // add elements to new map
        // cannot replace element in mgt_map, this breaks iterator
    }
    mgt_map = mgt_map_r; // store new map in the fpga
}

void fpga::check_registers (int fd)
{
    // scan all links
    for (map<int, drp_unit>::iterator it = mgt_map.begin(); it != mgt_map.end(); ++it)
    {
        //string uname = it->first;
        drp_unit du = it->second;

        // write quad selector
//        du.reg_write (fd, quad_drp_addr0, 1 << du.quad_address);

        du.check_registers (fd); // check registers in each MGT

        du.common_unit->check_registers (fd); // and in COMMON unit
    }
}

void fpga::check_atts ()
{
    // scan all links
    for (map<int, drp_unit>::iterator it = mgt_map.begin(); it != mgt_map.end(); ++it)
    {
        //string uname = it->first;
        drp_unit du = it->second;

        du.check_atts (); // check registers in each MGT

        du.common_unit->check_atts (); // and in COMMON unit
    }

}


void fpga::reset (int fd)
{

    // QPLL reset
    // scan common map
    for (map<int, drp_unit*>::iterator it = common_map.begin(); it != common_map.end(); ++it)
    {
        drp_unit* dup = it->second;
        dup->reset_qpll(fd);
    }

    // CPLL reset
    for (map<int, drp_unit>::iterator it = mgt_map.begin(); it != mgt_map.end(); ++it)
    {
        drp_unit du = it->second;
        du.reset_cpll (fd);
    }

    // TX reset
    for (map<int, drp_unit>::iterator it = mgt_map.begin(); it != mgt_map.end(); ++it)
    {
        drp_unit du = it->second;
        du.reset_tx (fd);
    }

    // RX reset
    for (map<int, drp_unit>::iterator it = mgt_map.begin(); it != mgt_map.end(); ++it)
    {
        drp_unit du = it->second;
        du.reset_rx (fd);
    }
}

void fpga::read_inversions()
{

    ifstream file(mgt_inversions_fn.c_str(), ifstream::in);
    string str;
    while (getline(file, str)) // read line by line
    {

        if (str.find("//") != string::npos) continue; // skip commented lines

        vector <string> fld;

        boost::split(fld, str, boost::is_any_of("\t")); // split on tabs
        if (fld.size() < 4) continue; // malformed line
// format
//x       y       tx_inv  rx_inv
//0       31      x       x

        for (int bi = 0; bi < 4; bi++) boost::trim(fld[bi]);

        int x = strtol (fld[0].c_str(), NULL, 10); // x coord
        int y = strtol (fld[1].c_str(), NULL, 10); // y coord
        bool tx_inv = !fld[2].empty(); // tx inversion if field is not empty
        bool rx_inv = !fld[3].empty(); // rx inversion if field is not empty

        // find the link in map
        if (mgt_map.count(mkxy(x,y)) != 0)
        {
            drp_unit du = mgt_map.at(mkxy(x,y));
            // assign inversions
//            cout << "INV: found MGT: " << x << " " << y << " "
//                 << du.tx_group_name << "+" << du.tx_group_index << " "
//                 << du.rx_group_name << "+" << du.rx_group_index
//                 << " " << tx_inv << " " << rx_inv << endl;

            // find and assign polarity ports in the map
            attribute rxpa = du.atts.at(mgt_rxpolarity_name);
            rxpa.value = rx_inv ? 1 : 0;
            rxpa.valid_value = true;
            du.atts.erase(mgt_rxpolarity_name);
            du.atts.insert (make_pair(mgt_rxpolarity_name, rxpa));

            attribute txpa = du.atts.at(mgt_txpolarity_name);
            txpa.value = tx_inv ? 1 : 0;
            txpa.valid_value = true;
            du.atts.erase(mgt_txpolarity_name);
            du.atts.insert (make_pair(mgt_txpolarity_name, txpa));

            // reinsert the unit
            mgt_map.erase  (mkxy(x,y));
            mgt_map.insert (make_pair(mkxy(x,y), du));
        }
    }
}

void fpga::read_top_config(string fname)
{
    ifstream file(fname.c_str(), std::ifstream::in);
    string str;
    while (getline(file, str)) // read line by line
    {

        if (str.find("//") != string::npos) continue; // skip commented lines

        vector <string> sf;

        boost::split(sf, str, boost::is_any_of("\t")); // split on tabs
        if (sf.size() < 2) continue; // malformed line

        boost::trim(sf[0]);
        boost::trim(sf[1]);

        if (sf[0].find("//") == string::npos && sf[0].length() > 0) // ignore commented and empty lines
        {
            if (sf[0].compare("int_reg_offset"        ) == 0)  int_reg_offset        = strtol (sf[1].c_str(), NULL, 10); else
            if (sf[0].compare("reg_width_bits"        ) == 0)  reg_width_bits        = strtol (sf[1].c_str(), NULL, 10); else
            if (sf[0].compare("mgt_rx_iface"          ) == 0)  mgt_rx_iface          = sf[1]; else
            if (sf[0].compare("mgt_tx_iface"          ) == 0)  mgt_tx_iface          = sf[1]; else
            if (sf[0].compare("drp_iface"             ) == 0)  drp_iface             = sf[1]; else
            if (sf[0].compare("interfaces_fname"      ) == 0)  interfaces_fname      = sf[1]; else
            if (sf[0].compare("mgt_module_fn"         ) == 0)  mgt_module_fn         = sf[1]; else
            if (sf[0].compare("common_module_fn"      ) == 0)  common_module_fn      = sf[1]; else
            if (sf[0].compare("quad_module_fn"        ) == 0)  quad_module_fn        = sf[1]; else
            if (sf[0].compare("mgt_port_offsets_fn"   ) == 0)  mgt_port_offsets_fn   = sf[1]; else
            if (sf[0].compare("common_port_offsets_fn") == 0)  common_port_offsets_fn= sf[1]; else
            if (sf[0].compare("mgt_drp_offsets_fn"    ) == 0)  mgt_drp_offsets_fn    = sf[1]; else
            if (sf[0].compare("common_drp_offsets_fn" ) == 0)  common_drp_offsets_fn = sf[1]; else
            if (sf[0].compare("mgt_atts_fn"           ) == 0)  mgt_atts_fn           = sf[1]; else
            if (sf[0].compare("common_atts_fn"        ) == 0)  common_atts_fn        = sf[1]; else
            if (sf[0].compare("MGT"                   ) == 0)  mgt_path              = sf[1]; else
            if (sf[0].compare("board"                 ) == 0)  board_path            = sf[1]; else
            if (sf[0].compare("root_config"           ) == 0)  {} else // unused
            if (sf[0].compare("phalg_tx_iface"        ) == 0)  {} else // unused
            {
                printf ("Unsupported parameter in top config file: %s = %s\n", sf[0].c_str(), sf[1].c_str());
            }
        }
    }


    root_path = "."; // always running from inside main config directory
    base_addr_fn      = "fpga_base_addr.tab";
    // create file names from MGT and board paths
    string mgt_full_path = root_path + "/chips/" + mgt_path + "/";

    mgt_config_fn           = mgt_full_path + "mgt_config.tab";
    mgt_ports_fn            = mgt_full_path + "mgt_ports.tab";
    common_ports_fn         = mgt_full_path + "common_ports.tab";
    quad_connections_fn     = mgt_full_path + "quad_connections.tab";
    fpga_mgts_fn	        = mgt_full_path + "mgt_placement.tab";
    refclk_buf_inst_fn      = mgt_full_path + "refclk_buf_inst.sv";
    mgt_port_offsets_fn     = mgt_full_path + "mgt_port_offsets.tab";
    common_port_offsets_fn  = mgt_full_path + "common_port_offsets.tab";

    board_full_path = root_path + "/boards/" + board_path + "/";

    reference_clocks_fn  = board_full_path + "reference_clocks.tab";
    links_fn             = board_full_path + "links.tab";
    mgt_inversions_fn    = board_full_path + "/mgt_inversions.tab";
    tx_mmcm_map_fn       = board_full_path + "/tx_mmcm_map.tab";

    // add path for generated sources
    sv_path = root_path + "/sv/";

    quad_module_fn   = sv_path + quad_module_fn;
    common_module_fn = sv_path + common_module_fn;
    mgt_module_fn    = sv_path + mgt_module_fn;
    interfaces_fn    = sv_path + interfaces_fname; // need to keep name without path for include statements
}

void fpga::read_mgt_config()
{
    ifstream file(mgt_config_fn.c_str(), std::ifstream::in);
    string str;
    while (getline(file, str)) // read line by line
    {

        if (str.find("//") != string::npos) continue; // skip commented lines

        vector <string> sf;

        boost::split(sf, str, boost::is_any_of("\t")); // split on tabs
        if (sf.size() < 2) continue; // malformed line

        boost::trim(sf[0]);
        boost::trim(sf[1]);

        if (sf[0].find("//") == string::npos && sf[0].length() > 0) // ignore commented and empty lines
        {
            if (sf[0].compare("mgts_per_quad"         ) == 0) mgts_per_quad          = strtol (sf[1].c_str(), NULL, 10); else
            if (sf[0].compare("commons_per_quad"      ) == 0) commons_per_quad       = strtol (sf[1].c_str(), NULL, 10); else
            if (sf[0].compare("max_drp_addr_width"    ) == 0) max_drp_addr_width     = strtol (sf[1].c_str(), NULL, 10); else
            if (sf[0].compare("max_mgt_x"             ) == 0) max_mgt_x              = strtol (sf[1].c_str(), NULL, 10); else
            if (sf[0].compare("max_mgt_y"             ) == 0) max_mgt_y              = strtol (sf[1].c_str(), NULL, 10); else
            if (sf[0].compare("max_common_x"          ) == 0) max_common_x           = strtol (sf[1].c_str(), NULL, 10); else
            if (sf[0].compare("max_common_y"          ) == 0) max_common_y           = strtol (sf[1].c_str(), NULL, 10); else
            if (sf[0].compare("mgt_refclk_name"       ) == 0) mgt_refclk_name        = sf[1]; else
            if (sf[0].compare("common_refclk_name"    ) == 0) common_refclk_name     = sf[1]; else
            if (sf[0].compare("mgt_channel_name"      ) == 0) mgt_channel_name       = sf[1]; else
            if (sf[0].compare("mgt_common_name"       ) == 0) mgt_common_name        = sf[1]; else
            if (sf[0].compare("mgt_quad_name"         ) == 0) mgt_quad_name          = sf[1]; else
            if (sf[0].compare("mgt_txoutclk_name"     ) == 0) mgt_txoutclk_name      = sf[1]; else
            if (sf[0].compare("mgt_txusrclk_name"     ) == 0) mgt_txusrclk_name      = sf[1]; else
            if (sf[0].compare("mgt_rxoutclk_name"     ) == 0) mgt_rxoutclk_name      = sf[1]; else
            if (sf[0].compare("mgt_rxusrclk_name"     ) == 0) mgt_rxusrclk_name      = sf[1]; else
            if (sf[0].compare("mgt_txpolarity_name"   ) == 0) mgt_txpolarity_name    = sf[1]; else
            if (sf[0].compare("mgt_rxpolarity_name"   ) == 0) mgt_rxpolarity_name    = sf[1]; else
            if (sf[0].compare("mgt_tx_powerdown_name" ) == 0) mgt_tx_powerdown_name  = sf[1]; else
            if (sf[0].compare("mgt_rx_powerdown_name" ) == 0) mgt_rx_powerdown_name  = sf[1]; else
            if (sf[0].compare("cppl_powerdown_name"   ) == 0) cppl_powerdown_name    = sf[1]; else
            if (sf[0].compare("qpll_powerdown_name"   ) == 0) qpll_powerdown_name    = sf[1]; else
            {
                printf ("Unsupported parameter in MGT config file: %s = %s\n", sf[0].c_str(), sf[1].c_str());
            }
        }
    }

    // how many bits used for addressing mgts and commons in a quad
    int mgts_per_quad_bits = 0;
    switch (mgts_per_quad + commons_per_quad)
    {
        case 5: mgts_per_quad_bits = 3; break;
        case 3: mgts_per_quad_bits = 2; break;
        default: cout << "ERROR: unusual count of MGTs per quad : " << mgts_per_quad << endl;
    }
    // total count of bits for addressing any register/port in any of the MGTs/COMMON in one quad
    full_drp_addr_width = mgts_per_quad_bits + max_drp_addr_width;

    // addresses of quad selection registers
    quad_port_addr0  = (1 << full_drp_addr_width) - 1;
    quad_port_addr1  = (1 << full_drp_addr_width) - 2;
    quad_drp_addr0   = (1 << full_drp_addr_width) - 3;
    quad_drp_addr1   = (1 << full_drp_addr_width) - 4;
    fpga_in_out_addr = (1 << full_drp_addr_width) - 5;

    // register in each drp_unit that returns its own XY location
    // has to be read as port
    xy_reg_addr = (1 << max_drp_addr_width) - 1;

}

