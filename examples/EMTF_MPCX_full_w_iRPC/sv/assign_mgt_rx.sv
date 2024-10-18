// This code was automatically generated by FPGA MGT Builder
// Timestamp: 2024-10-11 09:31:28.753
// User: patrick
// FPGA MGT builder homepage: https://github.com/madorskya/mgt_builder

`include "mgt_interfaces.sv"
module assign_mgt_rx
(
	mgt_rx.out dest,
	mgt_rx.in src
);
	 assign src.rxchariscomma = dest.rxchariscomma;
	 assign src.rxcharisk = dest.rxcharisk;
	 assign src.rxdisperr = dest.rxdisperr;
	 assign src.rxnotintable = dest.rxnotintable;
	 assign dest.gthrxn = src.gthrxn;
	 assign dest.gthrxp = src.gthrxp;
	 assign src.rxcommadet = dest.rxcommadet;
	 assign dest.rxslide = src.rxslide;
	 assign src.rxchanbondseq = dest.rxchanbondseq;
	 assign src.rxchanisaligned = dest.rxchanisaligned;
	 assign src.rxchanrealign = dest.rxchanrealign;
	 assign dest.rxchbondi = src.rxchbondi;
	 assign src.rxchbondo = dest.rxchbondo;
	 assign dest.rxsyncin = src.rxsyncin;
	 assign src.rxsyncout = dest.rxsyncout;
	 assign src.rxclkcorcnt = dest.rxclkcorcnt;
	 assign src.rxoutclk = dest.rxoutclk;
	 assign src.rxdatavalid = dest.rxdatavalid;
	 assign dest.rxgearboxslip = src.rxgearboxslip;
	 assign src.rxheader = dest.rxheader;
	 assign src.rxheadervalid = dest.rxheadervalid;
	 assign src.rxdata = dest.rxdata;
	 assign src.rxcominitdet = dest.rxcominitdet;
	 assign src.rxcomsasdet = dest.rxcomsasdet;
	 assign src.rxcomwakedet = dest.rxcomwakedet;
	 assign src.rxvalid = dest.rxvalid;
endmodule
