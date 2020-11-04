			IBUFDS_GTE2 refclk_buf
			(
			    .O     (refclk   [gi]),
			    .ODIV2 (),
			    .CEB   (1'b0),
			    .I     (refclk_p [gi]),
			    .IB    (refclk_n [gi])
			);
