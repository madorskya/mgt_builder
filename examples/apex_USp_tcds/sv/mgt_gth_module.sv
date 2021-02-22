// This code was automatically generated by FPGA MGT Builder
// Timestamp: 2021-01-30 10:52:57.958
// User: madorsky
// FPGA MGT builder homepage: <link>

`include "drp_interface.sv"
`include "mgt_gth_interfaces.sv"
module GTHE4_CHANNEL_tux
#(parameter [7:0] qind = 0, parameter [7:0] mind = 0)
(
	drp.in drp_gth_if,
	mgt_gth_tx.in mgt_gth_tx_if,
	mgt_gth_rx.in mgt_gth_rx_if,
	input         cplllockdetclk,
	input         dmonitorclk,
	input         drpclk,
	input         qpll0clk,
	input         qpll0refclk,
	input         qpll1clk,
	input         qpll1refclk,
	output        rxphaligndone,
	input         rxsyncallin,
	input         rxusrclk,
	input         rxusrclk2,
	input         txlatclk,
	input         txusrclk,
	input         txusrclk2
);
`include "xymap_gth.sv"
	wire [15:0] XY = mgtxy[qind][mind];
	reg  [31:0] reg_  [ 4: 0];
	wire [31:0] wire_ [ 5: 5];
    
      // manual addition: power down everything by default 
    initial
    begin
       reg_[0][1] = 1'b1;
       reg_[1][1:0] = 2'b11;
       reg_[3][5:4] = 2'b11;
    end

    reg         int_reg_rdy;
    reg  [31:0] drpdo_reg;
    wire        drprdy_mgt;
    wire [31:0] drpdo_mgt;
    reg         drprdy_reg;
    assign drp_gth_if.drprdy = drprdy_reg;
    assign drp_gth_if.drpdo  = drpdo_reg;
    always @(posedge drpclk)
    begin
        drprdy_reg = 1'b0;
		drpdo_reg = 32'h0;
        if (drp_gth_if.int_reg == 1'b1)
        begin
            if (drp_gth_if.drpwe == 1'b1)
            begin
                case (drp_gth_if.drpaddr)

                    9'd0: reg_[0] = drp_gth_if.drpdi;
                    9'd1: reg_[1] = drp_gth_if.drpdi;
                    9'd2: reg_[2] = drp_gth_if.drpdi;
                    9'd3: reg_[3] = drp_gth_if.drpdi;
                    9'd4: reg_[4] = drp_gth_if.drpdi;
                endcase
                drprdy_reg = 1'b1;
            end
            else
            begin
                case (drp_gth_if.drpaddr)

                    9'd0: drpdo_reg = reg_[0];
                    9'd1: drpdo_reg = reg_[1];
                    9'd2: drpdo_reg = reg_[2];
                    9'd3: drpdo_reg = reg_[3];
                    9'd4: drpdo_reg = reg_[4];
                    9'd5: drpdo_reg = wire_[5];
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

	bufg_gt_tux rxoutclk_buf (.O(mgt_gth_rx_if.rxoutclk), .I(rxoutclk_loc));
	GTHE4_CHANNEL GTHE4_CHANNEL_inst
	(
		.CDRSTEPDIR                 (1'b0),
		.CDRSTEPSQ                  (1'b0),
		.CDRSTEPSX                  (1'b0),
		.CFGRESET                   ( reg_[0][ 0: 0]),
		.CPLLFBCLKLOST              (wire_[5][ 0: 0]),
		.CPLLLOCK                   (wire_[5][ 1: 1]),
		.CPLLLOCKDETCLK             (cplllockdetclk            ),
		.CPLLLOCKEN                 (1'b1),
		.CPLLPD                     ( reg_[0][ 1: 1]),
		.CPLLREFCLKLOST             (wire_[5][ 2: 2]),
		.CPLLREFCLKSEL              (3'b1),
		.CPLLRESET                  (1'b0),
		.DMONFIFORESET              (1'b0),
		.DMONITORCLK                (dmonitorclk               ),
		.DMONITOROUT                (),
		.DMONITOROUTCLK             (),
		.DRPADDR                    (drp_gth_if.drpaddr        ),
		.DRPCLK                     (drpclk                    ),
		.DRPDI                      (drp_gth_if.drpdi [15:0]),
		.DRPDO                      (drpdo_mgt [15:0]),
		.DRPEN                      (drp_gth_if.drpen          ),
		.DRPRDY                     (drprdy_mgt),
		.DRPRST                     ( reg_[0][ 2: 2]),
		.DRPWE                      (drp_gth_if.drpwe          ),
		.EYESCANDATAERROR           (wire_[5][ 3: 3]),
		.EYESCANRESET               ( reg_[0][ 3: 3]),
		.EYESCANTRIGGER             ( reg_[0][ 4: 4]),
		.FREQOS                     (1'b0),
		.GTGREFCLK                  (1'b0),
		.GTNORTHREFCLK0             (1'b0),
		.GTNORTHREFCLK1             (1'b0),
		.GTPOWERGOOD                (wire_[5][ 4: 4]),
		.GTREFCLK0                  (1'b0),
		.GTREFCLK1                  (1'b0),
		.GTREFCLKMONITOR            (),
		.GTRXRESET                  ( reg_[0][ 5: 5]),
		.GTRXRESETSEL               ( reg_[0][ 6: 6]),
		.GTSOUTHREFCLK0             (1'b0),
		.GTSOUTHREFCLK1             (1'b0),
		.GTTXRESET                  ( reg_[0][ 7: 7]),
		.GTTXRESETSEL               ( reg_[0][ 8: 8]),
		.GTHRXN                     (mgt_gth_rx_if.gthrxn      ),
		.GTHRXP                     (mgt_gth_rx_if.gthrxp      ),
		.INCPCTRL                   (1'b0),
		.LOOPBACK                   ( reg_[0][11: 9]),
		.GTHTXN                     (mgt_gth_tx_if.gthtxn      ),
		.GTHTXP                     (mgt_gth_tx_if.gthtxp      ),
		.PCSRSVDOUT                 (),
		.PHYSTATUS                  (),
		.QPLL0CLK                   (qpll0clk                  ),
		.QPLL0REFCLK                (qpll0refclk               ),
		.QPLL1CLK                   (qpll1clk                  ),
		.QPLL1REFCLK                (qpll1refclk               ),
		.RESETEXCEPTION             (),
		.RESETOVRD                  (1'b0),
		.RX8B10BEN                  ( reg_[0][12:12]),
		.RXAFECFOKEN                (1'b1),
		.RXBUFRESET                 ( reg_[0][13:13]),
		.RXBUFSTATUS                (wire_[5][ 7: 5]),
		.RXBYTEISALIGNED            (wire_[5][ 8: 8]),
		.RXBYTEREALIGN              (),
		.RXCDRFREQRESET             (1'b0),
		.RXCDRHOLD                  ( reg_[0][14:14]),
		.RXCDRLOCK                  (),
		.RXCDROVRDEN                ( reg_[0][15:15]),
		.RXCDRPHDONE                (),
		.RXCDRRESET                 (1'b0),
		.RXCHANBONDSEQ              (),
		.RXCHANISALIGNED            (),
		.RXCHANREALIGN              (),
		.RXCHBONDEN                 (1'b0),
		.RXCHBONDI                  (5'b0),
		.RXCHBONDLEVEL              (3'b0),
		.RXCHBONDMASTER             (1'b0),
		.RXCHBONDO                  (),
		.RXCHBONDSLAVE              (1'b0),
		.RXCKCALDONE                (),
		.RXCKCALRESET               (1'b0),
		.RXCKCALSTART               (7'b0),
		.RXCLKCORCNT                (),
		.RXCOMINITDET               (),
		.RXCOMMADET                 (),
		.RXCOMMADETEN               ( reg_[0][16:16]),
		.RXCOMSASDET                (),
		.RXCOMWAKEDET               (),
		.RXCTRL0                    (mgt_gth_rx_if.rxctrl0     ),
		.RXCTRL1                    (mgt_gth_rx_if.rxctrl1     ),
		.RXCTRL2                    (mgt_gth_rx_if.rxctrl2     ),
		.RXCTRL3                    (mgt_gth_rx_if.rxctrl3     ),
		.RXDATA                     (mgt_gth_rx_if.rxdata      ),
		.RXDATAEXTENDRSVD           (),
		.RXDATAVALID                (mgt_gth_rx_if.rxdatavalid ),
		.RXDFEAGCHOLD               (1'b0),
		.RXDFEAGCOVRDEN             (1'b0),
		.RXDFECFOKFCNUM             (4'b1101),
		.RXDFECFOKFEN               (1'b0),
		.RXDFECFOKFPULSE            (1'b0),
		.RXDFECFOKHOLD              (1'b0),
		.RXDFECFOKOVREN             (1'b0),
		.RXDFEKHHOLD                (1'b0),
		.RXDFEKHOVRDEN              (1'b0),
		.RXDFELFHOLD                (1'b0),
		.RXDFELFOVRDEN              (1'b0),
		.RXDFELPMRESET              ( reg_[0][17:17]),
		.RXDFETAP10HOLD             (1'b0),
		.RXDFETAP10OVRDEN           (1'b0),
		.RXDFETAP11HOLD             (1'b0),
		.RXDFETAP11OVRDEN           (1'b0),
		.RXDFETAP12HOLD             (1'b0),
		.RXDFETAP12OVRDEN           (1'b0),
		.RXDFETAP13HOLD             (1'b0),
		.RXDFETAP13OVRDEN           (1'b0),
		.RXDFETAP14HOLD             (1'b0),
		.RXDFETAP14OVRDEN           (1'b0),
		.RXDFETAP15HOLD             (1'b0),
		.RXDFETAP15OVRDEN           (1'b0),
		.RXDFETAP2HOLD              (1'b0),
		.RXDFETAP2OVRDEN            (1'b0),
		.RXDFETAP3HOLD              (1'b0),
		.RXDFETAP3OVRDEN            (1'b0),
		.RXDFETAP4HOLD              (1'b0),
		.RXDFETAP4OVRDEN            (1'b0),
		.RXDFETAP5HOLD              (1'b0),
		.RXDFETAP5OVRDEN            (1'b0),
		.RXDFETAP6HOLD              (1'b0),
		.RXDFETAP6OVRDEN            (1'b0),
		.RXDFETAP7HOLD              (1'b0),
		.RXDFETAP7OVRDEN            (1'b0),
		.RXDFETAP8HOLD              (1'b0),
		.RXDFETAP8OVRDEN            (1'b0),
		.RXDFETAP9HOLD              (1'b0),
		.RXDFETAP9OVRDEN            (1'b0),
		.RXDFEUTHOLD                (1'b0),
		.RXDFEUTOVRDEN              (1'b0),
		.RXDFEVPHOLD                (1'b0),
		.RXDFEVPOVRDEN              (1'b0),
		.RXDFEXYDEN                 (1'b1),
		.RXDLYBYPASS                ( reg_[0][18:18]),
		.RXDLYEN                    ( reg_[0][19:19]),
		.RXDLYOVRDEN                ( reg_[0][20:20]),
		.RXDLYSRESET                ( reg_[0][21:21]),
		.RXDLYSRESETDONE            (wire_[5][ 9: 9]),
		.RXELECIDLE                 (),
		.RXELECIDLEMODE             ( reg_[0][23:22]),
		.RXGEARBOXSLIP              (mgt_gth_rx_if.rxgearboxslip),
		.RXHEADER                   (mgt_gth_rx_if.rxheader    ),
		.RXHEADERVALID              (mgt_gth_rx_if.rxheadervalid),
		.RXLATCLK                   (mgt_gth_rx_if.rxlatclk    ),
		.RXLPMEN                    ( reg_[0][24:24]),
		.RXLPMGCHOLD                (1'b0),
		.RXLPMGCOVRDEN              (1'b0),
		.RXLPMHFHOLD                (1'b0),
		.RXLPMHFOVRDEN              (1'b0),
		.RXLPMLFHOLD                (1'b0),
		.RXLPMLFKLOVRDEN            (1'b0),
		.RXLPMOSHOLD                (1'b0),
		.RXLPMOSOVRDEN              (1'b0),
		.RXMCOMMAALIGNEN            ( reg_[0][25:25]),
		.RXMONITOROUT               (),
		.RXMONITORSEL               (2'b00),
		.RXOOBRESET                 (1'b0),
		.RXOSCALRESET               ( reg_[0][26:26]),
		.RXOSHOLD                   (1'b0),
		.RXOSINTDONE                (),
		.RXOSINTSTARTED             (),
		.RXOSINTSTROBEDONE          (),
		.RXOSINTSTROBESTARTED       (),
		.RXOSOVRDEN                 (1'b0),
		.RXOUTCLK                   (rxoutclk_loc              ),
		.RXOUTCLKFABRIC             (),
		.RXOUTCLKPCS                (),
		.RXOUTCLKSEL                ( reg_[0][29:27]),
		.RXPCOMMAALIGNEN            ( reg_[0][30:30]),
		.RXPCSRESET                 ( reg_[0][31:31]),
		.RXPD                       ( reg_[1][ 1: 0]),
		.RXPHALIGN                  ( reg_[1][ 2: 2]),
		.RXPHALIGNDONE              (rxphaligndone             ),
		.RXPHALIGNEN                ( reg_[1][ 3: 3]),
		.RXPHALIGNERR               (wire_[5][10:10]),
		.RXPHDLYPD                  (1'b0),
		.RXPHDLYRESET               ( reg_[1][ 4: 4]),
		.RXPLLCLKSEL                ( reg_[1][ 6: 5]),
		.RXPMARESET                 ( reg_[1][ 7: 7]),
		.RXPMARESETDONE             (wire_[5][11:11]),
		.RXPOLARITY                 ( reg_[1][ 8: 8]),
		.RXPRBSCNTRESET             ( reg_[1][ 9: 9]),
		.RXPRBSERR                  (wire_[5][12:12]),
		.RXPRBSLOCKED               (wire_[5][13:13]),
		.RXPRBSSEL                  ( reg_[1][13:10]),
		.RXPRGDIVRESETDONE          (wire_[5][14:14]),
		.RXPROGDIVRESET             ( reg_[1][14:14]),
		.RXRATE                     ( reg_[1][17:15]),
		.RXRATEDONE                 (),
		.RXRATEMODE                 ( reg_[1][18:18]),
		.RXRESETDONE                (wire_[5][15:15]),
		.RXSLIDE                    (mgt_gth_rx_if.rxslide     ),
		.RXSLIPOUTCLK               ( reg_[1][19:19]),
		.RXSLIPPMA                  ( reg_[1][20:20]),
		.RXSTARTOFSEQ               (mgt_gth_rx_if.rxstartofseq),
		.RXSTATUS                   (),
		.RXSYNCALLIN                (rxsyncallin               ),
		.RXSYNCDONE                 (wire_[5][16:16]),
		.RXSYNCIN                   ( reg_[1][21:21]),
		.RXSYNCMODE                 ( reg_[1][22:22]),
		.RXSYNCOUT                  (wire_[5][17:17]),
		.RXSYSCLKSEL                ( reg_[1][24:23]),
		.RXTERMINATION              (1'b0),
		.RXUSERRDY                  ( reg_[1][25:25]),
		.RXUSRCLK                   (rxusrclk                  ),
		.RXUSRCLK2                  (rxusrclk2                 ),
		.TX8B10BBYPASS              ( reg_[2][ 7: 0]),
		.TX8B10BEN                  ( reg_[2][ 8: 8]),
		.TXBUFSTATUS                (wire_[5][19:18]),
		.TXCOMFINISH                (),
		.TXCOMINIT                  (1'b0),
		.TXCOMSAS                   (1'b0),
		.TXCOMWAKE                  (1'b0),
		.TXCTRL0                    (mgt_gth_tx_if.txctrl0     ),
		.TXCTRL1                    (mgt_gth_tx_if.txctrl1     ),
		.TXCTRL2                    (mgt_gth_tx_if.txctrl2     ),
		.TXDATA                     (mgt_gth_tx_if.txdata      ),
		.TXDATAEXTENDRSVD           (1'b0),
		.TXDCCDONE                  (),
		.TXDCCFORCESTART            (),
		.TXDCCRESET                 (),
		.TXDEEMPH                   ( reg_[2][10: 9]),
		.TXDETECTRX                 (1'b0),
		.TXDIFFCTRL                 ( reg_[2][15:11]),
		.TXDLYBYPASS                ( reg_[2][16:16]),
		.TXDLYEN                    ( reg_[2][17:17]),
		.TXDLYHOLD                  (1'b0),
		.TXDLYOVRDEN                ( reg_[2][18:18]),
		.TXDLYSRESET                ( reg_[2][19:19]),
		.TXDLYSRESETDONE            (wire_[5][20:20]),
		.TXDLYUPDOWN                (1'b0),
		.TXELECIDLE                 ( reg_[2][20:20]),
		.TXHEADER                   (mgt_gth_tx_if.txheader    ),
		.TXINHIBIT                  ( reg_[2][21:21]),
		.TXLATCLK                   (txlatclk                  ),
		.TXMAINCURSOR               ( reg_[2][28:22]),
		.TXMARGIN                   ( reg_[2][31:29]),
		.TXOUTCLK                   (mgt_gth_tx_if.txoutclk    ),
		.TXOUTCLKFABRIC             (),
		.TXOUTCLKPCS                (),
		.TXOUTCLKSEL                ( reg_[3][ 2: 0]),
		.TXPCSRESET                 ( reg_[3][ 3: 3]),
		.TXPD                       ( reg_[3][ 5: 4]),
		.TXPDELECIDLEMODE           (1'b0),
		.TXPHALIGN                  ( reg_[3][ 6: 6]),
		.TXPHALIGNDONE              (wire_[5][21:21]),
		.TXPHALIGNEN                ( reg_[3][ 7: 7]),
		.TXPHDLYPD                  (1'b0),
		.TXPHDLYRESET               ( reg_[3][ 8: 8]),
		.TXPHDLYTSTCLK              (1'b0),
		.TXPHINIT                   (1'b0),
		.TXPHINITDONE               (wire_[5][22:22]),
		.TXPHOVRDEN                 ( reg_[3][ 9: 9]),
		.TXPIPPMEN                  ( reg_[3][10:10]),
		.TXPIPPMOVRDEN              ( reg_[3][11:11]),
		.TXPIPPMPD                  ( reg_[3][12:12]),
		.TXPIPPMSEL                 ( reg_[3][13:13]),
		.TXPIPPMSTEPSIZE            ( reg_[3][18:14]),
		.TXPISOPD                   (1'b0),
		.TXPLLCLKSEL                ( reg_[3][20:19]),
		.TXPMARESET                 ( reg_[3][21:21]),
		.TXPMARESETDONE             (wire_[5][23:23]),
		.TXPOLARITY                 ( reg_[3][22:22]),
		.TXPOSTCURSOR               ( reg_[3][27:23]),
		.TXPRBSFORCEERR             ( reg_[3][28:28]),
		.TXPRBSSEL                  ( reg_[4][ 3: 0]),
		.TXPRECURSOR                ( reg_[4][ 8: 4]),
		.TXPRGDIVRESETDONE          (wire_[5][24:24]),
		.TXPROGDIVRESET             ( reg_[4][ 9: 9]),
		.TXRATE                     ( reg_[4][12:10]),
		.TXRATEDONE                 (wire_[5][25:25]),
		.TXRATEMODE                 ( reg_[4][13:13]),
		.TXRESETDONE                (wire_[5][26:26]),
		.TXSEQUENCE                 (mgt_gth_tx_if.txsequence  ),
		.TXSWING                    ( reg_[4][14:14]),
		.TXSYNCALLIN                ( reg_[4][15:15]),
		.TXSYNCDONE                 (wire_[5][27:27]),
		.TXSYNCIN                   ( reg_[4][16:16]),
		.TXSYNCMODE                 ( reg_[4][17:17]),
		.TXSYNCOUT                  (wire_[5][28:28]),
		.TXSYSCLKSEL                ( reg_[4][19:18]),
		.TXUSERRDY                  ( reg_[4][20:20]),
		.TXUSRCLK                   (txusrclk                  ),
		.TXUSRCLK2                  (txusrclk2                 ),
		.RXQPISENN                  (),
		.RXQPISENP                  (),
		.TXQPISENN                  (),
		.TXQPISENP                  (),
		.RXDFEAGCCTRL               (2'b01),
		.RXPHOVRDEN                 (1'b0),
		.RXQPIEN                    (1'b0),
		.TXQPIBIASEN                (1'b0),
		.TXQPIWEAKPUP               (1'b0)
	);
endmodule
