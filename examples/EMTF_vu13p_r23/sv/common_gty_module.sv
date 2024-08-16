// This code was automatically generated by FPGA MGT Builder
// Timestamp: 2024-08-15 16:10:40.534
// User: agreshil
// FPGA MGT builder homepage: https://github.com/madorskya/mgt_builder

`include "drp_interface.sv"
`include "mgt_gty_interfaces.sv"
module GTYE4_COMMON_tux
#(parameter [7:0] qind = 0, parameter [7:0] mind = 0)
(
	drp.in drp_gty_if,
	input         gtrefclk00,
	input         gtrefclk01,
	output        qpll0outclk,
	output        qpll1outclk,
	output        qpll0outrefclk,
	output        qpll1outrefclk,
	input         qpll0lockdetclk,
	input         qpll1lockdetclk,
	input         drpclk
);
`include "xymap_gty.sv"
	wire [15:0] XY = mgtxy[qind][mind];
	reg  [31:0] reg_  [ 2: 0];
	wire [31:0] wire_ [ 3: 3];
	initial
	begin
		reg_[0][ 0: 0] = 1'b1; // QPLL0PD
		reg_[0][ 1: 1] = 1'b1; // QPLL1PD
	end
    reg         int_reg_rdy;
    reg  [31:0] drpdo_reg;
    wire        drprdy_mgt;
    wire [31:0] drpdo_mgt;
    reg         drprdy_reg;
    assign drp_gty_if.drprdy = drprdy_reg;
    assign drp_gty_if.drpdo  = drpdo_reg;
    always @(posedge drpclk)
    begin
        drprdy_reg = 1'b0;
		drpdo_reg = 32'h0;
        if (drp_gty_if.int_reg == 1'b1)
        begin
            if (drp_gty_if.drpwe == 1'b1)
            begin
                case (drp_gty_if.drpaddr)

                    9'd0: reg_[0] = drp_gty_if.drpdi;
                    9'd1: reg_[1] = drp_gty_if.drpdi;
                    9'd2: reg_[2] = drp_gty_if.drpdi;
                endcase
                drprdy_reg = 1'b1;
            end
            else
            begin
                case (drp_gty_if.drpaddr)

                    9'd0: drpdo_reg = reg_[0];
                    9'd1: drpdo_reg = reg_[1];
                    9'd2: drpdo_reg = reg_[2];
                    9'd3: drpdo_reg = wire_[3];
                    9'h1ff: drpdo_reg = XY; // each MGT reports its XY
                endcase
                drprdy_reg = 1'b1;
            end
        end

        if (drprdy_mgt == 1'b1)
        begin
            drpdo_reg = drpdo_mgt;
            drprdy_reg = 1'b1;
        end
    end

	GTYE4_COMMON GTYE4_COMMON_inst
	(
		.QPLL0REFCLKSEL             (3'b1),
		.QPLL1REFCLKSEL             (3'b1),
		.GTNORTHREFCLK00            (1'b0),
		.GTNORTHREFCLK01            (1'b0),
		.GTNORTHREFCLK10            (1'b0),
		.GTNORTHREFCLK11            (1'b0),
		.GTREFCLK00                 (gtrefclk00                ),
		.GTREFCLK10                 (1'b0),
		.GTREFCLK01                 (gtrefclk01                ),
		.GTREFCLK11                 (1'b0),
		.GTSOUTHREFCLK00            (1'b0),
		.GTSOUTHREFCLK01            (1'b0),
		.GTSOUTHREFCLK10            (1'b0),
		.GTSOUTHREFCLK11            (1'b0),
		.QPLL0OUTCLK                (qpll0outclk               ),
		.QPLL1OUTCLK                (qpll1outclk               ),
		.QPLL0OUTREFCLK             (qpll0outrefclk            ),
		.QPLL1OUTREFCLK             (qpll1outrefclk            ),
		.REFCLKOUTMONITOR0          (),
		.REFCLKOUTMONITOR1          (),
		.QPLLDMONITOR0              (),
		.QPLLDMONITOR1              (),
		.QPLL0CLKRSVD0              (1'b0),
		.QPLL1CLKRSVD0              (1'b0),
		.QPLL0CLKRSVD1              (1'b0),
		.QPLL1CLKRSVD1              (1'b0),
		.QPLL0FBCLKLOST             (wire_[3][ 0: 0]),
		.QPLL1FBCLKLOST             (wire_[3][ 1: 1]),
		.QPLL0LOCK                  (wire_[3][ 2: 2]),
		.QPLL1LOCK                  (wire_[3][ 3: 3]),
		.QPLL0LOCKDETCLK            (qpll0lockdetclk           ),
		.QPLL1LOCKDETCLK            (qpll1lockdetclk           ),
		.QPLL0LOCKEN                (1'b1),
		.QPLL1LOCKEN                (1'b1),
		.QPLL0PD                    ( reg_[0][ 0: 0]),
		.QPLL1PD                    ( reg_[0][ 1: 1]),
		.QPLL0REFCLKLOST            (wire_[3][ 4: 4]),
		.QPLL1REFCLKLOST            (wire_[3][ 5: 5]),
		.QPLL0RESET                 ( reg_[0][ 2: 2]),
		.QPLL1RESET                 ( reg_[0][ 3: 3]),
		.QPLLRSVD1                  ( reg_[0][11: 4]),
		.QPLLRSVD2                  ( reg_[0][16:12]),
		.QPLLRSVD3                  ( reg_[0][21:17]),
		.QPLLRSVD4                  ( reg_[0][29:22]),
		.BGBYPASSB                  (1'b1),
		.BGMONITORENB               (1'b1),
		.BGPDB                      (1'b1),
		.BGRCALOVRD                 (5'b11111),
		.BGRCALOVRDENB              (1'b1),
		.RCALENB                    (1'b1),
		.PMARSVD0                   (8'b0),
		.PMARSVD1                   (8'b0),
		.SDM0RESET                  ( reg_[0][30:30]),
		.SDM1RESET                  ( reg_[0][31:31]),
		.SDM0DATA                   ( reg_[1][24: 0]),
		.SDM1DATA                   ( reg_[2][24: 0]),
		.SDM0WIDTH                  ( reg_[2][26:25]),
		.SDM1WIDTH                  ( reg_[2][28:27]),
		.QPLL0FBDIV                 (8'b00000000),
		.QPLL1FBDIV                 (8'b00000000),
		.SDM0TOGGLE                 (1'b0),
		.SDM1TOGGLE                 (1'b0),
		.SDM0FINALOUT               (),
		.SDM1FINALOUT               (),
		.SDM0TESTDATA               (),
		.SDM1TESTDATA               (),
		.DRPADDR                    (drp_gty_if.drpaddr        ),
		.DRPCLK                     (drpclk                    ),
		.DRPDI                      (drp_gty_if.drpdi [15:0]),
		.DRPDO                      (drpdo_mgt [15:0]),
		.DRPEN                      (drp_gty_if.drpen          ),
		.DRPRDY                     (drprdy_mgt),
		.DRPWE                      (drp_gty_if.drpwe          )
	);
endmodule
