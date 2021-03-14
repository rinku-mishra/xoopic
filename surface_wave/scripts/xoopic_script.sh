#!/bin/sh

echo "serial xoopic execution script"
echo "inputs: surface_wave_prad_5.inp, surface_wave_prad_15.inp, surface_wave_prad_25.inp"

echo "running E5......"
./xoopic -i ../inputs/surface_wave_prad_5.inp -s 100000 -dp 100000 -nox >>xoopic_run.log
echo "E5 run completed"

echo "running E15......"
./xoopic -i ../inputs/surface_wave_prad_15.inp -s 100000 -dp 100000 -nox >>xoopic_run.log
echo "E15 run completed"

#echo "running E25......"
#./xoopic -i ../inputs/surface_wave_prad_25.inp -s 100000 -dp 100000 -nox >>xoopic_run.log
#echo "E25 run completed"

echo "all completed"

