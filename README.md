# FPGA MGT builder
FPGA MGT builder is a set of software tools with the following functionality:
-	Automatic generation of firmware structure with support for arbitrary MGT configurations. This includes:
    -	Different bit rates and encodings in RX and TX parts of the same MGT
    -	Using CPLL and QPLL as needed for each MGT, programmable separately for RX and TX parts (within the constraints of the particular FPGA architecture)
    -	Automatic routing and assignment of available reference clocks for each MGT
    -	Grouping MGTs into interfaces with programmable names and indexes, which makes using them in the top-level design much easier
    -	Automatic generation of all constraints related to MGTs
        -	Reference clock location, timing, and grouping
        -	MGT location
        -	User clock timing
    -	The software is designed to make porting a project to different board design or FPGA an easy task.
    -	Targeting lowest-latency serial links, by disabling RX and TX buffers
-	Software framework:
    -	Reads configuration settings and programs all DRP registers and port settings in each MGT and COMMON modules
    -	Customizable reset procedures, with main functionality provided in the example design
    -	Written in portable C++, can be adapted for nearly any system, including embedded processors
    -	Does not need rework if the MGT configuration is changed
-	Configuration sources:
    -	All source configuration files are kept in Excel XLSX format
    -	Makes working with them much easier
    -	Data format is optimized for direct copying from Xilinx manuals and example source code, with minimal manual rework
    -	A Python script is provided for exporting configuration files into plain text
-	Currently supported:
    -	System Verilog HDL
    -	Linux
    -	Xilinx 7-series FPGAs, GTH transceivers
    -	Ultrascale support is coming soon

See documents in doc directory
