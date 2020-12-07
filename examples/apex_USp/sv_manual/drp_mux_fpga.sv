`timescale 1ns / 1ps
module drp_mux_fpga #
(
    parameter N = 2,      // 8  - number of quads 
    parameter AW_QUAD = 9 // 12 - 11:9 - MGT/COM selection, 8:0 - reg address within MGT/COM
)
(
    drp.out drp_quad[N-1:0],
    drp.in  drp_fpga,
    input drp_clk
);

	(* mark_debug *) logic [AW_QUAD-1:0] drpaddr_quad [N-1:0];
	(* mark_debug *) logic [31:0]  drpdi_quad   [N-1:0];
	(* mark_debug *) logic [31:0]  drpdo_quad   [N-1:0];
	(* mark_debug *) logic [N-1:0] drpen_quad ;
	(* mark_debug *) logic [N-1:0] drprdy_quad ;
	(* mark_debug *) logic [N-1:0] drpwe_quad ;
	(* mark_debug *) logic [N-1:0] int_reg_quad ;

	(* mark_debug *) logic [AW_QUAD-1:0] drpaddr_fpga;
	(* mark_debug *) logic [31:0] drpdi_fpga;
	(* mark_debug *) logic [31:0] drpdo_fpga;
	(* mark_debug *) logic        drpen_fpga;
	(* mark_debug *) logic        drprdy_fpga;
	(* mark_debug *) logic        drpwe_fpga;
	(* mark_debug *) logic        int_reg_fpga;
	
	genvar gi;
	generate
	   for (gi = 0; gi < N; gi++)
	   begin: ass_quad_loop
	       assign drp_quad[gi].drpaddr = drpaddr_quad[gi];
	       assign drp_quad[gi].drpdi   = drpdi_quad[gi];
	       assign drpdo_quad[gi]       = drp_quad[gi].drpdo;
	       assign drp_quad[gi].drpen   = drpen_quad[gi];
	       assign drprdy_quad[gi]      = drp_quad[gi].drprdy;
	       assign drp_quad[gi].drpwe   = drpwe_quad[gi];
	       assign drp_quad[gi].int_reg = int_reg_quad[gi];
	   end
	endgenerate

   assign drpaddr_fpga    = drp_fpga.drpaddr;
   assign drpdi_fpga      = drp_fpga.drpdi  ;
   assign drp_fpga.drpdo  = drpdo_fpga      ;
   assign drpen_fpga      = drp_fpga.drpen  ;
   assign drp_fpga.drprdy = drprdy_fpga     ;
   assign drpwe_fpga      = drp_fpga.drpwe  ;
   assign int_reg_fpga    = drp_fpga.int_reg;
   
   reg [63:0] quad_drp_select; // positional quad selector
   reg [63:0] quad_port_select; // positional quad selector
   (* mark_debug *) wire [63:0] quad_drp_select_w  = quad_drp_select; 
   (* mark_debug *) wire [63:0] quad_port_select_w = quad_port_select; 
   // use four last addresses as quad positional selectors 
   localparam quad_drp_addr1  = {{(AW_QUAD-2){1'b1}}, 2'b00};
   localparam quad_drp_addr0  = {{(AW_QUAD-2){1'b1}}, 2'b01};
   localparam quad_port_addr1 = {{(AW_QUAD-2){1'b1}}, 2'b10};
   localparam quad_port_addr0 = {{(AW_QUAD-2){1'b1}}, 2'b11};
   
	integer di;
	always @(posedge drp_clk)
	begin
		drprdy_fpga = 1'b0;
		for (di = 0; di < N; di++)
		begin
			if (drprdy_quad[di] == 1'b1)
			begin
				drpdo_fpga = drpdo_quad[di];
				drprdy_fpga = 1'b1;
			end
		end
		
		// quad selector processing
		if (drpen_fpga == 1'b1 && drpwe_fpga == 1'b1)
		begin
		    //  zero out previous selections on each write to prevent accidental broadcasts
		    case (drpaddr_fpga)
		      quad_drp_addr0 : 
		      begin 
		          quad_drp_select = 64'h0;
		          quad_port_select = 64'h0;
		          quad_drp_select[31: 0]   = drpdi_fpga; 
				  drprdy_fpga = 1'b1;
		      end
		      quad_drp_addr1: 
		      begin 
		          quad_drp_select = 64'h0;
		          quad_port_select = 64'h0;
		          quad_drp_select[63:32]   = drpdi_fpga;
				  drprdy_fpga = 1'b1;
		      end
		      quad_port_addr0:
		      begin 
		          quad_drp_select = 64'h0;
		          quad_port_select = 64'h0;
		          quad_port_select[31: 0] = drpdi_fpga;
				  drprdy_fpga = 1'b1;
		      end
		      quad_port_addr1:
		      begin
		          quad_drp_select = 64'h0;
		          quad_port_select = 64'h0;
		          quad_port_select[63:32] = drpdi_fpga;
				  drprdy_fpga = 1'b1;
		      end
		   endcase
		end
		
	end
	always @(*)
	begin
		for (di = 0; di < N; di++)
		begin
			drpaddr_quad[di] = drpaddr_fpga;
			drpdi_quad  [di] = drpdi_fpga;
			drpwe_quad  [di] = drpwe_fpga;
		end
		if (drpen_fpga == 1'b1)
		begin
            drpen_quad   = quad_drp_select;
            int_reg_quad = quad_port_select;
        end
        else
        begin
            drpen_quad   = 64'h0;
            int_reg_quad = 64'h0;
        end
	end
   

endmodule
