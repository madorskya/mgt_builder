# replace the CELL_NAME with the MGT or COMMON cell name that you want to export parameters for
# open synthesized example design in Vivado
# copy this command into Vivado console and run
# copy the resulting printout into text file
# process with the following script: parse_props.py
# add the output to the protocol spreadsheet 

set props [report_property -all [get_cells CELL_NAME]]
