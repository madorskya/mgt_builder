// This code was automatically generated by FPGA MGT Builder
// Timestamp: 2020-06-26 16:20:50.435
// User: madorsky
// FPGA MGT builder homepage: <link>

`ifndef MGT_INTERFACES_SV
`define MGT_INTERFACES_SV
interface mgt_rx;
	logic [ 7:0] rxchariscomma;
	logic [ 7:0] rxcharisk;
	logic [ 7:0] rxdisperr;
	logic [ 7:0] rxnotintable;
	logic        gthrxn;
	logic        gthrxp;
	logic        rxcommadet;
	logic        rxslide;
	logic        rxchanbondseq;
	logic        rxchanisaligned;
	logic        rxchanrealign;
	logic [ 4:0] rxchbondi;
	logic [ 4:0] rxchbondo;
	logic        rxsyncin;
	logic        rxsyncout;
	logic [ 1:0] rxclkcorcnt;
	logic        rxoutclk;
	logic [ 1:0] rxdatavalid;
	logic        rxgearboxslip;
	logic [ 5:0] rxheader;
	logic [ 1:0] rxheadervalid;
	logic [63:0] rxdata;
	logic        rxcominitdet;
	logic        rxcomsasdet;
	logic        rxcomwakedet;
	logic        rxvalid;
	modport in
	(
		output rxchariscomma,
		output rxcharisk,
		output rxdisperr,
		output rxnotintable,
		input  gthrxn,
		input  gthrxp,
		output rxcommadet,
		input  rxslide,
		output rxchanbondseq,
		output rxchanisaligned,
		output rxchanrealign,
		input  rxchbondi,
		output rxchbondo,
		input  rxsyncin,
		output rxsyncout,
		output rxclkcorcnt,
		output rxoutclk,
		output rxdatavalid,
		input  rxgearboxslip,
		output rxheader,
		output rxheadervalid,
		output rxdata,
		output rxcominitdet,
		output rxcomsasdet,
		output rxcomwakedet,
		output rxvalid
	);
	modport out
	(
		input  rxchariscomma,
		input  rxcharisk,
		input  rxdisperr,
		input  rxnotintable,
		output gthrxn,
		output gthrxp,
		input  rxcommadet,
		output rxslide,
		input  rxchanbondseq,
		input  rxchanisaligned,
		input  rxchanrealign,
		output rxchbondi,
		input  rxchbondo,
		output rxsyncin,
		input  rxsyncout,
		input  rxclkcorcnt,
		input  rxoutclk,
		input  rxdatavalid,
		output rxgearboxslip,
		input  rxheader,
		input  rxheadervalid,
		input  rxdata,
		input  rxcominitdet,
		input  rxcomsasdet,
		input  rxcomwakedet,
		input  rxvalid
	);
endinterface

interface mgt_tx;
	logic [ 7:0] txcharisk;
	logic        txphdlytstclk;
	logic        gthtxn;
	logic        gthtxp;
	logic        txoutclk;
	logic        txgearboxready;
	logic [ 2:0] txheader;
	logic [ 6:0] txsequence;
	logic        txstartseq;
	logic [ 7:0] txchardispmode;
	logic [ 7:0] txchardispval;
	logic [63:0] txdata;
	modport in
	(
		input  txcharisk,
		input  txphdlytstclk,
		output gthtxn,
		output gthtxp,
		output txoutclk,
		output txgearboxready,
		input  txheader,
		input  txsequence,
		input  txstartseq,
		input  txchardispmode,
		input  txchardispval,
		input  txdata
	);
	modport out
	(
		output txcharisk,
		output txphdlytstclk,
		input  gthtxn,
		input  gthtxp,
		input  txoutclk,
		input  txgearboxready,
		output txheader,
		output txsequence,
		output txstartseq,
		output txchardispmode,
		output txchardispval,
		output txdata
	);
endinterface

`endif