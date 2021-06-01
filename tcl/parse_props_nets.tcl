#!/usr/bin/tclsh

# Put next command in Vivado Tcl console: set args "YOUR GTs CELL NAME PATTERNS"; source /path/to/parse_props_nets.tcl

proc parse_props {arg} {
	puts "Parsing props ..."
	set filename $arg
	append filename "_props.out"
	set f [open "$::env(HOME)/$filename" "w"]
	set reg ".*$arg\[A-Za-z_\]+"
	set props [report_property -all [lindex [get_cells -hierarchical -regexp $reg] end] -return_string]
	foreach prop [split $props "\n"] {
		set varlist [regexp -inline -all -- {\S+} $prop]
		set length [llength $varlist]
		if {$length == 4 && [string first "Property" [lindex $varlist 0]] == -1} {
			if {[string first "." [lindex $varlist 3]] == -1 && [string first "/" [lindex $varlist 3]] == -1} {
				set var1 [string replace [lindex $varlist 0] end+1 end+1 ""]
				set var2 [string replace [lindex $varlist 3] end+1 end+1 ""]
				puts "$var1 $var2"
				puts $f "$var1 $var2"
			}
		}
	}	
	close $f
	puts "Done!"
}

proc parse_nets {arg} {
	puts "Parsing nets ..."
	set filename $arg
	append filename "_nets.out"
	set f [open "$::env(HOME)/$filename" "w"]
	set pin_mbits ""
	set net_mbits "m'b"
	set mbits 0
	set nets ""
	set_param tcl.collectionResultDisplayLimit 0
	set reg ".*$arg\[A-Za-z_\]+"
	set pins [get_pins -of_objects [lindex [get_cells -hierarchical -regexp $reg] end]]
	regsub -all {\s+} $pins "\n"
	foreach pin $pins { 
		set getnets [get_nets -top -segments -of [get_pins $pin]]
		set pairs "$pin $getnets"
		append nets "," $pairs 
		puts "$pairs" 
	}
	foreach net [split $nets ","] {
		if {[llength $net] == 2} {
			if {[string first "const0" [lindex $net 1]] != -1 || [string first "const1" [lindex $net 1]] != -1} {
				set pinlast [string last "/" [lindex $net 0]]
				set netlast [string last "/" [lindex $net 1]]
				set pn [string range [lindex $net 0] [expr int($pinlast)+1] end] 
				set nt [string range [lindex $net 1] [expr int($netlast)+1] end]
				if {[string first "\[" $pn] == -1} {
					set nt [string map {"<const" "1'b"} $nt]
					set nt [string map {">" ""} $nt]
					if {[llength $pin_mbits] > 0 && $mbits == 1} {
						puts "$pin_mbits $net_mbits"
						puts $f "$pin_mbits $net_mbits"
					}
					set pin_mbits ""
					set net_mbits ""
					set mbits 0
					puts "$pn $nt"
					puts $f "$pn $nt"
				}
				if {[string first "\[" $pn] != -1} {	
					set pinbrace [string last "\[" $pn] 
					if {$pin_mbits != [string replace $pn $pinbrace end+1 ""]} {
						if {[llength $pin_mbits] > 0 && $mbits == 1} {
							puts "$pin_mbits $net_mbits"
							puts $f "$pin_mbits $net_mbits"
						}
						set val [string range $pn [expr int($pinbrace)+1] end-1]
						set mbits [expr int($val)+1]
						set net_mbits "$mbits"
						append net_mbits "'b"
					}
					set val [string range $pn [expr int($pinbrace)+1] end-1]
					set mbits [expr int($val)+1]
					set pin_mbits [string replace $pn $pinbrace end+1 ""]
					set rplc [string map {"<const" ""} $nt]
					set rplc [string map {">" ""} $rplc]
					append net_mbits "$rplc"
				}
			}
		}
	}
	if {[llength $pin_mbits] > 0} {
		puts "$pin_mbits $net_mbits"
		puts $f "$pin_mbits $net_mbits"	
	}
	close $f
	puts "Done!"
}

foreach arg $args {
	parse_props $arg
	parse_nets $arg
}
