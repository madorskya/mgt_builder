// This code was automatically generated by FPGA MGT Builder
// Timestamp: 2021-02-19 13:07:32.072
// User: madorsky
// FPGA MGT builder homepage: <link>

`include "mgt_interfaces.sv"
module assign_mgt_tx
(
	mgt_tx.out dest,
	mgt_tx.in src
);
	 assign dest.txcharisk = src.txcharisk;
	 assign dest.txphdlytstclk = src.txphdlytstclk;
	 assign src.gthtxn = dest.gthtxn;
	 assign src.gthtxp = dest.gthtxp;
	 assign src.txoutclk = dest.txoutclk;
	 assign src.txgearboxready = dest.txgearboxready;
	 assign dest.txheader = src.txheader;
	 assign dest.txsequence = src.txsequence;
	 assign dest.txstartseq = src.txstartseq;
	 assign dest.txchardispmode = src.txchardispmode;
	 assign dest.txchardispval = src.txchardispval;
	 assign dest.txdata = src.txdata;
endmodule
