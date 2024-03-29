// This code was automatically generated by FPGA MGT Builder
// Timestamp: 2020-12-17 15:40:24.702
// User: madorsky
// FPGA MGT builder homepage: <link>

`include "mgt_gth_interfaces.sv"
module assign_mgt_gth_tx
(
	mgt_gth_tx.out dest,
	mgt_gth_tx.in src
);
	 assign src.gthtxn = dest.gthtxn;
	 assign src.gthtxp = dest.gthtxp;
	 assign dest.txctrl0 = src.txctrl0;
	 assign dest.txctrl1 = src.txctrl1;
	 assign dest.txctrl2 = src.txctrl2;
	 assign dest.txdata = src.txdata;
	 assign dest.txheader = src.txheader;
	 assign src.txoutclk = dest.txoutclk;
	 assign dest.txsequence = src.txsequence;
endmodule
