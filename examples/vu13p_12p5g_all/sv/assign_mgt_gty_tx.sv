// This code was automatically generated by FPGA MGT Builder
// Timestamp: 2022-09-15 16:14:34.006
// User: madorsky
// FPGA MGT builder homepage: <link>

`include "mgt_gty_interfaces.sv"
module assign_mgt_gty_tx
(
	mgt_gty_tx.out dest,
	mgt_gty_tx.in src
);
	 assign src.gtytxn = dest.gtytxn;
	 assign src.gtytxp = dest.gtytxp;
	 assign dest.txctrl0 = src.txctrl0;
	 assign dest.txctrl1 = src.txctrl1;
	 assign dest.txctrl2 = src.txctrl2;
	 assign dest.txdata = src.txdata;
	 assign dest.txheader = src.txheader;
	 assign src.txoutclk = dest.txoutclk;
	 assign dest.txsequence = src.txsequence;
endmodule
