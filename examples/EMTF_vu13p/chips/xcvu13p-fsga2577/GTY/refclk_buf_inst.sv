			IBUFDS_GTE4 #		
			(		
				.REFCLK_EN_TX_PATH  (1'b0),	
				.REFCLK_HROW_CK_SEL (2'b00),	
				.REFCLK_ICNTL_RX    (2'b00)	
			) 		
			refclk_buf		
			(		
				.I     (refclk_p [gi]),	
				.IB    (refclk_n [gi]),	
				.CEB   (1'b0),	
				.O     (refclk   [gi]),	
				.ODIV2 (refclk_odiv [gi])	
			);		
