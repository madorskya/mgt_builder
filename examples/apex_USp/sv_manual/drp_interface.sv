`ifndef DRP_INTERFACE_SV
`define DRP_INTERFACE_SV

interface drp #(parameter AW = 9);
	logic [AW-1:0] drpaddr;
//	logic          drpclk;
	logic [31:0]   drpdi;
	logic [31:0]   drpdo;
	logic          drpen;
	logic          drprdy;
	logic          drpwe;
	logic          int_reg;
	modport in
	(
		input  drpaddr,
//		input  drpclk,
		input  drpdi,
		output drpdo,
		input  drpen,
		output drprdy,
		input  drpwe,
		input  int_reg
	);
	modport out
	(
		output drpaddr,
//		output drpclk,
		output drpdi,
		input  drpdo,
		output drpen,
		input  drprdy,
		output drpwe,
		output int_reg
	);
endinterface
`endif

