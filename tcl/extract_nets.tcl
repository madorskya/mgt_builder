# replace the CELL_NAME with the MGT or COMMON cell name that you want to export parameters for
# open synthesized example design in Vivado
# copy these commands into Vivado console and run 
# copy the resulting printout into text file
# process with the following script: parse_nets.py
# add the output to the protocol spreadsheet 


set_param tcl.collectionResultDisplayLimit 0

set pins [get_pins -of_objects [get_cells CELL_NAME]]

regsub -all {\s+} $pins "\n"

# puts [llength $pins]

foreach pin $pins { set nets [get_nets -top -segments -of [get_pins $pin]] ; puts "$pin $nets" }
