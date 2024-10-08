// This code was automatically generated by FPGA MGT Builder
// Timestamp: 2024-08-15 16:10:40.534
// User: agreshil
// FPGA MGT builder homepage: https://github.com/madorskya/mgt_builder

`include "drp_interface.sv"
`include "mgt_gty_interfaces.sv"
module emtf_vu13p_gty_serial_io
(
	drp.in drp_gty_fif,
	input drpclk,
	input [47:0] refclk_p,
	input [47:0] refclk_n,
	mgt_gty_rx.in tmb_outer [34:0],
	mgt_gty_rx.in rpc [10:0],
	mgt_gty_rx.in tmb_inner [37:0],
	mgt_gty_rx.in gem [12:0],
	mgt_gty_tx.in daq [0:0],
	mgt_gty_tx.in gmt [17:0],
	output cstlp_23_mmcm_clk,
	output slink_16_mmcm_clk,
	output cstlp_17_mmcm_clk,
	output cstlp_18_mmcm_clk,
	output cstlp_20_mmcm_clk,
	output cstlp_22_mmcm_clk,
	output cstlp_21_mmcm_clk,
	output [47:0] refclk,
	output [47:0] refclk_odiv
);
	localparam quadn = 27;
	drp #(.AW(13)) drp_gty_qif [quadn-1:0]();
	drp_mux_fpga #(.N(quadn), .AW_QUAD(13)) drpm (drp_gty_qif, drp_gty_fif, drpclk);

	mgt_gty_tx mgt_gty_tx_if [quadn-1:0][3:0]();
	mgt_gty_rx mgt_gty_rx_if [quadn-1:0][3:0]();
	wire [ 3:0] mgt_gtrefclk0 [quadn-1:0];
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
				.mgt_gtrefclk0              (mgt_gtrefclk0              [gi]),
				.mgt_txlatclk               (mgt_txlatclk              ),
				.mgt_txusrclk               (mgt_txusrclk               [gi]),
				.com_gtrefclk00             (com_gtrefclk00             [gi]),
				.com_gtrefclk01             (com_gtrefclk01             [gi])
			);
		end
		for (gi = 0; gi < 48; gi++)
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
				.ODIV2 (refclk_odiv [gi])	
			);		
		end
	endgenerate
	assign_mgt_gty_rx ari0 (mgt_gty_rx_if [0][0], tmb_outer [6]);
	assign_mgt_gty_rx ari1 (mgt_gty_rx_if [0][1], tmb_outer [3]);
	assign_mgt_gty_rx ari2 (mgt_gty_rx_if [0][2], tmb_inner [10]);
	assign_mgt_gty_rx ari3 (mgt_gty_rx_if [0][3], tmb_outer [7]);
	assign_mgt_gty_rx ari4 (mgt_gty_rx_if [1][0], tmb_outer [2]);
	assign_mgt_gty_rx ari5 (mgt_gty_rx_if [1][1], tmb_inner [11]);
	assign_mgt_gty_rx ari6 (mgt_gty_rx_if [1][2], tmb_outer [4]);
	assign_mgt_gty_rx ari7 (mgt_gty_rx_if [1][3], tmb_outer [5]);
	assign_mgt_gty_rx ari8 (mgt_gty_rx_if [2][0], tmb_outer [11]);
	assign_mgt_gty_rx ari9 (mgt_gty_rx_if [2][1], tmb_inner [6]);
	assign_mgt_gty_rx ari10 (mgt_gty_rx_if [2][2], tmb_outer [9]);
	assign_mgt_gty_rx ari11 (mgt_gty_rx_if [2][3], tmb_outer [8]);
	assign_mgt_gty_rx ari12 (mgt_gty_rx_if [3][0], tmb_inner [9]);
	assign_mgt_gty_rx ari13 (mgt_gty_rx_if [3][1], tmb_outer [10]);
	assign_mgt_gty_rx ari14 (mgt_gty_rx_if [3][2], tmb_inner [7]);
	assign_mgt_gty_rx ari15 (mgt_gty_rx_if [3][3], tmb_inner [8]);
	assign_mgt_gty_rx ari16 (mgt_gty_rx_if [4][0], tmb_outer [1]);
	assign_mgt_gty_rx ari17 (mgt_gty_rx_if [4][1], tmb_outer [0]);
	assign_mgt_gty_rx ari18 (mgt_gty_rx_if [4][2], tmb_inner [5]);
	assign_mgt_gty_rx ari19 (mgt_gty_rx_if [4][3], tmb_inner [4]);
	assign_mgt_gty_rx ari20 (mgt_gty_rx_if [5][0], tmb_inner [3]);
	assign_mgt_gty_rx ari21 (mgt_gty_rx_if [5][1], tmb_inner [2]);
	assign_mgt_gty_rx ari22 (mgt_gty_rx_if [5][2], tmb_inner [1]);
	assign_mgt_gty_rx ari23 (mgt_gty_rx_if [5][3], tmb_inner [0]);
	assign_mgt_gty_rx ari24 (mgt_gty_rx_if [6][0], tmb_outer [14]);
	assign_mgt_gty_rx ari25 (mgt_gty_rx_if [6][1], tmb_outer [15]);
	assign_mgt_gty_rx ari26 (mgt_gty_rx_if [6][2], tmb_outer [16]);
	assign_mgt_gty_rx ari27 (mgt_gty_rx_if [6][3], tmb_outer [17]);
	assign_mgt_gty_rx ari28 (mgt_gty_rx_if [7][0], tmb_inner [12]);
	assign_mgt_gty_rx ari29 (mgt_gty_rx_if [7][1], tmb_inner [13]);
	assign_mgt_gty_rx ari30 (mgt_gty_rx_if [7][2], tmb_inner [14]);
	assign_mgt_gty_rx ari31 (mgt_gty_rx_if [7][3], tmb_inner [15]);
	assign_mgt_gty_rx ari32 (mgt_gty_rx_if [8][0], tmb_inner [21]);
	assign_mgt_gty_rx ari33 (mgt_gty_rx_if [8][1], tmb_inner [20]);
	assign_mgt_gty_rx ari34 (mgt_gty_rx_if [8][2], tmb_inner [19]);
	assign_mgt_gty_rx ari35 (mgt_gty_rx_if [8][3], tmb_inner [18]);
	assign_mgt_gty_rx ari36 (mgt_gty_rx_if [9][0], tmb_outer [13]);
	assign_mgt_gty_rx ari37 (mgt_gty_rx_if [9][1], tmb_outer [12]);
	assign_mgt_gty_rx ari38 (mgt_gty_rx_if [9][2], tmb_inner [17]);
	assign_mgt_gty_rx ari39 (mgt_gty_rx_if [9][3], tmb_inner [16]);
	assign_mgt_gty_rx ari40 (mgt_gty_rx_if [10][0], tmb_inner [24]);
	assign_mgt_gty_rx ari41 (mgt_gty_rx_if [10][1], tmb_inner [25]);
	assign_mgt_gty_rx ari42 (mgt_gty_rx_if [10][2], tmb_inner [26]);
	assign_mgt_gty_rx ari43 (mgt_gty_rx_if [10][3], tmb_inner [27]);
	assign_mgt_gty_rx ari44 (mgt_gty_rx_if [11][0], tmb_inner [22]);
	assign_mgt_gty_rx ari45 (mgt_gty_rx_if [11][1], tmb_inner [23]);
	assign_mgt_gty_rx ari46 (mgt_gty_rx_if [11][2], tmb_outer [18]);
	assign_mgt_gty_rx ari47 (mgt_gty_rx_if [11][3], tmb_outer [19]);
	assign_mgt_gty_rx ari48 (mgt_gty_rx_if [12][0], tmb_outer [25]);
	assign_mgt_gty_rx ari49 (mgt_gty_rx_if [12][1], tmb_outer [20]);
	assign_mgt_gty_rx ari50 (mgt_gty_rx_if [12][2], tmb_outer [21]);
	assign_mgt_gty_rx ari51 (mgt_gty_rx_if [12][3], tmb_outer [22]);
	assign_mgt_gty_rx ari52 (mgt_gty_rx_if [13][0], tmb_outer [23]);
	assign_mgt_gty_rx ari53 (mgt_gty_rx_if [13][1], tmb_inner [28]);
	assign_mgt_gty_rx ari54 (mgt_gty_rx_if [13][2], tmb_inner [29]);
	assign_mgt_gty_rx ari55 (mgt_gty_rx_if [13][3], tmb_outer [24]);
	assign_mgt_gty_rx ari56 (mgt_gty_rx_if [14][1], rpc [8]);
	assign_mgt_gty_tx ati57 (mgt_gty_tx_if [14][3], daq [0]);
	assign_mgt_gty_rx ari58 (mgt_gty_rx_if [15][0], rpc [9]);
	assign_mgt_gty_rx ari59 (mgt_gty_rx_if [15][3], rpc [10]);
	assign_mgt_gty_tx ati59 (mgt_gty_tx_if [15][3], gmt [17]);
	assign_mgt_gty_rx ari60 (mgt_gty_rx_if [16][0], rpc [2]);
	assign_mgt_gty_tx ati60 (mgt_gty_tx_if [16][0], gmt [16]);
	assign_mgt_gty_rx ari61 (mgt_gty_rx_if [16][1], rpc [5]);
	assign_mgt_gty_rx ari62 (mgt_gty_rx_if [16][2], rpc [0]);
	assign_mgt_gty_tx ati62 (mgt_gty_tx_if [16][2], gmt [14]);
	assign_mgt_gty_rx ari63 (mgt_gty_rx_if [16][3], rpc [1]);
	assign_mgt_gty_tx ati63 (mgt_gty_tx_if [16][3], gmt [15]);
	assign_mgt_gty_rx ari64 (mgt_gty_rx_if [17][0], rpc [6]);
	assign_mgt_gty_rx ari65 (mgt_gty_rx_if [17][1], rpc [3]);
	assign_mgt_gty_rx ari66 (mgt_gty_rx_if [17][2], rpc [4]);
	assign_mgt_gty_rx ari67 (mgt_gty_rx_if [17][3], rpc [7]);
	assign_mgt_gty_rx ari68 (mgt_gty_rx_if [18][0], gem [9]);
	assign_mgt_gty_tx ati68 (mgt_gty_tx_if [18][0], gmt [9]);
	assign_mgt_gty_rx ari69 (mgt_gty_rx_if [18][1], gem [8]);
	assign_mgt_gty_tx ati69 (mgt_gty_tx_if [18][1], gmt [8]);
	assign_mgt_gty_rx ari70 (mgt_gty_rx_if [18][2], gem [11]);
	assign_mgt_gty_tx ati70 (mgt_gty_tx_if [18][2], gmt [11]);
	assign_mgt_gty_rx ari71 (mgt_gty_rx_if [18][3], gem [10]);
	assign_mgt_gty_tx ati71 (mgt_gty_tx_if [18][3], gmt [10]);
	assign_mgt_gty_tx ati72 (mgt_gty_tx_if [19][0], gmt [13]);
	assign_mgt_gty_rx ari73 (mgt_gty_rx_if [19][1], gem [12]);
	assign_mgt_gty_tx ati73 (mgt_gty_tx_if [19][1], gmt [12]);
	assign_mgt_gty_rx ari74 (mgt_gty_rx_if [20][0], gem [2]);
	assign_mgt_gty_tx ati74 (mgt_gty_tx_if [20][0], gmt [2]);
	assign_mgt_gty_rx ari75 (mgt_gty_rx_if [20][1], gem [3]);
	assign_mgt_gty_tx ati75 (mgt_gty_tx_if [20][1], gmt [3]);
	assign_mgt_gty_rx ari76 (mgt_gty_rx_if [20][2], gem [0]);
	assign_mgt_gty_tx ati76 (mgt_gty_tx_if [20][2], gmt [0]);
	assign_mgt_gty_rx ari77 (mgt_gty_rx_if [20][3], gem [1]);
	assign_mgt_gty_tx ati77 (mgt_gty_tx_if [20][3], gmt [1]);
	assign_mgt_gty_rx ari78 (mgt_gty_rx_if [21][0], gem [6]);
	assign_mgt_gty_tx ati78 (mgt_gty_tx_if [21][0], gmt [6]);
	assign_mgt_gty_rx ari79 (mgt_gty_rx_if [21][1], gem [7]);
	assign_mgt_gty_tx ati79 (mgt_gty_tx_if [21][1], gmt [7]);
	assign_mgt_gty_rx ari80 (mgt_gty_rx_if [21][2], gem [4]);
	assign_mgt_gty_tx ati80 (mgt_gty_tx_if [21][2], gmt [4]);
	assign_mgt_gty_rx ari81 (mgt_gty_rx_if [21][3], gem [5]);
	assign_mgt_gty_tx ati81 (mgt_gty_tx_if [21][3], gmt [5]);
	assign_mgt_gty_rx ari82 (mgt_gty_rx_if [22][2], tmb_outer [34]);
	assign_mgt_gty_rx ari83 (mgt_gty_rx_if [23][0], tmb_inner [31]);
	assign_mgt_gty_rx ari84 (mgt_gty_rx_if [23][1], tmb_inner [30]);
	assign_mgt_gty_rx ari85 (mgt_gty_rx_if [23][2], tmb_inner [33]);
	assign_mgt_gty_rx ari86 (mgt_gty_rx_if [23][3], tmb_inner [32]);
	assign_mgt_gty_rx ari87 (mgt_gty_rx_if [24][0], tmb_outer [31]);
	assign_mgt_gty_rx ari88 (mgt_gty_rx_if [24][1], tmb_outer [30]);
	assign_mgt_gty_rx ari89 (mgt_gty_rx_if [24][2], tmb_outer [33]);
	assign_mgt_gty_rx ari90 (mgt_gty_rx_if [24][3], tmb_outer [32]);
	assign_mgt_gty_rx ari91 (mgt_gty_rx_if [25][0], tmb_outer [28]);
	assign_mgt_gty_rx ari92 (mgt_gty_rx_if [25][1], tmb_inner [35]);
	assign_mgt_gty_rx ari93 (mgt_gty_rx_if [25][2], tmb_outer [26]);
	assign_mgt_gty_rx ari94 (mgt_gty_rx_if [25][3], tmb_inner [37]);
	assign_mgt_gty_rx ari95 (mgt_gty_rx_if [26][0], tmb_inner [36]);
	assign_mgt_gty_rx ari96 (mgt_gty_rx_if [26][1], tmb_outer [27]);
	assign_mgt_gty_rx ari97 (mgt_gty_rx_if [26][2], tmb_inner [34]);
	assign_mgt_gty_rx ari98 (mgt_gty_rx_if [26][3], tmb_outer [29]);
	assign mgt_gtrefclk0 [0][0] = refclk [33]; // tmb_outer 6
	assign mgt_gtrefclk0 [0][1] = refclk [33]; // tmb_outer 3
	assign mgt_gtrefclk0 [0][2] = refclk [33]; // tmb_inner 10
	assign mgt_gtrefclk0 [0][3] = refclk [33]; // tmb_outer 7
	assign mgt_gtrefclk0 [1][0] = refclk [33]; // tmb_outer 2
	assign mgt_gtrefclk0 [1][1] = refclk [33]; // tmb_inner 11
	assign mgt_gtrefclk0 [1][2] = refclk [33]; // tmb_outer 4
	assign mgt_gtrefclk0 [1][3] = refclk [33]; // tmb_outer 5
	assign mgt_gtrefclk0 [2][0] = refclk [33]; // tmb_outer 11
	assign mgt_gtrefclk0 [2][1] = refclk [33]; // tmb_inner 6
	assign mgt_gtrefclk0 [2][2] = refclk [33]; // tmb_outer 9
	assign mgt_gtrefclk0 [2][3] = refclk [33]; // tmb_outer 8
	assign mgt_gtrefclk0 [3][0] = refclk [33]; // tmb_inner 9
	assign mgt_gtrefclk0 [3][1] = refclk [33]; // tmb_outer 10
	assign mgt_gtrefclk0 [3][2] = refclk [33]; // tmb_inner 7
	assign mgt_gtrefclk0 [3][3] = refclk [33]; // tmb_inner 8
	assign mgt_gtrefclk0 [4][0] = refclk [34]; // tmb_outer 1
	assign mgt_gtrefclk0 [4][1] = refclk [34]; // tmb_outer 0
	assign mgt_gtrefclk0 [4][2] = refclk [34]; // tmb_inner 5
	assign mgt_gtrefclk0 [4][3] = refclk [34]; // tmb_inner 4
	assign mgt_gtrefclk0 [5][0] = refclk [34]; // tmb_inner 3
	assign mgt_gtrefclk0 [5][1] = refclk [34]; // tmb_inner 2
	assign mgt_gtrefclk0 [5][2] = refclk [34]; // tmb_inner 1
	assign mgt_gtrefclk0 [5][3] = refclk [34]; // tmb_inner 0
	assign mgt_gtrefclk0 [6][0] = refclk [37]; // tmb_outer 14
	assign mgt_gtrefclk0 [6][1] = refclk [37]; // tmb_outer 15
	assign mgt_gtrefclk0 [6][2] = refclk [37]; // tmb_outer 16
	assign mgt_gtrefclk0 [6][3] = refclk [37]; // tmb_outer 17
	assign mgt_gtrefclk0 [7][0] = refclk [37]; // tmb_inner 12
	assign mgt_gtrefclk0 [7][1] = refclk [37]; // tmb_inner 13
	assign mgt_gtrefclk0 [7][2] = refclk [37]; // tmb_inner 14
	assign mgt_gtrefclk0 [7][3] = refclk [37]; // tmb_inner 15
	assign mgt_gtrefclk0 [8][0] = refclk [37]; // tmb_inner 21
	assign mgt_gtrefclk0 [8][1] = refclk [37]; // tmb_inner 20
	assign mgt_gtrefclk0 [8][2] = refclk [37]; // tmb_inner 19
	assign mgt_gtrefclk0 [8][3] = refclk [37]; // tmb_inner 18
	assign mgt_gtrefclk0 [9][0] = refclk [37]; // tmb_outer 13
	assign mgt_gtrefclk0 [9][1] = refclk [37]; // tmb_outer 12
	assign mgt_gtrefclk0 [9][2] = refclk [37]; // tmb_inner 17
	assign mgt_gtrefclk0 [9][3] = refclk [37]; // tmb_inner 16
	assign mgt_gtrefclk0 [10][0] = refclk [39]; // tmb_inner 24
	assign mgt_gtrefclk0 [10][1] = refclk [39]; // tmb_inner 25
	assign mgt_gtrefclk0 [10][2] = refclk [39]; // tmb_inner 26
	assign mgt_gtrefclk0 [10][3] = refclk [39]; // tmb_inner 27
	assign mgt_gtrefclk0 [11][0] = refclk [39]; // tmb_inner 22
	assign mgt_gtrefclk0 [11][1] = refclk [39]; // tmb_inner 23
	assign mgt_gtrefclk0 [11][2] = refclk [39]; // tmb_outer 18
	assign mgt_gtrefclk0 [11][3] = refclk [39]; // tmb_outer 19
	assign mgt_gtrefclk0 [12][0] = refclk [39]; // tmb_outer 25
	assign mgt_gtrefclk0 [12][1] = refclk [39]; // tmb_outer 20
	assign mgt_gtrefclk0 [12][2] = refclk [39]; // tmb_outer 21
	assign mgt_gtrefclk0 [12][3] = refclk [39]; // tmb_outer 22
	assign mgt_gtrefclk0 [13][0] = refclk [39]; // tmb_outer 23
	assign mgt_gtrefclk0 [13][1] = refclk [39]; // tmb_inner 28
	assign mgt_gtrefclk0 [13][2] = refclk [39]; // tmb_inner 29
	assign mgt_gtrefclk0 [13][3] = refclk [39]; // tmb_outer 24
	assign com_gtrefclk00 [14] = refclk [16]; // rpc 8
	assign com_gtrefclk00 [14] = refclk [16]; // daq 0
	assign com_gtrefclk00 [15] = refclk [17]; // rpc 9
	assign com_gtrefclk00 [15] = refclk [17]; // gmt 17
	assign com_gtrefclk00 [16] = refclk [18]; // gmt 16
	assign com_gtrefclk00 [16] = refclk [18]; // rpc 5
	assign com_gtrefclk00 [16] = refclk [18]; // gmt 14
	assign com_gtrefclk00 [16] = refclk [18]; // gmt 15
	assign com_gtrefclk00 [17] = refclk [19]; // rpc 6
	assign com_gtrefclk00 [17] = refclk [19]; // rpc 3
	assign com_gtrefclk00 [17] = refclk [19]; // rpc 4
	assign com_gtrefclk00 [17] = refclk [19]; // rpc 7
	assign com_gtrefclk00 [18] = refclk [20]; // gmt 9
	assign com_gtrefclk00 [18] = refclk [20]; // gmt 8
	assign com_gtrefclk00 [18] = refclk [20]; // gmt 11
	assign com_gtrefclk00 [18] = refclk [20]; // gmt 10
	assign com_gtrefclk00 [19] = refclk [21]; // gmt 13
	assign com_gtrefclk00 [19] = refclk [21]; // gmt 12
	assign com_gtrefclk00 [20] = refclk [22]; // gmt 2
	assign com_gtrefclk00 [20] = refclk [22]; // gmt 3
	assign com_gtrefclk00 [20] = refclk [22]; // gmt 0
	assign com_gtrefclk00 [20] = refclk [22]; // gmt 1
	assign com_gtrefclk00 [21] = refclk [23]; // gmt 6
	assign com_gtrefclk00 [21] = refclk [23]; // gmt 7
	assign com_gtrefclk00 [21] = refclk [23]; // gmt 4
	assign com_gtrefclk00 [21] = refclk [23]; // gmt 5
	assign mgt_gtrefclk0 [22][2] = refclk [45]; // tmb_outer 34
	assign mgt_gtrefclk0 [23][0] = refclk [47]; // tmb_inner 31
	assign mgt_gtrefclk0 [23][1] = refclk [47]; // tmb_inner 30
	assign mgt_gtrefclk0 [23][2] = refclk [47]; // tmb_inner 33
	assign mgt_gtrefclk0 [23][3] = refclk [47]; // tmb_inner 32
	assign mgt_gtrefclk0 [24][0] = refclk [47]; // tmb_outer 31
	assign mgt_gtrefclk0 [24][1] = refclk [47]; // tmb_outer 30
	assign mgt_gtrefclk0 [24][2] = refclk [47]; // tmb_outer 33
	assign mgt_gtrefclk0 [24][3] = refclk [47]; // tmb_outer 32
	assign mgt_gtrefclk0 [25][0] = refclk [47]; // tmb_outer 28
	assign mgt_gtrefclk0 [25][1] = refclk [47]; // tmb_inner 35
	assign mgt_gtrefclk0 [25][2] = refclk [47]; // tmb_outer 26
	assign mgt_gtrefclk0 [25][3] = refclk [47]; // tmb_inner 37
	assign mgt_gtrefclk0 [26][0] = refclk [47]; // tmb_inner 36
	assign mgt_gtrefclk0 [26][1] = refclk [47]; // tmb_outer 27
	assign mgt_gtrefclk0 [26][2] = refclk [47]; // tmb_inner 34
	assign mgt_gtrefclk0 [26][3] = refclk [47]; // tmb_outer 29
	cstlp_mmcm cstlp_23_inst (.O(cstlp_23_mmcm_clk), .I(mgt_gty_tx_if[21][0].txoutclk));
	assign mgt_txusrclk[21][0] = cstlp_23_mmcm_clk; // gmt6
	assign mgt_txusrclk[21][1] = cstlp_23_mmcm_clk; // gmt7
	assign mgt_txusrclk[21][2] = cstlp_23_mmcm_clk; // gmt4
	assign mgt_txusrclk[21][3] = cstlp_23_mmcm_clk; // gmt5
	slink_mmcm slink_16_inst (.O(slink_16_mmcm_clk), .I(mgt_gty_tx_if[14][3].txoutclk));
	assign mgt_txusrclk[14][3] = slink_16_mmcm_clk; // daq0
	cstlp_mmcm cstlp_17_inst (.O(cstlp_17_mmcm_clk), .I(mgt_gty_tx_if[15][3].txoutclk));
	assign mgt_txusrclk[15][3] = cstlp_17_mmcm_clk; // gmt17
	cstlp_mmcm cstlp_18_inst (.O(cstlp_18_mmcm_clk), .I(mgt_gty_tx_if[16][0].txoutclk));
	assign mgt_txusrclk[16][0] = cstlp_18_mmcm_clk; // gmt16
	assign mgt_txusrclk[16][2] = cstlp_18_mmcm_clk; // gmt14
	assign mgt_txusrclk[16][3] = cstlp_18_mmcm_clk; // gmt15
	cstlp_mmcm cstlp_20_inst (.O(cstlp_20_mmcm_clk), .I(mgt_gty_tx_if[18][0].txoutclk));
	assign mgt_txusrclk[18][0] = cstlp_20_mmcm_clk; // gmt9
	assign mgt_txusrclk[18][1] = cstlp_20_mmcm_clk; // gmt8
	assign mgt_txusrclk[18][2] = cstlp_20_mmcm_clk; // gmt11
	assign mgt_txusrclk[18][3] = cstlp_20_mmcm_clk; // gmt10
	cstlp_mmcm cstlp_22_inst (.O(cstlp_22_mmcm_clk), .I(mgt_gty_tx_if[20][0].txoutclk));
	assign mgt_txusrclk[20][0] = cstlp_22_mmcm_clk; // gmt2
	assign mgt_txusrclk[20][1] = cstlp_22_mmcm_clk; // gmt3
	assign mgt_txusrclk[20][2] = cstlp_22_mmcm_clk; // gmt0
	assign mgt_txusrclk[20][3] = cstlp_22_mmcm_clk; // gmt1
	cstlp_mmcm cstlp_21_inst (.O(cstlp_21_mmcm_clk), .I(mgt_gty_tx_if[19][0].txoutclk));
	assign mgt_txusrclk[19][0] = cstlp_21_mmcm_clk; // gmt13
	assign mgt_txusrclk[19][1] = cstlp_21_mmcm_clk; // gmt12
endmodule
