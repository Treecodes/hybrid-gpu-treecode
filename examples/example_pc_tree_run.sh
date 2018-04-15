#!/bin/bash

# s4 t4
#../bin_cpu/tree_cpu.exe /scratch/krasny_fluxg/lwwilson/S10000.bin /scratch/krasny_fluxg/lwwilson/T10000.bin /scratch/krasny_fluxg/lwwilson/ex_s4_t4_0_p1.bin out.tsv 10000 10000 0.5 5 1 1000 0 0 0 1 0
#../bin/tree.exe         /scratch/krasny_fluxg/lwwilson/S10000.bin /scratch/krasny_fluxg/lwwilson/T10000.bin /scratch/krasny_fluxg/lwwilson/ex_s4_t4_0_p1.bin out.tsv 10000 10000 0.0 5 1 1000 0 0 0 1 0 2000

# s5 t4
#../bin_cpu/tree_cpu.exe /scratch/krasny_fluxg/lwwilson/S100000.bin /scratch/krasny_fluxg/lwwilson/T10000.bin /scratch/krasny_fluxg/lwwilson/ex_s5_t4_0_p1.bin out.tsv 100000 10000 0.0 5 1 10000 0 0 0 1 0 2000
#../bin/tree.exe         /scratch/krasny_fluxg/lwwilson/S100000.bin /scratch/krasny_fluxg/lwwilson/T10000.bin /scratch/krasny_fluxg/lwwilson/ex_s5_t4_0_p1.bin out.tsv 100000 10000 0.0 5 1 10000 0 0 0 1 0 2000

# s6 t5
#../bin_cpu/tree_cpu.exe /scratch/krasny_fluxg/lwwilson/S1000000.bin /scratch/krasny_fluxg/lwwilson/T100000.bin /scratch/krasny_fluxg/lwwilson/ex_s6_t5_0_p1.bin out.tsv 1000000 100000 0.5 5 1 1000 0 0 0 1 0 2000
#../bin/tree.exe         /scratch/krasny_fluxg/lwwilson/S1000000.bin /scratch/krasny_fluxg/lwwilson/T100000.bin /scratch/krasny_fluxg/lwwilson/ex_s6_t5_0_p1.bin out.tsv 1000000 100000 0.0 5 1 30000 0 0 0 1 0 2000

# s7 t6


# testing across methods
#echo ~~~~~~~~~~~~~~~~~~~~~CPU DIRECT SUM~~~~~~~~~~~~~~~~~~~~~~~~
#../bin_cpu/tree_cpu.exe /scratch/krasny_fluxg/lwwilson/S10000000.bin /scratch/krasny_fluxg/lwwilson/T10000000.bin /scratch/krasny_fluxg/lwwilson/ex_s6_t5_0_p1.bin out.tsv 200000 200000 0.0 5 1 1000 0 0 0 1 0 100000
#echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

echo ~~~~~~~~~~~~~~~~~~~~~~CPU TREECODE~~~~~~~~~~~~~~~~~~~~~~~~~
../bin_cpu/tree_cpu.exe /scratch/krasny_fluxg/lwwilson/S10000000.bin /scratch/krasny_fluxg/lwwilson/T10000000.bin /scratch/krasny_fluxg/lwwilson/ex_s6_t5_0_p1.bin out.tsv 1600000 1600000 0.5 5 1 1000 0 0 0 1 0 1
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#echo ~~~~~~~CPU TREECODE with large leaves and batching~~~~~~~~~
#../bin_cpu/tree_cpu.exe /scratch/krasny_fluxg/lwwilson/S10000000.bin /scratch/krasny_fluxg/lwwilson/T10000000.bin /scratch/krasny_fluxg/lwwilson/ex_s6_t5_0_p1.bin out.tsv 200000 200000 0.5 5 1 30000 0 0 0 1 0 2000
#echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

echo ~~~~~~~~~~~~~~~~~~~~~GPU DIRECT SUM~~~~~~~~~~~~~~~~~~~~~~~~
../bin/tree.exe         /scratch/krasny_fluxg/lwwilson/S10000000.bin /scratch/krasny_fluxg/lwwilson/T10000000.bin /scratch/krasny_fluxg/lwwilson/ex_s6_t5_0_p1.bin out.tsv 1600000 1600000 0.0 5 1 30000 0 0 0 1 0 2000
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

echo ~~~~~~~~~~~~~~~~~~~~~~GPU TREECODE~~~~~~~~~~~~~~~~~~~~~~~~~
../bin/tree.exe         /scratch/krasny_fluxg/lwwilson/S10000000.bin /scratch/krasny_fluxg/lwwilson/T10000000.bin /scratch/krasny_fluxg/lwwilson/ex_s6_t5_0_p1.bin out.tsv 1600000 1600000 0.5 5 1 30000 0 0 0 1 0 2000
echo ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

