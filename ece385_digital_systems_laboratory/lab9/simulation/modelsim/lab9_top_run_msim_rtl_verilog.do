transcript on
if {[file exists rtl_work]} {
	vdel -lib rtl_work -all
}
vlib rtl_work
vmap work rtl_work

vlog -sv -work work +incdir+C:/Users/ZLO/Documents/lab9 {C:/Users/ZLO/Documents/lab9/SubBytes.sv}
vlog -sv -work work +incdir+C:/Users/ZLO/Documents/lab9 {C:/Users/ZLO/Documents/lab9/InvShiftRows.sv}
vlog -sv -work work +incdir+C:/Users/ZLO/Documents/lab9 {C:/Users/ZLO/Documents/lab9/InvMixColumns.sv}
vlog -sv -work work +incdir+C:/Users/ZLO/Documents/lab9 {C:/Users/ZLO/Documents/lab9/AES.sv}
vlog -sv -work work +incdir+C:/Users/ZLO/Documents/lab9 {C:/Users/ZLO/Documents/lab9/KeyExpansion.sv}
vlog -sv -work work +incdir+C:/Users/ZLO/Documents/lab9 {C:/Users/ZLO/Documents/lab9/avalon_aes_interface.sv}

vlog -sv -work work +incdir+C:/Users/ZLO/Documents/lab9 {C:/Users/ZLO/Documents/lab9/testbench.sv}

vsim -t 1ps -L altera_ver -L lpm_ver -L sgate_ver -L altera_mf_ver -L altera_lnsim_ver -L cycloneive_ver -L rtl_work -L work -voptargs="+acc"  testbench

add wave *
view structure
view signals
run 4000 ns
