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


#ifndef FPGA_H
#define FPGA_H
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>
#include <sys/sem.h>
#include <sys/stat.h>
#include "drp_unit.h"

#define MAX_DEVICES 100

using namespace std;

class fpga
{
private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
      ar& int_reg_offset    ;
      ar& reg_width_bits    ;
      ar& mgt_rx_iface          ;
      ar& mgt_tx_iface          ;
      ar& drp_iface             ;
      ar& interfaces_fname      ;
      ar& mgt_module_fn         ;
      ar& common_module_fn      ;
      ar& quad_module_fn        ;
      ar& mgt_port_offsets_fn   ;
      ar& common_port_offsets_fn;
      ar& mgt_drp_offsets_fn    ;
      ar& common_drp_offsets_fn ;
      ar& mgt_atts_fn           ;
      ar& common_atts_fn        ;
      ar& mgt_path              ;
      ar& board_path            ;
      ar& device_prefix         ;
      ar& device_count          ;
      ar& board_full_path;
      ar& root_path             ;
      ar& sv_path;

      ar& mgt_config_fn         ;
      ar& mgt_ports_fn          ;
      ar& common_ports_fn       ;
      ar& quad_connections_fn   ;
      ar& fpga_mgts_fn	      ;
      ar& refclk_buf_inst_fn  ;
      ar& reference_clocks_fn   ;
      ar& links_fn              ;
      ar& interfaces_fn         ;
      ar& mgt_inversions_fn;
      ar& base_addr_fn;
      ar& tx_mmcm_map_fn;

      ar& mgts_per_quad         ;
      ar& commons_per_quad      ;
      ar& max_drp_addr_width    ;
      ar& max_mgt_x             ;
      ar& max_mgt_y             ;
      ar& max_common_x          ;
      ar& max_common_y          ;
      ar& mgt_refclk_name     ;
      ar& common_refclk_name   ;
      ar& mgt_channel_name      ;
      ar& mgt_common_name       ;
      ar& mgt_quad_name         ;
      ar& mgt_txoutclk_name     ;
      ar& mgt_txusrclk_name     ;
      ar& mgt_rxoutclk_name     ;
      ar& mgt_rxusrclk_name     ;
      ar& mgt_txpolarity_name  ;
      ar& mgt_rxpolarity_name  ;
      ar& mgt_tx_powerdown_name;
      ar& mgt_rx_powerdown_name;
      ar& cppl_powerdown_name  ;
      ar& qpll_powerdown_name  ;

      ar& full_drp_addr_width;

      ar& quad_drp_addr1 ;
      ar& quad_drp_addr0 ;
      ar& quad_port_addr1;
      ar& quad_port_addr0;
      ar& fpga_in_out_addr;
      ar& mem_base;
      ar& drp_base;

      ar& xy_reg_addr;
      ar& fd_semid;

      ar& mgt_map;
      ar& com_map;
      ar& mgt_name_map;
      ar& com_name_map;
      ar& tx_mmcm_masters;
      ar& common_map;
    }

public:

    int int_reg_offset    ;
    int reg_width_bits    ;
    string mgt_rx_iface          ;
    string mgt_tx_iface          ;
    string drp_iface             ;
    string interfaces_fname      ;
    string mgt_module_fn         ;
    string common_module_fn      ;
    string quad_module_fn        ;
    string mgt_port_offsets_fn   ;
    string common_port_offsets_fn;
    string mgt_drp_offsets_fn    ;
    string common_drp_offsets_fn ;
    string mgt_atts_fn           ;
    string common_atts_fn        ;
    string mgt_path              ;
    string board_path            ;
    string device_prefix         ;
    int    device_count          ;
    string board_full_path;
    string root_path             ;
    string sv_path;

    string mgt_config_fn         ;
    string mgt_ports_fn          ;
    string common_ports_fn       ;
    string quad_connections_fn   ;
    string fpga_mgts_fn	      ;
    string refclk_buf_inst_fn  ;
    string reference_clocks_fn   ;
    string links_fn              ;
    string interfaces_fn         ;
    string mgt_inversions_fn;
    string base_addr_fn;
    string tx_mmcm_map_fn;

    int mgts_per_quad         ;
    int commons_per_quad      ;
    int max_drp_addr_width    ;
    int max_mgt_x             ;
    int max_mgt_y             ;
    int max_common_x          ;
    int max_common_y          ;
    string mgt_refclk_name     ;
    string common_refclk_name   ;
    string mgt_channel_name      ;
    string mgt_common_name       ;
    string mgt_quad_name         ;
    string mgt_txoutclk_name     ;
    string mgt_txusrclk_name     ;
    string mgt_rxoutclk_name     ;
    string mgt_rxusrclk_name     ;
    string mgt_txpolarity_name  ;
    string mgt_rxpolarity_name  ;
    string mgt_tx_powerdown_name;
    string mgt_rx_powerdown_name;
    string cppl_powerdown_name  ;
    string qpll_powerdown_name  ;

    int full_drp_addr_width; // full width of mgt/common address inside quad

    int quad_drp_addr1 ;
    int quad_drp_addr0 ;
    int quad_port_addr1;
    int quad_port_addr0;
    int fpga_in_out_addr;
    int64_t mem_base;
    int64_t drp_base;

    int xy_reg_addr;
    int fd_semid[MAX_DEVICES];

    void read_mgt_list ();
    void read_links ();
    void read_params ();
    void read_inversions ();
    void fill_registers ();
    void write_registers (int fd);
    void read_registers  (int fd);
    void check_registers (int fd);
    void check_atts ();
    void reset_v7_gth (int fd);
    void reset_usplus_gth (int fd);
    void reset_steps (int fd);
    void read_top_config(string fname);
    void read_mgt_config();
    void read_tx_mmcm_map();
    void tx_phase_align(int fd);
    int  mkxy (int x, int y) {return x*1000+y;} // make one integer out of X and Y, for maps
    int  create_semaphore (int dev_ind);
    int  lock_board(int fd);
    int  unlock_board(int fd);

    map<int,drp_unit> mgt_map; // XY coordinate to MGT
    map<int,drp_unit> com_map; // XY coordinate to COMMON
    map<string,int> mgt_name_map; // MGT name to XY
    map<string,int> com_name_map; // COM name to XY
    vector<drp_unit> tx_mmcm_masters; // map of txuserclk-sharing masters. Each master remembers its slaves
    map<int,drp_unit*>common_map; // common base address to common unit

};

#endif // FPGA_H
