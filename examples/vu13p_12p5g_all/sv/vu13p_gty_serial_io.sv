// This code was automatically generated by FPGA MGT Builder
// Timestamp: 2022-09-15 16:14:34.006
// User: madorsky
// FPGA MGT builder homepage: <link>

`include "drp_interface.sv"
`include "mgt_gty_interfaces.sv"
module vu13p_gty_serial_io
(
	drp.in drp_gty_fif,
	input drpclk,
	input [31:0] refclk_p,
	input [31:0] refclk_n,
	mgt_gty_rx.in p25g_rx [125:0],
	mgt_gty_tx.in p25g_tx [125:0],
	output pr25g_13_mmcm_clk,
	output pr25g_12_mmcm_clk,
	output pr25g_11_mmcm_clk,
	output pr25g_10_mmcm_clk,
	output pr25g_31_mmcm_clk,
	output pr25g_30_mmcm_clk,
	output pr25g_29_mmcm_clk,
	output pr25g_28_mmcm_clk,
	output pr25g_27_mmcm_clk,
	output pr25g_26_mmcm_clk,
	output pr25g_25_mmcm_clk,
	output pr25g_9_mmcm_clk,
	output pr25g_5_mmcm_clk,
	output pr25g_6_mmcm_clk,
	output pr25g_7_mmcm_clk,
	output pr25g_8_mmcm_clk,
	output pr25g_1_mmcm_clk,
	output pr25g_24_mmcm_clk,
	output pr25g_2_mmcm_clk,
	output pr25g_23_mmcm_clk,
	output pr25g_3_mmcm_clk,
	output pr25g_22_mmcm_clk,
	output pr25g_4_mmcm_clk,
	output pr25g_21_mmcm_clk,
	output pr25g_20_mmcm_clk,
	output pr25g_0_mmcm_clk,
	output pr25g_19_mmcm_clk,
	output pr25g_18_mmcm_clk,
	output pr25g_17_mmcm_clk,
	output pr25g_16_mmcm_clk,
	output pr25g_15_mmcm_clk,
	output pr25g_14_mmcm_clk
    output [31:0] refclk,
    output [31:0] refclk_odiv
);
	localparam quadn = 32;
	drp #(.AW(13)) drp_gty_qif [quadn-1:0]();
	drp_mux_fpga #(.N(quadn), .AW_QUAD(13)) drpm (drp_gty_qif, drp_gty_fif, drpclk);

	mgt_gty_tx mgt_gty_tx_if [quadn-1:0][3:0]();
	mgt_gty_rx mgt_gty_rx_if [quadn-1:0][3:0]();
	wire        mgt_txlatclk ;
	wire [ 3:0] mgt_txusrclk [quadn-1:0];
	wire        com_gtrefclk00 [quadn-1:0];
	wire        com_gtrefclk01 [quadn-1:0];
	genvar gi;
	generate
		for (gi = 0; gi < quadn; gi++)
		begin: quad_loop
			GTYE4_QUAD #(.ind(gi)) GTYE4_QUAD_inst
			(
				.drp_gty_qif (drp_gty_qif [gi]),
				.drpclk (drpclk),
				.mgt_gty_tx_if (mgt_gty_tx_if [gi]),
				.mgt_gty_rx_if (mgt_gty_rx_if [gi]),
				.mgt_txlatclk               (mgt_txlatclk              ),
				.mgt_txusrclk               (mgt_txusrclk               [gi]),
				.com_gtrefclk00             (com_gtrefclk00             [gi]),
				.com_gtrefclk01             (com_gtrefclk01             [gi])
			);
		end
		for (gi = 0; gi < 32; gi++)
		begin: refclk_loop
			IBUFDS_GTE4 #
			(
				.REFCLK_EN_TX_PATH  (1'b0),
				.REFCLK_HROW_CK_SEL (2'b00),
				.REFCLK_ICNTL_RX    (2'b00)
			) 
			refclk_buf
			(
				.I     (refclk_p [gi]),
				.IB    (refclk_n [gi]),
				.CEB   (1'b0),
				.O     (refclk   [gi]),
				.ODIV2 (refclk_odiv[gi])
			);
		end
	endgenerate
	assign_mgt_gty_rx ari0 (mgt_gty_rx_if [0][1], p25g_rx [0]);
	assign_mgt_gty_tx ati0 (mgt_gty_tx_if [0][1], p25g_tx [0]);
	assign_mgt_gty_rx ari1 (mgt_gty_rx_if [0][2], p25g_rx [1]);
	assign_mgt_gty_tx ati1 (mgt_gty_tx_if [0][2], p25g_tx [1]);
	assign_mgt_gty_rx ari2 (mgt_gty_rx_if [0][3], p25g_rx [2]);
	assign_mgt_gty_tx ati2 (mgt_gty_tx_if [0][3], p25g_tx [2]);
	assign_mgt_gty_rx ari3 (mgt_gty_rx_if [1][1], p25g_rx [3]);
	assign_mgt_gty_tx ati3 (mgt_gty_tx_if [1][1], p25g_tx [3]);
	assign_mgt_gty_rx ari4 (mgt_gty_rx_if [1][2], p25g_rx [4]);
	assign_mgt_gty_tx ati4 (mgt_gty_tx_if [1][2], p25g_tx [4]);
	assign_mgt_gty_rx ari5 (mgt_gty_rx_if [1][3], p25g_rx [5]);
	assign_mgt_gty_tx ati5 (mgt_gty_tx_if [1][3], p25g_tx [5]);
	assign_mgt_gty_rx ari6 (mgt_gty_rx_if [2][0], p25g_rx [6]);
	assign_mgt_gty_tx ati6 (mgt_gty_tx_if [2][0], p25g_tx [6]);
	assign_mgt_gty_rx ari7 (mgt_gty_rx_if [2][1], p25g_rx [7]);
	assign_mgt_gty_tx ati7 (mgt_gty_tx_if [2][1], p25g_tx [7]);
	assign_mgt_gty_rx ari8 (mgt_gty_rx_if [2][2], p25g_rx [8]);
	assign_mgt_gty_tx ati8 (mgt_gty_tx_if [2][2], p25g_tx [8]);
	assign_mgt_gty_rx ari9 (mgt_gty_rx_if [2][3], p25g_rx [9]);
	assign_mgt_gty_tx ati9 (mgt_gty_tx_if [2][3], p25g_tx [9]);
	assign_mgt_gty_rx ari10 (mgt_gty_rx_if [3][0], p25g_rx [10]);
	assign_mgt_gty_tx ati10 (mgt_gty_tx_if [3][0], p25g_tx [10]);
	assign_mgt_gty_rx ari11 (mgt_gty_rx_if [3][1], p25g_rx [11]);
	assign_mgt_gty_tx ati11 (mgt_gty_tx_if [3][1], p25g_tx [11]);
	assign_mgt_gty_rx ari12 (mgt_gty_rx_if [3][2], p25g_rx [12]);
	assign_mgt_gty_tx ati12 (mgt_gty_tx_if [3][2], p25g_tx [12]);
	assign_mgt_gty_rx ari13 (mgt_gty_rx_if [3][3], p25g_rx [13]);
	assign_mgt_gty_tx ati13 (mgt_gty_tx_if [3][3], p25g_tx [13]);
	assign_mgt_gty_rx ari14 (mgt_gty_rx_if [4][0], p25g_rx [14]);
	assign_mgt_gty_tx ati14 (mgt_gty_tx_if [4][0], p25g_tx [14]);
	assign_mgt_gty_rx ari15 (mgt_gty_rx_if [4][1], p25g_rx [15]);
	assign_mgt_gty_tx ati15 (mgt_gty_tx_if [4][1], p25g_tx [15]);
	assign_mgt_gty_rx ari16 (mgt_gty_rx_if [4][2], p25g_rx [16]);
	assign_mgt_gty_tx ati16 (mgt_gty_tx_if [4][2], p25g_tx [16]);
	assign_mgt_gty_rx ari17 (mgt_gty_rx_if [4][3], p25g_rx [17]);
	assign_mgt_gty_tx ati17 (mgt_gty_tx_if [4][3], p25g_tx [17]);
	assign_mgt_gty_rx ari18 (mgt_gty_rx_if [5][0], p25g_rx [18]);
	assign_mgt_gty_tx ati18 (mgt_gty_tx_if [5][0], p25g_tx [18]);
	assign_mgt_gty_rx ari19 (mgt_gty_rx_if [5][1], p25g_rx [19]);
	assign_mgt_gty_tx ati19 (mgt_gty_tx_if [5][1], p25g_tx [19]);
	assign_mgt_gty_rx ari20 (mgt_gty_rx_if [5][2], p25g_rx [20]);
	assign_mgt_gty_tx ati20 (mgt_gty_tx_if [5][2], p25g_tx [20]);
	assign_mgt_gty_rx ari21 (mgt_gty_rx_if [5][3], p25g_rx [21]);
	assign_mgt_gty_tx ati21 (mgt_gty_tx_if [5][3], p25g_tx [21]);
	assign_mgt_gty_rx ari22 (mgt_gty_rx_if [6][0], p25g_rx [22]);
	assign_mgt_gty_tx ati22 (mgt_gty_tx_if [6][0], p25g_tx [22]);
	assign_mgt_gty_rx ari23 (mgt_gty_rx_if [6][1], p25g_rx [23]);
	assign_mgt_gty_tx ati23 (mgt_gty_tx_if [6][1], p25g_tx [23]);
	assign_mgt_gty_rx ari24 (mgt_gty_rx_if [6][2], p25g_rx [24]);
	assign_mgt_gty_tx ati24 (mgt_gty_tx_if [6][2], p25g_tx [24]);
	assign_mgt_gty_rx ari25 (mgt_gty_rx_if [6][3], p25g_rx [25]);
	assign_mgt_gty_tx ati25 (mgt_gty_tx_if [6][3], p25g_tx [25]);
	assign_mgt_gty_rx ari26 (mgt_gty_rx_if [7][0], p25g_rx [26]);
	assign_mgt_gty_tx ati26 (mgt_gty_tx_if [7][0], p25g_tx [26]);
	assign_mgt_gty_rx ari27 (mgt_gty_rx_if [7][1], p25g_rx [27]);
	assign_mgt_gty_tx ati27 (mgt_gty_tx_if [7][1], p25g_tx [27]);
	assign_mgt_gty_rx ari28 (mgt_gty_rx_if [7][2], p25g_rx [28]);
	assign_mgt_gty_tx ati28 (mgt_gty_tx_if [7][2], p25g_tx [28]);
	assign_mgt_gty_rx ari29 (mgt_gty_rx_if [7][3], p25g_rx [29]);
	assign_mgt_gty_tx ati29 (mgt_gty_tx_if [7][3], p25g_tx [29]);
	assign_mgt_gty_rx ari30 (mgt_gty_rx_if [8][0], p25g_rx [30]);
	assign_mgt_gty_tx ati30 (mgt_gty_tx_if [8][0], p25g_tx [30]);
	assign_mgt_gty_rx ari31 (mgt_gty_rx_if [8][1], p25g_rx [31]);
	assign_mgt_gty_tx ati31 (mgt_gty_tx_if [8][1], p25g_tx [31]);
	assign_mgt_gty_rx ari32 (mgt_gty_rx_if [8][2], p25g_rx [32]);
	assign_mgt_gty_tx ati32 (mgt_gty_tx_if [8][2], p25g_tx [32]);
	assign_mgt_gty_rx ari33 (mgt_gty_rx_if [8][3], p25g_rx [33]);
	assign_mgt_gty_tx ati33 (mgt_gty_tx_if [8][3], p25g_tx [33]);
	assign_mgt_gty_rx ari34 (mgt_gty_rx_if [9][0], p25g_rx [34]);
	assign_mgt_gty_tx ati34 (mgt_gty_tx_if [9][0], p25g_tx [34]);
	assign_mgt_gty_rx ari35 (mgt_gty_rx_if [9][1], p25g_rx [35]);
	assign_mgt_gty_tx ati35 (mgt_gty_tx_if [9][1], p25g_tx [35]);
	assign_mgt_gty_rx ari36 (mgt_gty_rx_if [9][2], p25g_rx [36]);
	assign_mgt_gty_tx ati36 (mgt_gty_tx_if [9][2], p25g_tx [36]);
	assign_mgt_gty_rx ari37 (mgt_gty_rx_if [9][3], p25g_rx [37]);
	assign_mgt_gty_tx ati37 (mgt_gty_tx_if [9][3], p25g_tx [37]);
	assign_mgt_gty_rx ari38 (mgt_gty_rx_if [10][0], p25g_rx [38]);
	assign_mgt_gty_tx ati38 (mgt_gty_tx_if [10][0], p25g_tx [38]);
	assign_mgt_gty_rx ari39 (mgt_gty_rx_if [10][1], p25g_rx [39]);
	assign_mgt_gty_tx ati39 (mgt_gty_tx_if [10][1], p25g_tx [39]);
	assign_mgt_gty_rx ari40 (mgt_gty_rx_if [10][2], p25g_rx [40]);
	assign_mgt_gty_tx ati40 (mgt_gty_tx_if [10][2], p25g_tx [40]);
	assign_mgt_gty_rx ari41 (mgt_gty_rx_if [10][3], p25g_rx [41]);
	assign_mgt_gty_tx ati41 (mgt_gty_tx_if [10][3], p25g_tx [41]);
	assign_mgt_gty_rx ari42 (mgt_gty_rx_if [11][0], p25g_rx [42]);
	assign_mgt_gty_tx ati42 (mgt_gty_tx_if [11][0], p25g_tx [42]);
	assign_mgt_gty_rx ari43 (mgt_gty_rx_if [11][1], p25g_rx [43]);
	assign_mgt_gty_tx ati43 (mgt_gty_tx_if [11][1], p25g_tx [43]);
	assign_mgt_gty_rx ari44 (mgt_gty_rx_if [11][2], p25g_rx [44]);
	assign_mgt_gty_tx ati44 (mgt_gty_tx_if [11][2], p25g_tx [44]);
	assign_mgt_gty_rx ari45 (mgt_gty_rx_if [11][3], p25g_rx [45]);
	assign_mgt_gty_tx ati45 (mgt_gty_tx_if [11][3], p25g_tx [45]);
	assign_mgt_gty_rx ari46 (mgt_gty_rx_if [12][0], p25g_rx [46]);
	assign_mgt_gty_tx ati46 (mgt_gty_tx_if [12][0], p25g_tx [46]);
	assign_mgt_gty_rx ari47 (mgt_gty_rx_if [12][1], p25g_rx [47]);
	assign_mgt_gty_tx ati47 (mgt_gty_tx_if [12][1], p25g_tx [47]);
	assign_mgt_gty_rx ari48 (mgt_gty_rx_if [12][2], p25g_rx [48]);
	assign_mgt_gty_tx ati48 (mgt_gty_tx_if [12][2], p25g_tx [48]);
	assign_mgt_gty_rx ari49 (mgt_gty_rx_if [12][3], p25g_rx [49]);
	assign_mgt_gty_tx ati49 (mgt_gty_tx_if [12][3], p25g_tx [49]);
	assign_mgt_gty_rx ari50 (mgt_gty_rx_if [13][0], p25g_rx [50]);
	assign_mgt_gty_tx ati50 (mgt_gty_tx_if [13][0], p25g_tx [50]);
	assign_mgt_gty_rx ari51 (mgt_gty_rx_if [13][1], p25g_rx [51]);
	assign_mgt_gty_tx ati51 (mgt_gty_tx_if [13][1], p25g_tx [51]);
	assign_mgt_gty_rx ari52 (mgt_gty_rx_if [13][2], p25g_rx [52]);
	assign_mgt_gty_tx ati52 (mgt_gty_tx_if [13][2], p25g_tx [52]);
	assign_mgt_gty_rx ari53 (mgt_gty_rx_if [13][3], p25g_rx [53]);
	assign_mgt_gty_tx ati53 (mgt_gty_tx_if [13][3], p25g_tx [53]);
	assign_mgt_gty_rx ari54 (mgt_gty_rx_if [14][0], p25g_rx [54]);
	assign_mgt_gty_tx ati54 (mgt_gty_tx_if [14][0], p25g_tx [54]);
	assign_mgt_gty_rx ari55 (mgt_gty_rx_if [14][1], p25g_rx [55]);
	assign_mgt_gty_tx ati55 (mgt_gty_tx_if [14][1], p25g_tx [55]);
	assign_mgt_gty_rx ari56 (mgt_gty_rx_if [14][2], p25g_rx [56]);
	assign_mgt_gty_tx ati56 (mgt_gty_tx_if [14][2], p25g_tx [56]);
	assign_mgt_gty_rx ari57 (mgt_gty_rx_if [14][3], p25g_rx [57]);
	assign_mgt_gty_tx ati57 (mgt_gty_tx_if [14][3], p25g_tx [57]);
	assign_mgt_gty_rx ari58 (mgt_gty_rx_if [15][0], p25g_rx [58]);
	assign_mgt_gty_tx ati58 (mgt_gty_tx_if [15][0], p25g_tx [58]);
	assign_mgt_gty_rx ari59 (mgt_gty_rx_if [15][1], p25g_rx [59]);
	assign_mgt_gty_tx ati59 (mgt_gty_tx_if [15][1], p25g_tx [59]);
	assign_mgt_gty_rx ari60 (mgt_gty_rx_if [15][2], p25g_rx [60]);
	assign_mgt_gty_tx ati60 (mgt_gty_tx_if [15][2], p25g_tx [60]);
	assign_mgt_gty_rx ari61 (mgt_gty_rx_if [15][3], p25g_rx [61]);
	assign_mgt_gty_tx ati61 (mgt_gty_tx_if [15][3], p25g_tx [61]);
	assign_mgt_gty_rx ari62 (mgt_gty_rx_if [16][0], p25g_rx [62]);
	assign_mgt_gty_tx ati62 (mgt_gty_tx_if [16][0], p25g_tx [62]);
	assign_mgt_gty_rx ari63 (mgt_gty_rx_if [16][1], p25g_rx [63]);
	assign_mgt_gty_tx ati63 (mgt_gty_tx_if [16][1], p25g_tx [63]);
	assign_mgt_gty_rx ari64 (mgt_gty_rx_if [16][2], p25g_rx [64]);
	assign_mgt_gty_tx ati64 (mgt_gty_tx_if [16][2], p25g_tx [64]);
	assign_mgt_gty_rx ari65 (mgt_gty_rx_if [16][3], p25g_rx [65]);
	assign_mgt_gty_tx ati65 (mgt_gty_tx_if [16][3], p25g_tx [65]);
	assign_mgt_gty_rx ari66 (mgt_gty_rx_if [17][0], p25g_rx [66]);
	assign_mgt_gty_tx ati66 (mgt_gty_tx_if [17][0], p25g_tx [66]);
	assign_mgt_gty_rx ari67 (mgt_gty_rx_if [17][1], p25g_rx [67]);
	assign_mgt_gty_tx ati67 (mgt_gty_tx_if [17][1], p25g_tx [67]);
	assign_mgt_gty_rx ari68 (mgt_gty_rx_if [17][2], p25g_rx [68]);
	assign_mgt_gty_tx ati68 (mgt_gty_tx_if [17][2], p25g_tx [68]);
	assign_mgt_gty_rx ari69 (mgt_gty_rx_if [17][3], p25g_rx [69]);
	assign_mgt_gty_tx ati69 (mgt_gty_tx_if [17][3], p25g_tx [69]);
	assign_mgt_gty_rx ari70 (mgt_gty_rx_if [18][0], p25g_rx [70]);
	assign_mgt_gty_tx ati70 (mgt_gty_tx_if [18][0], p25g_tx [70]);
	assign_mgt_gty_rx ari71 (mgt_gty_rx_if [18][1], p25g_rx [71]);
	assign_mgt_gty_tx ati71 (mgt_gty_tx_if [18][1], p25g_tx [71]);
	assign_mgt_gty_rx ari72 (mgt_gty_rx_if [18][2], p25g_rx [72]);
	assign_mgt_gty_tx ati72 (mgt_gty_tx_if [18][2], p25g_tx [72]);
	assign_mgt_gty_rx ari73 (mgt_gty_rx_if [18][3], p25g_rx [73]);
	assign_mgt_gty_tx ati73 (mgt_gty_tx_if [18][3], p25g_tx [73]);
	assign_mgt_gty_rx ari74 (mgt_gty_rx_if [19][0], p25g_rx [74]);
	assign_mgt_gty_tx ati74 (mgt_gty_tx_if [19][0], p25g_tx [74]);
	assign_mgt_gty_rx ari75 (mgt_gty_rx_if [19][1], p25g_rx [75]);
	assign_mgt_gty_tx ati75 (mgt_gty_tx_if [19][1], p25g_tx [75]);
	assign_mgt_gty_rx ari76 (mgt_gty_rx_if [19][2], p25g_rx [76]);
	assign_mgt_gty_tx ati76 (mgt_gty_tx_if [19][2], p25g_tx [76]);
	assign_mgt_gty_rx ari77 (mgt_gty_rx_if [19][3], p25g_rx [77]);
	assign_mgt_gty_tx ati77 (mgt_gty_tx_if [19][3], p25g_tx [77]);
	assign_mgt_gty_rx ari78 (mgt_gty_rx_if [20][0], p25g_rx [78]);
	assign_mgt_gty_tx ati78 (mgt_gty_tx_if [20][0], p25g_tx [78]);
	assign_mgt_gty_rx ari79 (mgt_gty_rx_if [20][1], p25g_rx [79]);
	assign_mgt_gty_tx ati79 (mgt_gty_tx_if [20][1], p25g_tx [79]);
	assign_mgt_gty_rx ari80 (mgt_gty_rx_if [20][2], p25g_rx [80]);
	assign_mgt_gty_tx ati80 (mgt_gty_tx_if [20][2], p25g_tx [80]);
	assign_mgt_gty_rx ari81 (mgt_gty_rx_if [20][3], p25g_rx [81]);
	assign_mgt_gty_tx ati81 (mgt_gty_tx_if [20][3], p25g_tx [81]);
	assign_mgt_gty_rx ari82 (mgt_gty_rx_if [21][0], p25g_rx [82]);
	assign_mgt_gty_tx ati82 (mgt_gty_tx_if [21][0], p25g_tx [82]);
	assign_mgt_gty_rx ari83 (mgt_gty_rx_if [21][1], p25g_rx [83]);
	assign_mgt_gty_tx ati83 (mgt_gty_tx_if [21][1], p25g_tx [83]);
	assign_mgt_gty_rx ari84 (mgt_gty_rx_if [21][2], p25g_rx [84]);
	assign_mgt_gty_tx ati84 (mgt_gty_tx_if [21][2], p25g_tx [84]);
	assign_mgt_gty_rx ari85 (mgt_gty_rx_if [21][3], p25g_rx [85]);
	assign_mgt_gty_tx ati85 (mgt_gty_tx_if [21][3], p25g_tx [85]);
	assign_mgt_gty_rx ari86 (mgt_gty_rx_if [22][0], p25g_rx [86]);
	assign_mgt_gty_tx ati86 (mgt_gty_tx_if [22][0], p25g_tx [86]);
	assign_mgt_gty_rx ari87 (mgt_gty_rx_if [22][1], p25g_rx [87]);
	assign_mgt_gty_tx ati87 (mgt_gty_tx_if [22][1], p25g_tx [87]);
	assign_mgt_gty_rx ari88 (mgt_gty_rx_if [22][2], p25g_rx [88]);
	assign_mgt_gty_tx ati88 (mgt_gty_tx_if [22][2], p25g_tx [88]);
	assign_mgt_gty_rx ari89 (mgt_gty_rx_if [22][3], p25g_rx [89]);
	assign_mgt_gty_tx ati89 (mgt_gty_tx_if [22][3], p25g_tx [89]);
	assign_mgt_gty_rx ari90 (mgt_gty_rx_if [23][0], p25g_rx [90]);
	assign_mgt_gty_tx ati90 (mgt_gty_tx_if [23][0], p25g_tx [90]);
	assign_mgt_gty_rx ari91 (mgt_gty_rx_if [23][1], p25g_rx [91]);
	assign_mgt_gty_tx ati91 (mgt_gty_tx_if [23][1], p25g_tx [91]);
	assign_mgt_gty_rx ari92 (mgt_gty_rx_if [23][2], p25g_rx [92]);
	assign_mgt_gty_tx ati92 (mgt_gty_tx_if [23][2], p25g_tx [92]);
	assign_mgt_gty_rx ari93 (mgt_gty_rx_if [23][3], p25g_rx [93]);
	assign_mgt_gty_tx ati93 (mgt_gty_tx_if [23][3], p25g_tx [93]);
	assign_mgt_gty_rx ari94 (mgt_gty_rx_if [24][0], p25g_rx [94]);
	assign_mgt_gty_tx ati94 (mgt_gty_tx_if [24][0], p25g_tx [94]);
	assign_mgt_gty_rx ari95 (mgt_gty_rx_if [24][1], p25g_rx [95]);
	assign_mgt_gty_tx ati95 (mgt_gty_tx_if [24][1], p25g_tx [95]);
	assign_mgt_gty_rx ari96 (mgt_gty_rx_if [24][2], p25g_rx [96]);
	assign_mgt_gty_tx ati96 (mgt_gty_tx_if [24][2], p25g_tx [96]);
	assign_mgt_gty_rx ari97 (mgt_gty_rx_if [24][3], p25g_rx [97]);
	assign_mgt_gty_tx ati97 (mgt_gty_tx_if [24][3], p25g_tx [97]);
	assign_mgt_gty_rx ari98 (mgt_gty_rx_if [25][0], p25g_rx [98]);
	assign_mgt_gty_tx ati98 (mgt_gty_tx_if [25][0], p25g_tx [98]);
	assign_mgt_gty_rx ari99 (mgt_gty_rx_if [25][1], p25g_rx [99]);
	assign_mgt_gty_tx ati99 (mgt_gty_tx_if [25][1], p25g_tx [99]);
	assign_mgt_gty_rx ari100 (mgt_gty_rx_if [25][2], p25g_rx [100]);
	assign_mgt_gty_tx ati100 (mgt_gty_tx_if [25][2], p25g_tx [100]);
	assign_mgt_gty_rx ari101 (mgt_gty_rx_if [25][3], p25g_rx [101]);
	assign_mgt_gty_tx ati101 (mgt_gty_tx_if [25][3], p25g_tx [101]);
	assign_mgt_gty_rx ari102 (mgt_gty_rx_if [26][0], p25g_rx [102]);
	assign_mgt_gty_tx ati102 (mgt_gty_tx_if [26][0], p25g_tx [102]);
	assign_mgt_gty_rx ari103 (mgt_gty_rx_if [26][1], p25g_rx [103]);
	assign_mgt_gty_tx ati103 (mgt_gty_tx_if [26][1], p25g_tx [103]);
	assign_mgt_gty_rx ari104 (mgt_gty_rx_if [26][2], p25g_rx [104]);
	assign_mgt_gty_tx ati104 (mgt_gty_tx_if [26][2], p25g_tx [104]);
	assign_mgt_gty_rx ari105 (mgt_gty_rx_if [26][3], p25g_rx [105]);
	assign_mgt_gty_tx ati105 (mgt_gty_tx_if [26][3], p25g_tx [105]);
	assign_mgt_gty_rx ari106 (mgt_gty_rx_if [27][0], p25g_rx [106]);
	assign_mgt_gty_tx ati106 (mgt_gty_tx_if [27][0], p25g_tx [106]);
	assign_mgt_gty_rx ari107 (mgt_gty_rx_if [27][1], p25g_rx [107]);
	assign_mgt_gty_tx ati107 (mgt_gty_tx_if [27][1], p25g_tx [107]);
	assign_mgt_gty_rx ari108 (mgt_gty_rx_if [27][2], p25g_rx [108]);
	assign_mgt_gty_tx ati108 (mgt_gty_tx_if [27][2], p25g_tx [108]);
	assign_mgt_gty_rx ari109 (mgt_gty_rx_if [27][3], p25g_rx [109]);
	assign_mgt_gty_tx ati109 (mgt_gty_tx_if [27][3], p25g_tx [109]);
	assign_mgt_gty_rx ari110 (mgt_gty_rx_if [28][0], p25g_rx [110]);
	assign_mgt_gty_tx ati110 (mgt_gty_tx_if [28][0], p25g_tx [110]);
	assign_mgt_gty_rx ari111 (mgt_gty_rx_if [28][1], p25g_rx [111]);
	assign_mgt_gty_tx ati111 (mgt_gty_tx_if [28][1], p25g_tx [111]);
	assign_mgt_gty_rx ari112 (mgt_gty_rx_if [28][2], p25g_rx [112]);
	assign_mgt_gty_tx ati112 (mgt_gty_tx_if [28][2], p25g_tx [112]);
	assign_mgt_gty_rx ari113 (mgt_gty_rx_if [28][3], p25g_rx [113]);
	assign_mgt_gty_tx ati113 (mgt_gty_tx_if [28][3], p25g_tx [113]);
	assign_mgt_gty_rx ari114 (mgt_gty_rx_if [29][0], p25g_rx [114]);
	assign_mgt_gty_tx ati114 (mgt_gty_tx_if [29][0], p25g_tx [114]);
	assign_mgt_gty_rx ari115 (mgt_gty_rx_if [29][1], p25g_rx [115]);
	assign_mgt_gty_tx ati115 (mgt_gty_tx_if [29][1], p25g_tx [115]);
	assign_mgt_gty_rx ari116 (mgt_gty_rx_if [29][2], p25g_rx [116]);
	assign_mgt_gty_tx ati116 (mgt_gty_tx_if [29][2], p25g_tx [116]);
	assign_mgt_gty_rx ari117 (mgt_gty_rx_if [29][3], p25g_rx [117]);
	assign_mgt_gty_tx ati117 (mgt_gty_tx_if [29][3], p25g_tx [117]);
	assign_mgt_gty_rx ari118 (mgt_gty_rx_if [30][0], p25g_rx [118]);
	assign_mgt_gty_tx ati118 (mgt_gty_tx_if [30][0], p25g_tx [118]);
	assign_mgt_gty_rx ari119 (mgt_gty_rx_if [30][1], p25g_rx [119]);
	assign_mgt_gty_tx ati119 (mgt_gty_tx_if [30][1], p25g_tx [119]);
	assign_mgt_gty_rx ari120 (mgt_gty_rx_if [30][2], p25g_rx [120]);
	assign_mgt_gty_tx ati120 (mgt_gty_tx_if [30][2], p25g_tx [120]);
	assign_mgt_gty_rx ari121 (mgt_gty_rx_if [30][3], p25g_rx [121]);
	assign_mgt_gty_tx ati121 (mgt_gty_tx_if [30][3], p25g_tx [121]);
	assign_mgt_gty_rx ari122 (mgt_gty_rx_if [31][0], p25g_rx [122]);
	assign_mgt_gty_tx ati122 (mgt_gty_tx_if [31][0], p25g_tx [122]);
	assign_mgt_gty_rx ari123 (mgt_gty_rx_if [31][1], p25g_rx [123]);
	assign_mgt_gty_tx ati123 (mgt_gty_tx_if [31][1], p25g_tx [123]);
	assign_mgt_gty_rx ari124 (mgt_gty_rx_if [31][2], p25g_rx [124]);
	assign_mgt_gty_tx ati124 (mgt_gty_tx_if [31][2], p25g_tx [124]);
	assign_mgt_gty_rx ari125 (mgt_gty_rx_if [31][3], p25g_rx [125]);
	assign_mgt_gty_tx ati125 (mgt_gty_tx_if [31][3], p25g_tx [125]);
	assign com_gtrefclk00 [0] = refclk [0]; // p25g_tx 0
	assign com_gtrefclk00 [0] = refclk [0]; // p25g_tx 1
	assign com_gtrefclk00 [0] = refclk [0]; // p25g_tx 2
	assign com_gtrefclk00 [1] = refclk [1]; // p25g_tx 3
	assign com_gtrefclk00 [1] = refclk [1]; // p25g_tx 4
	assign com_gtrefclk00 [1] = refclk [1]; // p25g_tx 5
	assign com_gtrefclk00 [2] = refclk [2]; // p25g_tx 6
	assign com_gtrefclk00 [2] = refclk [2]; // p25g_tx 7
	assign com_gtrefclk00 [2] = refclk [2]; // p25g_tx 8
	assign com_gtrefclk00 [2] = refclk [2]; // p25g_tx 9
	assign com_gtrefclk00 [3] = refclk [3]; // p25g_tx 10
	assign com_gtrefclk00 [3] = refclk [3]; // p25g_tx 11
	assign com_gtrefclk00 [3] = refclk [3]; // p25g_tx 12
	assign com_gtrefclk00 [3] = refclk [3]; // p25g_tx 13
	assign com_gtrefclk00 [4] = refclk [4]; // p25g_tx 14
	assign com_gtrefclk00 [4] = refclk [4]; // p25g_tx 15
	assign com_gtrefclk00 [4] = refclk [4]; // p25g_tx 16
	assign com_gtrefclk00 [4] = refclk [4]; // p25g_tx 17
	assign com_gtrefclk00 [5] = refclk [5]; // p25g_tx 18
	assign com_gtrefclk00 [5] = refclk [5]; // p25g_tx 19
	assign com_gtrefclk00 [5] = refclk [5]; // p25g_tx 20
	assign com_gtrefclk00 [5] = refclk [5]; // p25g_tx 21
	assign com_gtrefclk00 [6] = refclk [6]; // p25g_tx 22
	assign com_gtrefclk00 [6] = refclk [6]; // p25g_tx 23
	assign com_gtrefclk00 [6] = refclk [6]; // p25g_tx 24
	assign com_gtrefclk00 [6] = refclk [6]; // p25g_tx 25
	assign com_gtrefclk00 [7] = refclk [7]; // p25g_tx 26
	assign com_gtrefclk00 [7] = refclk [7]; // p25g_tx 27
	assign com_gtrefclk00 [7] = refclk [7]; // p25g_tx 28
	assign com_gtrefclk00 [7] = refclk [7]; // p25g_tx 29
	assign com_gtrefclk00 [8] = refclk [8]; // p25g_tx 30
	assign com_gtrefclk00 [8] = refclk [8]; // p25g_tx 31
	assign com_gtrefclk00 [8] = refclk [8]; // p25g_tx 32
	assign com_gtrefclk00 [8] = refclk [8]; // p25g_tx 33
	assign com_gtrefclk00 [9] = refclk [9]; // p25g_tx 34
	assign com_gtrefclk00 [9] = refclk [9]; // p25g_tx 35
	assign com_gtrefclk00 [9] = refclk [9]; // p25g_tx 36
	assign com_gtrefclk00 [9] = refclk [9]; // p25g_tx 37
	assign com_gtrefclk00 [10] = refclk [10]; // p25g_tx 38
	assign com_gtrefclk00 [10] = refclk [10]; // p25g_tx 39
	assign com_gtrefclk00 [10] = refclk [10]; // p25g_tx 40
	assign com_gtrefclk00 [10] = refclk [10]; // p25g_tx 41
	assign com_gtrefclk00 [11] = refclk [11]; // p25g_tx 42
	assign com_gtrefclk00 [11] = refclk [11]; // p25g_tx 43
	assign com_gtrefclk00 [11] = refclk [11]; // p25g_tx 44
	assign com_gtrefclk00 [11] = refclk [11]; // p25g_tx 45
	assign com_gtrefclk00 [12] = refclk [12]; // p25g_tx 46
	assign com_gtrefclk00 [12] = refclk [12]; // p25g_tx 47
	assign com_gtrefclk00 [12] = refclk [12]; // p25g_tx 48
	assign com_gtrefclk00 [12] = refclk [12]; // p25g_tx 49
	assign com_gtrefclk00 [13] = refclk [13]; // p25g_tx 50
	assign com_gtrefclk00 [13] = refclk [13]; // p25g_tx 51
	assign com_gtrefclk00 [13] = refclk [13]; // p25g_tx 52
	assign com_gtrefclk00 [13] = refclk [13]; // p25g_tx 53
	assign com_gtrefclk00 [14] = refclk [14]; // p25g_tx 54
	assign com_gtrefclk00 [14] = refclk [14]; // p25g_tx 55
	assign com_gtrefclk00 [14] = refclk [14]; // p25g_tx 56
	assign com_gtrefclk00 [14] = refclk [14]; // p25g_tx 57
	assign com_gtrefclk00 [15] = refclk [15]; // p25g_tx 58
	assign com_gtrefclk00 [15] = refclk [15]; // p25g_tx 59
	assign com_gtrefclk00 [15] = refclk [15]; // p25g_tx 60
	assign com_gtrefclk00 [15] = refclk [15]; // p25g_tx 61
	assign com_gtrefclk00 [16] = refclk [16]; // p25g_tx 62
	assign com_gtrefclk00 [16] = refclk [16]; // p25g_tx 63
	assign com_gtrefclk00 [16] = refclk [16]; // p25g_tx 64
	assign com_gtrefclk00 [16] = refclk [16]; // p25g_tx 65
	assign com_gtrefclk00 [17] = refclk [17]; // p25g_tx 66
	assign com_gtrefclk00 [17] = refclk [17]; // p25g_tx 67
	assign com_gtrefclk00 [17] = refclk [17]; // p25g_tx 68
	assign com_gtrefclk00 [17] = refclk [17]; // p25g_tx 69
	assign com_gtrefclk00 [18] = refclk [18]; // p25g_tx 70
	assign com_gtrefclk00 [18] = refclk [18]; // p25g_tx 71
	assign com_gtrefclk00 [18] = refclk [18]; // p25g_tx 72
	assign com_gtrefclk00 [18] = refclk [18]; // p25g_tx 73
	assign com_gtrefclk00 [19] = refclk [19]; // p25g_tx 74
	assign com_gtrefclk00 [19] = refclk [19]; // p25g_tx 75
	assign com_gtrefclk00 [19] = refclk [19]; // p25g_tx 76
	assign com_gtrefclk00 [19] = refclk [19]; // p25g_tx 77
	assign com_gtrefclk00 [20] = refclk [20]; // p25g_tx 78
	assign com_gtrefclk00 [20] = refclk [20]; // p25g_tx 79
	assign com_gtrefclk00 [20] = refclk [20]; // p25g_tx 80
	assign com_gtrefclk00 [20] = refclk [20]; // p25g_tx 81
	assign com_gtrefclk00 [21] = refclk [21]; // p25g_tx 82
	assign com_gtrefclk00 [21] = refclk [21]; // p25g_tx 83
	assign com_gtrefclk00 [21] = refclk [21]; // p25g_tx 84
	assign com_gtrefclk00 [21] = refclk [21]; // p25g_tx 85
	assign com_gtrefclk00 [22] = refclk [22]; // p25g_tx 86
	assign com_gtrefclk00 [22] = refclk [22]; // p25g_tx 87
	assign com_gtrefclk00 [22] = refclk [22]; // p25g_tx 88
	assign com_gtrefclk00 [22] = refclk [22]; // p25g_tx 89
	assign com_gtrefclk00 [23] = refclk [23]; // p25g_tx 90
	assign com_gtrefclk00 [23] = refclk [23]; // p25g_tx 91
	assign com_gtrefclk00 [23] = refclk [23]; // p25g_tx 92
	assign com_gtrefclk00 [23] = refclk [23]; // p25g_tx 93
	assign com_gtrefclk00 [24] = refclk [24]; // p25g_tx 94
	assign com_gtrefclk00 [24] = refclk [24]; // p25g_tx 95
	assign com_gtrefclk00 [24] = refclk [24]; // p25g_tx 96
	assign com_gtrefclk00 [24] = refclk [24]; // p25g_tx 97
	assign com_gtrefclk00 [25] = refclk [25]; // p25g_tx 98
	assign com_gtrefclk00 [25] = refclk [25]; // p25g_tx 99
	assign com_gtrefclk00 [25] = refclk [25]; // p25g_tx 100
	assign com_gtrefclk00 [25] = refclk [25]; // p25g_tx 101
	assign com_gtrefclk00 [26] = refclk [26]; // p25g_tx 102
	assign com_gtrefclk00 [26] = refclk [26]; // p25g_tx 103
	assign com_gtrefclk00 [26] = refclk [26]; // p25g_tx 104
	assign com_gtrefclk00 [26] = refclk [26]; // p25g_tx 105
	assign com_gtrefclk00 [27] = refclk [27]; // p25g_tx 106
	assign com_gtrefclk00 [27] = refclk [27]; // p25g_tx 107
	assign com_gtrefclk00 [27] = refclk [27]; // p25g_tx 108
	assign com_gtrefclk00 [27] = refclk [27]; // p25g_tx 109
	assign com_gtrefclk00 [28] = refclk [28]; // p25g_tx 110
	assign com_gtrefclk00 [28] = refclk [28]; // p25g_tx 111
	assign com_gtrefclk00 [28] = refclk [28]; // p25g_tx 112
	assign com_gtrefclk00 [28] = refclk [28]; // p25g_tx 113
	assign com_gtrefclk00 [29] = refclk [29]; // p25g_tx 114
	assign com_gtrefclk00 [29] = refclk [29]; // p25g_tx 115
	assign com_gtrefclk00 [29] = refclk [29]; // p25g_tx 116
	assign com_gtrefclk00 [29] = refclk [29]; // p25g_tx 117
	assign com_gtrefclk00 [30] = refclk [30]; // p25g_tx 118
	assign com_gtrefclk00 [30] = refclk [30]; // p25g_tx 119
	assign com_gtrefclk00 [30] = refclk [30]; // p25g_tx 120
	assign com_gtrefclk00 [30] = refclk [30]; // p25g_tx 121
	assign com_gtrefclk00 [31] = refclk [31]; // p25g_tx 122
	assign com_gtrefclk00 [31] = refclk [31]; // p25g_tx 123
	assign com_gtrefclk00 [31] = refclk [31]; // p25g_tx 124
	assign com_gtrefclk00 [31] = refclk [31]; // p25g_tx 125
	pr25g_mmcm pr25g_13_inst (.O(pr25g_13_mmcm_clk), .I(mgt_gty_tx_if[13][0].txoutclk));
	assign mgt_txusrclk[13][0] = pr25g_13_mmcm_clk; // p25g_tx50
	assign mgt_txusrclk[13][1] = pr25g_13_mmcm_clk; // p25g_tx51
	assign mgt_txusrclk[13][2] = pr25g_13_mmcm_clk; // p25g_tx52
	assign mgt_txusrclk[13][3] = pr25g_13_mmcm_clk; // p25g_tx53
	pr25g_mmcm pr25g_12_inst (.O(pr25g_12_mmcm_clk), .I(mgt_gty_tx_if[12][0].txoutclk));
	assign mgt_txusrclk[12][0] = pr25g_12_mmcm_clk; // p25g_tx46
	assign mgt_txusrclk[12][1] = pr25g_12_mmcm_clk; // p25g_tx47
	assign mgt_txusrclk[12][2] = pr25g_12_mmcm_clk; // p25g_tx48
	assign mgt_txusrclk[12][3] = pr25g_12_mmcm_clk; // p25g_tx49
	pr25g_mmcm pr25g_11_inst (.O(pr25g_11_mmcm_clk), .I(mgt_gty_tx_if[11][0].txoutclk));
	assign mgt_txusrclk[11][0] = pr25g_11_mmcm_clk; // p25g_tx42
	assign mgt_txusrclk[11][1] = pr25g_11_mmcm_clk; // p25g_tx43
	assign mgt_txusrclk[11][2] = pr25g_11_mmcm_clk; // p25g_tx44
	assign mgt_txusrclk[11][3] = pr25g_11_mmcm_clk; // p25g_tx45
	pr25g_mmcm pr25g_10_inst (.O(pr25g_10_mmcm_clk), .I(mgt_gty_tx_if[10][0].txoutclk));
	assign mgt_txusrclk[10][0] = pr25g_10_mmcm_clk; // p25g_tx38
	assign mgt_txusrclk[10][1] = pr25g_10_mmcm_clk; // p25g_tx39
	assign mgt_txusrclk[10][2] = pr25g_10_mmcm_clk; // p25g_tx40
	assign mgt_txusrclk[10][3] = pr25g_10_mmcm_clk; // p25g_tx41
	pr25g_mmcm pr25g_31_inst (.O(pr25g_31_mmcm_clk), .I(mgt_gty_tx_if[31][0].txoutclk));
	assign mgt_txusrclk[31][0] = pr25g_31_mmcm_clk; // p25g_tx122
	assign mgt_txusrclk[31][1] = pr25g_31_mmcm_clk; // p25g_tx123
	assign mgt_txusrclk[31][2] = pr25g_31_mmcm_clk; // p25g_tx124
	assign mgt_txusrclk[31][3] = pr25g_31_mmcm_clk; // p25g_tx125
	pr25g_mmcm pr25g_30_inst (.O(pr25g_30_mmcm_clk), .I(mgt_gty_tx_if[30][0].txoutclk));
	assign mgt_txusrclk[30][0] = pr25g_30_mmcm_clk; // p25g_tx118
	assign mgt_txusrclk[30][1] = pr25g_30_mmcm_clk; // p25g_tx119
	assign mgt_txusrclk[30][2] = pr25g_30_mmcm_clk; // p25g_tx120
	assign mgt_txusrclk[30][3] = pr25g_30_mmcm_clk; // p25g_tx121
	pr25g_mmcm pr25g_29_inst (.O(pr25g_29_mmcm_clk), .I(mgt_gty_tx_if[29][0].txoutclk));
	assign mgt_txusrclk[29][0] = pr25g_29_mmcm_clk; // p25g_tx114
	assign mgt_txusrclk[29][1] = pr25g_29_mmcm_clk; // p25g_tx115
	assign mgt_txusrclk[29][2] = pr25g_29_mmcm_clk; // p25g_tx116
	assign mgt_txusrclk[29][3] = pr25g_29_mmcm_clk; // p25g_tx117
	pr25g_mmcm pr25g_28_inst (.O(pr25g_28_mmcm_clk), .I(mgt_gty_tx_if[28][0].txoutclk));
	assign mgt_txusrclk[28][0] = pr25g_28_mmcm_clk; // p25g_tx110
	assign mgt_txusrclk[28][1] = pr25g_28_mmcm_clk; // p25g_tx111
	assign mgt_txusrclk[28][2] = pr25g_28_mmcm_clk; // p25g_tx112
	assign mgt_txusrclk[28][3] = pr25g_28_mmcm_clk; // p25g_tx113
	pr25g_mmcm pr25g_27_inst (.O(pr25g_27_mmcm_clk), .I(mgt_gty_tx_if[27][0].txoutclk));
	assign mgt_txusrclk[27][0] = pr25g_27_mmcm_clk; // p25g_tx106
	assign mgt_txusrclk[27][1] = pr25g_27_mmcm_clk; // p25g_tx107
	assign mgt_txusrclk[27][2] = pr25g_27_mmcm_clk; // p25g_tx108
	assign mgt_txusrclk[27][3] = pr25g_27_mmcm_clk; // p25g_tx109
	pr25g_mmcm pr25g_26_inst (.O(pr25g_26_mmcm_clk), .I(mgt_gty_tx_if[26][0].txoutclk));
	assign mgt_txusrclk[26][0] = pr25g_26_mmcm_clk; // p25g_tx102
	assign mgt_txusrclk[26][1] = pr25g_26_mmcm_clk; // p25g_tx103
	assign mgt_txusrclk[26][2] = pr25g_26_mmcm_clk; // p25g_tx104
	assign mgt_txusrclk[26][3] = pr25g_26_mmcm_clk; // p25g_tx105
	pr25g_mmcm pr25g_25_inst (.O(pr25g_25_mmcm_clk), .I(mgt_gty_tx_if[25][0].txoutclk));
	assign mgt_txusrclk[25][0] = pr25g_25_mmcm_clk; // p25g_tx98
	assign mgt_txusrclk[25][1] = pr25g_25_mmcm_clk; // p25g_tx99
	assign mgt_txusrclk[25][2] = pr25g_25_mmcm_clk; // p25g_tx100
	assign mgt_txusrclk[25][3] = pr25g_25_mmcm_clk; // p25g_tx101
	pr25g_mmcm pr25g_9_inst (.O(pr25g_9_mmcm_clk), .I(mgt_gty_tx_if[9][0].txoutclk));
	assign mgt_txusrclk[9][0] = pr25g_9_mmcm_clk; // p25g_tx34
	assign mgt_txusrclk[9][1] = pr25g_9_mmcm_clk; // p25g_tx35
	assign mgt_txusrclk[9][2] = pr25g_9_mmcm_clk; // p25g_tx36
	assign mgt_txusrclk[9][3] = pr25g_9_mmcm_clk; // p25g_tx37
	pr25g_mmcm pr25g_5_inst (.O(pr25g_5_mmcm_clk), .I(mgt_gty_tx_if[5][0].txoutclk));
	assign mgt_txusrclk[5][0] = pr25g_5_mmcm_clk; // p25g_tx18
	assign mgt_txusrclk[5][1] = pr25g_5_mmcm_clk; // p25g_tx19
	assign mgt_txusrclk[5][2] = pr25g_5_mmcm_clk; // p25g_tx20
	assign mgt_txusrclk[5][3] = pr25g_5_mmcm_clk; // p25g_tx21
	pr25g_mmcm pr25g_6_inst (.O(pr25g_6_mmcm_clk), .I(mgt_gty_tx_if[6][0].txoutclk));
	assign mgt_txusrclk[6][0] = pr25g_6_mmcm_clk; // p25g_tx22
	assign mgt_txusrclk[6][1] = pr25g_6_mmcm_clk; // p25g_tx23
	assign mgt_txusrclk[6][2] = pr25g_6_mmcm_clk; // p25g_tx24
	assign mgt_txusrclk[6][3] = pr25g_6_mmcm_clk; // p25g_tx25
	pr25g_mmcm pr25g_7_inst (.O(pr25g_7_mmcm_clk), .I(mgt_gty_tx_if[7][0].txoutclk));
	assign mgt_txusrclk[7][0] = pr25g_7_mmcm_clk; // p25g_tx26
	assign mgt_txusrclk[7][1] = pr25g_7_mmcm_clk; // p25g_tx27
	assign mgt_txusrclk[7][2] = pr25g_7_mmcm_clk; // p25g_tx28
	assign mgt_txusrclk[7][3] = pr25g_7_mmcm_clk; // p25g_tx29
	pr25g_mmcm pr25g_8_inst (.O(pr25g_8_mmcm_clk), .I(mgt_gty_tx_if[8][0].txoutclk));
	assign mgt_txusrclk[8][0] = pr25g_8_mmcm_clk; // p25g_tx30
	assign mgt_txusrclk[8][1] = pr25g_8_mmcm_clk; // p25g_tx31
	assign mgt_txusrclk[8][2] = pr25g_8_mmcm_clk; // p25g_tx32
	assign mgt_txusrclk[8][3] = pr25g_8_mmcm_clk; // p25g_tx33
	pr25g_mmcm pr25g_1_inst (.O(pr25g_1_mmcm_clk), .I(mgt_gty_tx_if[1][1].txoutclk));
	assign mgt_txusrclk[1][1] = pr25g_1_mmcm_clk; // p25g_tx3
	assign mgt_txusrclk[1][2] = pr25g_1_mmcm_clk; // p25g_tx4
	assign mgt_txusrclk[1][3] = pr25g_1_mmcm_clk; // p25g_tx5
	pr25g_mmcm pr25g_24_inst (.O(pr25g_24_mmcm_clk), .I(mgt_gty_tx_if[24][0].txoutclk));
	assign mgt_txusrclk[24][0] = pr25g_24_mmcm_clk; // p25g_tx94
	assign mgt_txusrclk[24][1] = pr25g_24_mmcm_clk; // p25g_tx95
	assign mgt_txusrclk[24][2] = pr25g_24_mmcm_clk; // p25g_tx96
	assign mgt_txusrclk[24][3] = pr25g_24_mmcm_clk; // p25g_tx97
	pr25g_mmcm pr25g_2_inst (.O(pr25g_2_mmcm_clk), .I(mgt_gty_tx_if[2][0].txoutclk));
	assign mgt_txusrclk[2][0] = pr25g_2_mmcm_clk; // p25g_tx6
	assign mgt_txusrclk[2][1] = pr25g_2_mmcm_clk; // p25g_tx7
	assign mgt_txusrclk[2][2] = pr25g_2_mmcm_clk; // p25g_tx8
	assign mgt_txusrclk[2][3] = pr25g_2_mmcm_clk; // p25g_tx9
	pr25g_mmcm pr25g_23_inst (.O(pr25g_23_mmcm_clk), .I(mgt_gty_tx_if[23][0].txoutclk));
	assign mgt_txusrclk[23][0] = pr25g_23_mmcm_clk; // p25g_tx90
	assign mgt_txusrclk[23][1] = pr25g_23_mmcm_clk; // p25g_tx91
	assign mgt_txusrclk[23][2] = pr25g_23_mmcm_clk; // p25g_tx92
	assign mgt_txusrclk[23][3] = pr25g_23_mmcm_clk; // p25g_tx93
	pr25g_mmcm pr25g_3_inst (.O(pr25g_3_mmcm_clk), .I(mgt_gty_tx_if[3][0].txoutclk));
	assign mgt_txusrclk[3][0] = pr25g_3_mmcm_clk; // p25g_tx10
	assign mgt_txusrclk[3][1] = pr25g_3_mmcm_clk; // p25g_tx11
	assign mgt_txusrclk[3][2] = pr25g_3_mmcm_clk; // p25g_tx12
	assign mgt_txusrclk[3][3] = pr25g_3_mmcm_clk; // p25g_tx13
	pr25g_mmcm pr25g_22_inst (.O(pr25g_22_mmcm_clk), .I(mgt_gty_tx_if[22][0].txoutclk));
	assign mgt_txusrclk[22][0] = pr25g_22_mmcm_clk; // p25g_tx86
	assign mgt_txusrclk[22][1] = pr25g_22_mmcm_clk; // p25g_tx87
	assign mgt_txusrclk[22][2] = pr25g_22_mmcm_clk; // p25g_tx88
	assign mgt_txusrclk[22][3] = pr25g_22_mmcm_clk; // p25g_tx89
	pr25g_mmcm pr25g_4_inst (.O(pr25g_4_mmcm_clk), .I(mgt_gty_tx_if[4][0].txoutclk));
	assign mgt_txusrclk[4][0] = pr25g_4_mmcm_clk; // p25g_tx14
	assign mgt_txusrclk[4][1] = pr25g_4_mmcm_clk; // p25g_tx15
	assign mgt_txusrclk[4][2] = pr25g_4_mmcm_clk; // p25g_tx16
	assign mgt_txusrclk[4][3] = pr25g_4_mmcm_clk; // p25g_tx17
	pr25g_mmcm pr25g_21_inst (.O(pr25g_21_mmcm_clk), .I(mgt_gty_tx_if[21][0].txoutclk));
	assign mgt_txusrclk[21][0] = pr25g_21_mmcm_clk; // p25g_tx82
	assign mgt_txusrclk[21][1] = pr25g_21_mmcm_clk; // p25g_tx83
	assign mgt_txusrclk[21][2] = pr25g_21_mmcm_clk; // p25g_tx84
	assign mgt_txusrclk[21][3] = pr25g_21_mmcm_clk; // p25g_tx85
	pr25g_mmcm pr25g_20_inst (.O(pr25g_20_mmcm_clk), .I(mgt_gty_tx_if[20][0].txoutclk));
	assign mgt_txusrclk[20][0] = pr25g_20_mmcm_clk; // p25g_tx78
	assign mgt_txusrclk[20][1] = pr25g_20_mmcm_clk; // p25g_tx79
	assign mgt_txusrclk[20][2] = pr25g_20_mmcm_clk; // p25g_tx80
	assign mgt_txusrclk[20][3] = pr25g_20_mmcm_clk; // p25g_tx81
	pr25g_mmcm pr25g_0_inst (.O(pr25g_0_mmcm_clk), .I(mgt_gty_tx_if[0][1].txoutclk));
	assign mgt_txusrclk[0][1] = pr25g_0_mmcm_clk; // p25g_tx0
	assign mgt_txusrclk[0][2] = pr25g_0_mmcm_clk; // p25g_tx1
	assign mgt_txusrclk[0][3] = pr25g_0_mmcm_clk; // p25g_tx2
	pr25g_mmcm pr25g_19_inst (.O(pr25g_19_mmcm_clk), .I(mgt_gty_tx_if[19][0].txoutclk));
	assign mgt_txusrclk[19][0] = pr25g_19_mmcm_clk; // p25g_tx74
	assign mgt_txusrclk[19][1] = pr25g_19_mmcm_clk; // p25g_tx75
	assign mgt_txusrclk[19][2] = pr25g_19_mmcm_clk; // p25g_tx76
	assign mgt_txusrclk[19][3] = pr25g_19_mmcm_clk; // p25g_tx77
	pr25g_mmcm pr25g_18_inst (.O(pr25g_18_mmcm_clk), .I(mgt_gty_tx_if[18][0].txoutclk));
	assign mgt_txusrclk[18][0] = pr25g_18_mmcm_clk; // p25g_tx70
	assign mgt_txusrclk[18][1] = pr25g_18_mmcm_clk; // p25g_tx71
	assign mgt_txusrclk[18][2] = pr25g_18_mmcm_clk; // p25g_tx72
	assign mgt_txusrclk[18][3] = pr25g_18_mmcm_clk; // p25g_tx73
	pr25g_mmcm pr25g_17_inst (.O(pr25g_17_mmcm_clk), .I(mgt_gty_tx_if[17][0].txoutclk));
	assign mgt_txusrclk[17][0] = pr25g_17_mmcm_clk; // p25g_tx66
	assign mgt_txusrclk[17][1] = pr25g_17_mmcm_clk; // p25g_tx67
	assign mgt_txusrclk[17][2] = pr25g_17_mmcm_clk; // p25g_tx68
	assign mgt_txusrclk[17][3] = pr25g_17_mmcm_clk; // p25g_tx69
	pr25g_mmcm pr25g_16_inst (.O(pr25g_16_mmcm_clk), .I(mgt_gty_tx_if[16][0].txoutclk));
	assign mgt_txusrclk[16][0] = pr25g_16_mmcm_clk; // p25g_tx62
	assign mgt_txusrclk[16][1] = pr25g_16_mmcm_clk; // p25g_tx63
	assign mgt_txusrclk[16][2] = pr25g_16_mmcm_clk; // p25g_tx64
	assign mgt_txusrclk[16][3] = pr25g_16_mmcm_clk; // p25g_tx65
	pr25g_mmcm pr25g_15_inst (.O(pr25g_15_mmcm_clk), .I(mgt_gty_tx_if[15][0].txoutclk));
	assign mgt_txusrclk[15][0] = pr25g_15_mmcm_clk; // p25g_tx58
	assign mgt_txusrclk[15][1] = pr25g_15_mmcm_clk; // p25g_tx59
	assign mgt_txusrclk[15][2] = pr25g_15_mmcm_clk; // p25g_tx60
	assign mgt_txusrclk[15][3] = pr25g_15_mmcm_clk; // p25g_tx61
	pr25g_mmcm pr25g_14_inst (.O(pr25g_14_mmcm_clk), .I(mgt_gty_tx_if[14][0].txoutclk));
	assign mgt_txusrclk[14][0] = pr25g_14_mmcm_clk; // p25g_tx54
	assign mgt_txusrclk[14][1] = pr25g_14_mmcm_clk; // p25g_tx55
	assign mgt_txusrclk[14][2] = pr25g_14_mmcm_clk; // p25g_tx56
	assign mgt_txusrclk[14][3] = pr25g_14_mmcm_clk; // p25g_tx57
endmodule
